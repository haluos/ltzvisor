/* CPU Operation Modes */
#define	USER_MODE		0x10
#define FIQ_MODE		0x11
#define IRQ_MODE		0x12
#define SVC_MODE		0x13
// MON Not Used
#define ABORT_MODE   		0x17
// HYP Not Used
#define UNDEF_MODE		0x1B
#define SYS_MODE		0x1F


/* CPSR bit assignments */
#define CPSR_I_BIT		(1 << 7) // Mask IRQ
#define CPSR_F_BIT		(1 << 6) // Mask IRQ


/* SCTLR bit assignements */
#define	SCTLR_M_BIT		(1 << 0)
#define	SCTLR_A_BIT		(1 << 1)
#define SCTLR_C_BIT		(1 << 2)
#define SCTLR_Z_BIT		(1 << 11)
#define SCTLR_I_BIT		(1 << 12)
#define SCTLR_U_BIT		(1 << 22)
#define SCTLR_XP_BIT		(1 << 23)


/* Cache (L1 & L2) specific */
#define	CACHE_LEVEL1_SET	1
#define	CACHE_LEVEL2_SET	1
// LEVEL 1 Configuration 32KB, NumSets = 256, Ways=2,
// Line Size = 6 (log2 line_len) and Line len = 64 bytes
#define MAX_L1_CACHE_WAYS	1   	// Ways -1
#define MAX_L1_SETS		255 	// NumSets -1
#define MAX_L1_LINE_LEN		6
// LEVEL 2 Configuration 512 KB, NumSets = 512, Ways=16,
// Line Size = 6 (log2 line_len) and Line len = 64 bytes
#define MAX_L2_CACHE_WAYS	15   	/* Ways -1 */
#define MAX_L2_SETS		255 	/* NumSets -1 */
#define MAX_L2_LINE_LEN		6


/***************** Data Cache enable (L1) *****************/
.global dCache_enable
.func dCache_enable
dCache_enable:
	push	{r0}
	dmb
	mrc	p15, 0, r0, c1, c0, 0	
	orr 	r0, r0, #SCTLR_C_BIT
	mcr 	p15, 0, r0, c1, c0, 0		
	dsb
	pop	{r0}
  	bx      lr

.endfunc	@ dCache_enable


/***************** Data Cache disable (L1) ****************/
.global dCache_disable
.func dCache_disable
dCache_disable:
	push	{r0}
	dmb
	mrc	p15, 0, r0, c1, c0, 0	
	bic 	r0, r0, #SCTLR_C_BIT
	mcr 	p15, 0, r0, c1, c0, 0		
	dsb
	pop	{r0}
  	bx      lr

.endfunc	@ dCache_disable


/***************** Data Cache disable (L1) ****************/
.global dCache_cleanLine_mva
.func dCache_cleanLine_mva
@ void dCache_cleanLine_mva(uint32_t mva);
clean_dcache_line_mva:
	mcr	p15, 0, r0, c7, c10, 1		@ DCCMVAC
    	bx      lr
.endfunc


/************** Data Cache invalidate (L1) ***************/
/* 	r0 - Cache level
	r1 - Log2 Line Length
	r2 - Way
	r3 - LEADING ZEROS of WAY
	r4 - Set
*/
dCache_invalidate_setWay:
	mov	r9, r2
2:
	// val = (cache level << 1) | (set << log2 LINE_LEN) |  (way << LEADING_ZEROS_OF WAY)
	orr	r11, r0, r9, lsl r3
	orr	r11, r11, r4, lsl r1
	mcr 	p15, 0, r11, c7, c6, 2		@ DCISW (Invalidate)
	subs	r9, r9, #1
	bge 	2b
	subs	r4, r4, #1
	bge 	dCache_invalidate_setWay
	bx	lr

.global dCache_invalidate
.func dCache_invalidate
dCache_invalidate:
	push	{r0-r12, lr}
	dmb
	ldr	r0, =CACHE_LEVEL1_SET
	cmp	r0, #1
	bne	1f
	/* Level 1 */
	mov	r0, #0
	mcr	p15, 2, r0, c0, c0, 0
	isb
	ldr	r1, =MAX_L1_LINE_LEN
	ldr	r2, =MAX_L1_CACHE_WAYS
	clz	r3, r2
	ldr	r4, =MAX_L1_SETS
	bl	dCache_invalidate_setWay
	/* Level 2 */
	ldr	r0, =CACHE_LEVEL2_SET
	cmp	r0, #1
	bne	1f
	mov	r0, #2 /* Shift Cache level Id by 1 bit */
	mcr	p15, 2, r0, c0, c0, 0
	isb
	ldr	r1, =MAX_L2_LINE_LEN
	ldr	r2, =MAX_L2_CACHE_WAYS
	clz	r3, r2
	ldr	r4, =MAX_L2_SETS
	bl	dCache_invalidate_setWay
1:
	mov 	r0, #0
	mcr 	p15, 2, r10, c0, c0, 0
	dsb
	isb
	pop	{r0-r12, lr}
	bx	lr

.endfunc	@ dCache_invalidate






/************* Instruction Cache enable (L1) *************/
.global iCache_enable
.func iCache_enable
iCache_enable:
	push	{r0}
	dmb
	mrc	p15, 0, r0, c1, c0, 0	
	orr 	r0, r0, #SCTLR_I_BIT
	mcr 	p15, 0, r0, c1, c0, 0		
	isb
	pop	{r0}
  	bx      lr

.endfunc	@ iCache_enable


/************* Instruction Cache disable (L1) ************/
.global iCache_disable
.func iCache_disable
iCache_disable:
	push	{r0}
	dmb
	mrc	p15, 0, r0, c1, c0, 0	
	bic 	r0, r0, #SCTLR_I_BIT
	mcr 	p15, 0, r0, c1, c0, 0		
	isb
	pop	{r0}
  	bx      lr

.endfunc	@ iCache_disable


/********** Instruction Cache invalidate (L1) ************/
.global iCache_invalidate
.func iCache_invalidate
iCache_invalidate:
	push	{r0}
	dmb
	mov	r0, #0
	mcr 	p15, 0, r0, c7, c5, 0		@ ICIALLU (invalidate all)
	isb
	pop	{r0}
  	bx      lr

.endfunc	@ iCache_invalidate




