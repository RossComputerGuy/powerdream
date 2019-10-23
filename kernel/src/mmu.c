#include <kernel/mmu.h>
#include <string.h>

static volatile uint32_t* const pteh = (uint32_t*)(0xff000000);
static volatile uint32_t* const ptel = (uint32_t*)(0xff000004);
static volatile uint32_t* const ptea = (uint32_t*)(0xff000034);
static volatile uint32_t* const ttb = (uint32_t*)(0xff000008);
static volatile uint32_t* const tea = (uint32_t*)(0xff00000c);
static volatile uint32_t* const mmucr = (uint32_t*)(0xff000010);

#define SET_PTEH(VA, ASID) do { *pteh = ((VA) & 0xfffffc00) | ((ASID) & 0xff); } while(0)
#define BUILD_PTEH(VA, ASID) (((VA) & 0xfffffc00) | ((ASID) & 0xff))
#define SET_PTEL(PA, V, SZ, PR, C, D, SH, WT) do { *ptel = ((PA) & 0x1ffffc00) | ((V) << 8) | ( ((SZ) & 2) << 6 ) | ( ((SZ) & 1) << 4 ) | ( (PR) << 5 ) | ( (C) << 3 ) | ( (D) << 2 ) | ((SH) << 1) | ((WT) << 0); } while(0)
#define BUILD_PTEL(PA, V, SZ, PR, C, D, SH, WT) (((PA) & 0x1ffffc00) | ((V) << 8) | (((SZ) & 2) << 6) | (((SZ) & 1) << 4) | ((PR) << 5) | ((C) << 3) | ((D) << 2) | ((SH) << 1) | ((WT) << 0))
#define SET_TTB(TTB) do { *ttb = TTB; } while(0)
#define SET_MMUCR(URB, URC, SQMD, SV, TI, AT) do { *mmucr = ((URB) << 18) | ((URC) << 10) | ((SQMD) << 9) | ((SV) << 8) | ((TI) << 2) | ((AT) << 0); } while(0)
#define SET_URC(URC) do { *mmucr = (*mmucr & ~(63 << 10)) | (((URC) & 63) << 10); } while(0)
#define GET_URC() ((*mmucr >> 10) & 63)
#define INCR_URC() do { SET_URC(GET_URC() + 1); } while(0)

pd_mmu_context_t pd_mmu_ctx_curr;

static int mmu_shortcut_ok = 0;
static pd_mmu_mapfunc_t map_func;

/* Physical hardware management */
static void mmu_ldtlb(int asid, uint32_t virt, uint32_t phys, int sz, int pr, int c, int d, int sh, int wt) {
  SET_PTEH(virt, asid);
  SET_PTEL(phys, 1, sz, pr, c, d, sh, wt);
  asm ("ldtlb");
}

static void mmu_ldtlb_quick(uint32_t ptehv, uint32_t ptelv) {
  *pteh = ptehv;
  *ptel = ptelv;
  asm ("ldtlb");
}

static void mmu_ldtlb_wait() {
  asm ("nop");
  asm ("nop");
  asm ("nop");
  asm ("nop");
  asm ("nop");
  asm ("nop");
  asm ("nop");
  asm ("nop");
}

extern void pd_mmu_reset_itlb();

/* Table management */
static pd_mmu_page_t* map_virt(pd_mmu_context_t* context, int virtpage) {
  virtpage = virtpage << PD_MMU_IND_BITS;

  int top = (virtpage >> PD_MMU_TOP_SHIFT) & PD_MMU_TOP_MASK;
  int bot = (virtpage >> PD_MMU_BOT_SHIFT) & PD_MMU_BOT_SHIFT;

  pd_mmu_subcontext_t* sub = &context->sub[top];
  pd_mmu_page_t* page = (pd_mmu_page_t*)(sub + bot);
  if (!page->valid) return NULL;
  return page;
}

void pd_mmu_use_table(pd_mmu_context_t context) {
  memcpy(&pd_mmu_ctx_curr, &context, sizeof(pd_mmu_context_t));
  mmu_shortcut_ok = (map_func == map_virt);
}

void pd_mmu_context_create(pd_mmu_context_t* context, int asid) {
  context->asid = asid;
  for (int i = 0; i < PD_MMU_PAGES; i++) memset(&context->sub[i], 0, sizeof(pd_mmu_subcontext_t));
}

int pd_mmu_virt2phys(pd_mmu_context_t* context, int virtpage) {
  pd_mmu_page_t* page = map_virt(context, virtpage);
  if (page == NULL) return -1;
  return page->physical;
}

void pd_mmu_switch_context(pd_mmu_context_t* context) {
  SET_PTEH(0, context->asid);
}

static void pd_mmu_page_map_single(pd_mmu_context_t* context, int virtpage, int physpage, int prot, int cache, int share, int dirty) {
  virtpage = virtpage << PD_MMU_IND_BITS;

  int top = (virtpage >> PD_MMU_TOP_SHIFT) & PD_MMU_TOP_MASK;
  int bot = (virtpage >> PD_MMU_BOT_SHIFT) & PD_MMU_BOT_SHIFT;

  pd_mmu_subcontext_t* sub = &context->sub[top];
  for (int i = 0; i < PD_MMU_SUB_PAGES; i++) sub[i]->valid = 0;

  pd_mmu_page_t* page = (pd_mmu_page_t*)(sub + bot);

  page->physical = physpage;
  page->prkey = prot;

  switch (cache) {
    case PD_MMU_NO_CACHE:
      page->cache = 0;
      break;
    case PD_MMU_CACHE_BACK:
      page->cache = 0;
      page->wthru = 0;
      break;
    case PD_MMU_CACHE_WT:
      page->cache = 0;
      page->wthru = 1;
      break;
    default:
      page->cache = 0;
      page->wthru = 0;
      break;
  }

  page->dirty = 1;
  page->blank = 0;
  page->shared = share;
  page->valid = 1;
  page->pteh = BUILD_PTEH(virtpage, 0);
  page->ptel = BUILD_PTEL(page->physical << PD_PAGESIZE_BITS, 1, 1, page->prkey, page->cache, page->dirty, page->shared, page->wthru);
}

void pd_mmu_page_map(pd_mmu_context_t* context, int virtpage, int physpage, int count, int prot, int cache, int share, int dirty) {
  while (count > 0) {
    pd_mmu_page_map_single(context, virtpage, physpage, prot, cache, share, dirty);
    virtpage++;
    physpage++;
    count--;
  }
}

int pd_mmu_copyin(pd_mmu_context_t* context, uint32_t srcaddr, uint32_t srccnt, void* buffer) {
  uint32_t srcptr = srcaddr;
  pd_mmu_page_t* srcpage = (pd_mmu_page_t*)srcptr;
  int srckrn = 1;
  uint32_t src = 0;

  if (!(srcptr & 0x8000000)) {
    srcpage = map_virt(context, srcptr >> PD_PAGESIZE_BITS);
    // TODO: if (srcpage == NULL) panic
    src = (srcpage->physical << PD_PAGESIZE_BITS) | (srcptr & PD_PAGEMASK);
    srckrn = 0;
  }

  uint8_t* dst = (uint8_t*)buffer;

  int copied = 0;
  uint32_t run = 0;
  while (srccnt > 0) {
    run = PD_PAGESIZE - (srcptr & PD_PAGEMASK);
    if (srccnt < run) run = srccnt;

    memcpy(dst, (void*)(src | 0x80000000), run);

    src += run;
    srcptr += run;
    dst += run;
    srccnt -= run;

    if (!srckrn & (srcptr & ~PD_PAGEMASK) != ((srcptr - run) & PD_PAGEMASK)) {
      srcpage = map_virt(context, srcptr >> PD_PAGESIZE_BITS);
      // TODO: if (srcpage == NULL) panic
      src = (srcpage->physical << PD_PAGESIZE_BITS) | (srcptr - (srcptr & ~PD_PAGEMASK));
    }

    copied += run;
  }

  return copied;
}

int pd_mmu_copyv(pd_mmu_context_t* context1, struct iovec* iov1, int iovcnt1, pd_mmu_context_t* context2, struct iovec* iov2, int iovcnt2) {
  int srciov = 0;
  uint32_t srccnt = iov1[srciov].iov_len;
  uint32_t srcptr = (uint32_t)iov1[srciov].iov_base;

  pd_mmu_page_t* srcpage = NULL;
  uint32_t src = srcptr;
  int srckrn = 1;
  if(!(srcptr & 0x80000000)) {
    srcpage = map_virt(context1, srcptr >> PD_PAGESIZE_BITS);
    // TODO: if (srcpage == NULL) panic
    src = (srcpage->physical << PD_PAGESIZE_BITS) | (srcptr & PD_PAGEMASK);
    srckrn = 0;
  }

  int dstiov = 0;
  uint32_t dstcnt = iov2[dstiov].iov_len;
  uint32_t dstptr = (uint32_t)iov2[dstiov].iov_base;

  pd_mmu_page_t* dstpage = NULL;
  uint32_t dst = dstptr;
  int dstkrn = 1;
  if (!(dstptr & 0x80000000)) {
    dstpage = map_virt(context2, dstrptr >> PD_PAGESIZE_BITS);
    // TODO: if (dstpage == NULL) panic
    dst = (dstpage->physical << PD_PAGESIZE_BITS) | (dstptr & PD_PAGEMASK);
    dstkrn = 0;
  }

  int copied = 0;
  uint32_t run = 0;
  while (srciov < iovcnt1 && dstiov < iovcnt2) {
    run = PD_PAGESIZE - (srcptr & PD_PAGEMASK);

    if ((PD_PAGESIZE - (dstptr & PD_PAGEMASK)) < run) run = PD_PAGESIZE - (dstptr & PD_PAGEMASK);
    if (srccnt < run) run = srccnt;
    if (dstcnt < run) run = dstcnt;

    memcpy((void*)(dst | 0xa0000000), (void*)(src | 0x80000000), run);
    pd_dcache_inval_range(dst | 0x80000000, run);

    src += run;
    srcptr += run;
    dst += run;
    dstptr += run;
    copied += run;

    srccnt -= run;

    if (srccnt <= 0) {
        srciov++;

        if (srciov >= iovcnt1) break;

        srccnt = iov1[srciov].iov_len;
        srcptr = (uint32_t)iov1[srciov].iov_base;

        if (!srckrn) {
            srcpage = map_virt(context1, srcptr >> PD_PAGESIZE_BITS);
            // TODO: if (srcpage == NULL) panic
            src = (srcpage->physical << PD_PAGESIZE_BITS) | (srcptr & PD_PAGEMASK);
        } else src = srcptr;
    } else {
        if (!srckrn && (srcptr & ~PD_PAGEMASK) != ((srcptr - run) & ~PD_PAGEMASK)) {
            srcpage = map_virt(context1, srcptr >> PD_PAGESIZE_BITS);
            // TODO: if (srcpage == NULL) panic
            src = (srcpage->physical << PD_PAGESIZE_BITS) | (srcptr - (srcptr & ~PD_PAGEMASK));
        }
    }

    dstcnt -= run;

    if (dstcnt <= 0) {
        dstiov++;

        if (dstiov >= iovcnt2) break;

        dstcnt = iov2[dstiov].iov_len;
        dstptr = (uint32_t)iov2[dstiov].iov_base;

        if (!dstkrn) {
          dstpage = map_virt(context2, dstptr >> PD_PAGESIZE_BITS);
          // TODO: if (dstpage == NULL) panic
          dst = (dstpage->physical << PD_PAGESIZE_BITS) | (dstptr & PD_PAGEMASK);
        } else dst = dstptr;
    } else {
      if (!dstkrn && (dstptr & ~PD_PAGEMASK) != ((dstptr - run) & ~PD_PAGEMASK)) {
        dstpage = map_virt(context2, dstptr >> PD_PAGESIZE_BITS);
        // TODO: if (dstpage == NULL) panic
        dst = (dstpage->physical << PD_PAGESIZE_BITS) | (dstptr - (dstptr & ~PD_PAGEMASK));
      }
    }
  }
  return copied;
}

void pd_mmu_init() {
  last_urc = 0;
  map_func = map_virt;
  mmu_shortcut_ok = 0;

  // TODO: install IRQ handlers

  SET_MMUCR(0x3f, 0, 1, 0, 1, 1);

  pd_mmu_reset_itlb();
}