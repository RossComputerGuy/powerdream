.globl pd_irq_save_regs
pd_irq_save_regs:
  mov.l pd_irq_srt_addr,r0
	add	#0x20,r0
	stc.l	r7_bank,@-r0
	stc.l	r6_bank,@-r0
	stc.l	r5_bank,@-r0
	stc.l	r4_bank,@-r0
	stc.l	r3_bank,@-r0
	stc.l	r2_bank,@-r0
	stc.l	r1_bank,@-r0
	stc.l	r0_bank,@-r0
	
	mov.l	r8,@(0x20,r0)
	mov.l	r9,@(0x24,r0)
	mov.l	r10,@(0x28,r0)
	mov.l	r11,@(0x2c,r0)
	mov.l	r12,@(0x30,r0)
	mov.l	r13,@(0x34,r0)
	mov.l	r14,@(0x38,r0)
	mov.l	r15,@(0x3c,r0)
	add	#0x5c,r0
	stc.l	ssr,@-r0
	sts.l	macl,@-r0
	sts.l	mach,@-r0
	stc.l	vbr,@-r0
	stc.l	gbr,@-r0
	sts.l	pr,@-r0	
	stc.l	spc,@-r0
	
	add	#0x60,r0
	add	#0x44,r0
	sts.l	fpul,@-r0
	sts.l	fpscr,@-r0
	mov	#0,r2
	lds	r2,fpscr
	fmov.s fr15,@-r0
	fmov.s fr14,@-r0
	fmov.s fr13,@-r0
	fmov.s fr12,@-r0
	fmov.s fr11,@-r0
	fmov.s fr10,@-r0
	fmov.s fr9,@-r0
	fmov.s fr8,@-r0
	fmov.s fr7,@-r0
	fmov.s fr6,@-r0
	fmov.s fr5,@-r0
	fmov.s fr4,@-r0
	fmov.s fr3,@-r0
	fmov.s fr2,@-r0
	fmov.s fr1,@-r0

.globl pd_irq_force_return
pd_irq_force_return:
	mov.l	_pd_irqfr_or,r1
	stc	sr,r0
	or	r1,r0
	ldc	r0,sr
	bra	pd_save_regs_finish
	nop
	
.align 2
_pd_irqfr_or: .long 0x20000000
pd_stkaddr: .long krn_stack

.globl pd_save_regs_finish
pd_save_regs_finish:
	mov.l	pd_irq_srt_addr, r1
	ldc.l	@r1+,r0_bank
	ldc.l	@r1+,r1_bank
	ldc.l	@r1+,r2_bank
	ldc.l	@r1+,r3_bank
	ldc.l	@r1+,r4_bank
	ldc.l	@r1+,r5_bank
	ldc.l	@r1+,r6_bank
  ldc.l	@r1+,r7_bank
	add	#-32,r1
	mov.l	@(0x20,r1), r8
	mov.l	@(0x24,r1), r9
	mov.l	@(0x28,r1), r10
	mov.l	@(0x2c,r1), r11
	mov.l	@(0x30,r1), r12
	mov.l	@(0x34,r1), r13
	mov.l	@(0x38,r1), r14
	mov.l	@(0x3c,r1), r15
	
	add	#0x40,r1
	ldc.l	@r1+,spc
	lds.l	@r1+,pr	
	ldc.l	@r1+,gbr
	add	#4,r1
	lds.l	@r1+,mach
	lds.l	@r1+,macl
	ldc.l	@r1+,ssr

	mov	#0,r2	
	lds	r2,fpscr
	frchg
	fmov.s	@r1+,fr0
	fmov.s	@r1+,fr1
	fmov.s	@r1+,fr2
	fmov.s	@r1+,fr3
	fmov.s	@r1+,fr4
	fmov.s	@r1+,fr5
	fmov.s	@r1+,fr6
	fmov.s	@r1+,fr7
	fmov.s	@r1+,fr8
	fmov.s	@r1+,fr9
	fmov.s	@r1+,fr10
	fmov.s	@r1+,fr11
	fmov.s	@r1+,fr12
	fmov.s	@r1+,fr13
	fmov.s	@r1+,fr14
	fmov.s	@r1+,fr15
	frchg
	fmov.s	@r1+,fr0
	fmov.s	@r1+,fr1
	fmov.s	@r1+,fr2
	fmov.s	@r1+,fr3
	fmov.s	@r1+,fr4
	fmov.s	@r1+,fr5
	fmov.s	@r1+,fr6
	fmov.s	@r1+,fr7
	fmov.s	@r1+,fr8
	fmov.s	@r1+,fr9
	fmov.s	@r1+,fr10
	fmov.s	@r1+,fr11
	fmov.s	@r1+,fr12
	fmov.s	@r1+,fr13
	fmov.s	@r1+,fr14
	fmov.s	@r1+,fr15
	lds.l	@r1+,fpscr
	lds.l	@r1+,fpul

	mov	#2,r0

	rte
	nop
	
.align 2
.globl pd_irq_srt_addr
pd_irq_srt_addr: .long 0
pd_hdl_except: .long pd_irq_handle_exception

.text
.align 2
pd_tlb_miss_hnd:
	mov	#-1,r3
	shll16	r3
	shll8	r3
	add	#0x24,r3
	mov.l	@r3,r0

	mov	#0x40,r1

	cmp/eq	r0,r1
	bt.s pd_tmh_doit
	mov	#0x60,r1

	cmp/eq	r0,r1
	bt pd_tmh_doit

	bra pd_irq_save_regs
	mov	#2,r4

pd_tmh_doit:
	mov.l pd_tmh_shortcut_addr,r0
	mov.l	@r0,r0
	cmp/pz r0
	bt pd_tmh_clear
	bra pd_irq_save_regs
	mov	#2,r4

pd_tmh_clear:
	mov.l	pd_tmh_stack_save_addr,r0
	mov.l	r15,@r0
	mov.l	pd_tmh_temp_stack_addr,r15

	mov	#0,r4
	mov	#0,r5
	mov.l	pd_tmh_gen_miss_addr,r0
	jsr	@r0
	mov	#0,r6

	mov.l	pd_tmh_stack_save,r15

	rte
	nop

.align 2
pd_tmh_shortcut_addr: .long pd_mmu_shortcut_ok
pd_tmh_stack_save_addr: .long pd_tmh_stack_save
pd_tmh_stack_save: .long 0
pd_tmh_temp_stack_addr: .long pd_tmh_temp_stack
pd_tmh_gen_miss_addr: .long pd_mmu_gen_tlb_miss

.data
.space	256
pd_tmh_temp_stack:

.text
	.align 2
	.globl _pd_irq_vma_table
_pd_irq_vma_table:
	.rep 0x100
	.byte	0
	.endr
	
_pd_vma_table_100:
	nop
	bra	pd_irq_save_regs
	mov	#1,r4
	
	.rep 0x300 - 6
	.byte	0
	.endr

_pd_vma_table_400:
	nop
	bra	pd_irq_save_regs
	mov	#2,r4

	.rep 0x200 - 6
	.byte	0
  .endr

_pd_vma_table_600:
	nop
	bra	pd_irq_save_regs
	mov	#3,r4

.globl pd_irq_disable
pd_irq_disable:
	mov.l	_pd_irqd_and,r1
	mov.l	_pd_irqd_or,r2
	stc	sr,r0
	and	r0,r1
	or r2,r1
	ldc	r1,sr
	rts
	nop
	
.align 2
_pd_irqd_and: .long 0xefffff0f
_pd_irqd_or: .long	0x000000f0

.globl _pd_irq_enable
_pd_irq_enable:
	mov.l	_pd_irqe_and,r1
	stc	sr,r0
	and	r0,r1
	ldc	r1,sr
	rts
	nop
	
.align 2
_pd_irqe_and: .long 0xefffff0f

.globl _pd_irq_restore
_pd_irq_restore:
	ldc	r4,sr
	rts
	nop

.globl _pd_irq_get_sr
_pd_irq_get_sr:
	rts
  stc sr,r0
