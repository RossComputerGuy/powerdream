.text
.align 2
.space 4096
.globl krn_stack
krn_stack:

.globl start
.globl _start
_start:
start:
  mov.l	old_sr_addr,r0
	stc	sr,r1
	mov.l	r1,@r0
	mov.l	init_sr,r0
	ldc	r0,sr

	mov.l	setup_cache_addr,r0
	mov.l	p2_mask,r1
	or	r1,r0
	jmp	@r0
	nop
setup_cache:
	mov.w	ccr_data,r1
	bra	.L_setup_cache_L1
	nop
.L_setup_cache_L1:
	mov.l	ccr_addr,r0
	mov.l	r1,@r0

	nop
	nop
	nop
	nop
	nop
	nop
	mov.l	init_addr,r0
	mov	#0,r1
	jmp	@r0
	mov	r1,r0
	nop

init:
	sts.l	pr,@-r15

	mov.l	old_stack_addr,r0
	mov.l	r15,@r0
	mov.l	new_stack,r15

	mov.l	old_vbr_addr,r0
	stc	vbr,r1
	mov.l	r1,@r0

/*
	mov.l	old_fpscr_addr,r0
	sts	fpscr,r1
	mov.l	r1,@r0

	mov	#4,r4
	mov.l	fpscr_addr,r0
	jsr	@r0
	shll16	r4
*/

	mov	#-1,r14

	mov.l	main_addr,r0
	jsr	@r0
	nop

.align	2
dcload_magic_addr: .long 0x8c004004
dcload_magic_value: .long 0xdeadbeef
dcload_syscall: .long 0x8c004008
__bootstrap_old_sr:
old_sr:	.long	0
__bootstrap_old_vbr:
old_vbr: .long	0
__bootstrap_old_fpscr:
init_sr: .long 0x500000f0
old_sr_addr: .long old_sr
old_vbr_addr: .long old_vbr
/*old_fpscr_addr: .long old_fpscr
fpscr_addr: .long	___set_fpscr	! in libgcc
old_fpscr: .long 0*/
old_stack_addr:	.long	old_stack
__bootstrap_old_stack: old_stack: .long 0
new_stack: .long 0x8d000000
p2_mask: .long 0xa0000000
setup_cache_addr:	.long	setup_cache
init_addr: .long init
main_addr: .long bootstrap_main
mmu_addr:	.long	0xff000010
ccr_addr:	.long	0xff00001c
ccr_data:	.word	0x090d
ccr_data_ocram:	.word	0x092d
