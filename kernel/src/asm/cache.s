.text

.globl pd_icache_flush_range
pd_icache_flush_range:
	mov.l	fraddr,r0
	mov.l	p2mask,r1
	or r1,r0
	jmp	@r0
  nop
.align 2
fraddr:	.long	pd_flush_real
p2mask:	.long	0x20000000

pd_flush_real:
  stc	sr,r0
	mov.l	r0,@-r15
	mov.l	ormask,r1
	or	r1,r0
	ldc	r0,sr

  add	r4,r5
	mov.l	l1align,r0
	and	r0,r4
	mov.l	addrarray,r1
	mov.l	entrymask,r2
	mov.l	validmask,r3

pd_flush_loop:
  ocbwb	@r4

  mov	r4,r6
	and	r2,r6
	or	r1,r6	

	mov	r4,r7
	and	r3,r7

	add	#32,r4
	cmp/hs	r4,r5
	bt/s pd_flush_loop
	mov.l	r7,@r6

  mov.l	@r15+,r0
	ldc	r0,sr

  nop
	nop
	nop
	nop
	nop
	nop
	nop
	rts
	nop

.align 2
ormask: .long 0x100000f0
addrarray: .long 0xf0000000
entrymask: .long 0x1fe0
validmask: .long 0xfffffc00

.globl pd_dcache_inval_range
pd_dcache_inval_range:
	add	r4,r5
	mov.l	l1align,r0
	and	r0,r4

pd_dinval_loop:
  ocbi @r4
	
	mov	#0x10,r0
	shll8	r0
	or	r4,r0
	ocbi	@r0
	
	mov	#0x20,r0
	shll8	r0
	or	r4,r0
	ocbi	@r0
	
	mov	#0x30,r0
	shll8	r0
	or	r4,r0
	ocbi	@r0
	
	cmp/hs r4,r5
	bt/s pd_dinval_loop
	add	#32,r4

	rts
	nop

.globl pd_dcache_flush_range
pd_dcache_flush_range:
	add	r4,r5
	mov.l	l1align,r0
	and	r0,r4

pd_dflush_loop:
  ocbwb	@r4

	mov	#0x10,r0
	shll8	r0
	or	r4,r0
	ocbwb	@r0
	
	mov	#0x20,r0
	shll8	r0
	or	r4,r0
	ocbwb	@r0
	
	mov	#0x30,r0
	shll8	r0
	or	r4,r0
	ocbwb	@r0
	
	cmp/hs r4,r5
	bt/s pd_dflush_loop
	add	#32,r4

	rts
	nop

.align 2
l1align: .long ~31
