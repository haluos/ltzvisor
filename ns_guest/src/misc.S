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


/**********************************************************************************
 * Branch Prediction
 * Branch Prediction support functions
 **********************************************************************************/

/**************** Branch Prediction enable ****************/
.global branchPrediction_enable
.func branchPrediction_enable
branchPrediction_enable:
	push	{r0}
	dmb
	mrc	p15, 0, r0, c1, c0, 0			@ Read SCTLR
	orr     r0, r0, #SCTLR_Z_BIT			@ Set Z bit (bit 11)
	mcr     p15, 0,r0, c1, c0, 0                  	@ Write SCTLR
	pop	{r0}
  	bx      lr

.endfunc	@ branchPrediction_enable


/**************** Branch Prediction disable ***************/
.global branchPrediction_disable
.func branchPrediction_disable
branchPrediction_disable:
	push	{r0}
	dmb
	mrc     p15, 0, r0, c1, c0, 0                 	@ Read SCTLR
	bic     r0, r0, #SCTLR_Z_BIT                    @ Clear Z bit (bit 11)
	mcr     p15, 0,r0, c1, c0, 0                  	@ Write SCTLR
	pop	{r0}
	bx      lr
 
.endfunc	@ branchPrediction_disable
  

/****** Branch Prediction traget cache invalidation *******/
.global branchTCache_invalidate
.func branchTCache_invalidate
branchTCache_invalidate:
	push	{r0}
	dmb
	mov     r0, #0
	mcr     p15, 0, r0, c7, c5, 6                 	@ BPIALL
	dsb
	isb
	pop	{r0}
	bx      lr

.endfunc	@ branchTCache_invalidate



/**********************************************************************************
 * Strict Align Check
 * Strict Align Check support functions
 **********************************************************************************/

/**************** Strick align check enable ****************/
.global strickAlignCheck_enable
.func strickAlignCheck_enable
@ void strickAlignCheck_enable(void);
strickAlignCheck_enable:
	push	{r0}
	dmb
	mrc	p15, 0, r0, c1, c0, 0	
	orr 	r0, r0, #SCTLR_A_BIT		@ Set A bit (bit 1)
	mcr 	p15, 0, r0, c1, c0, 0		
	dsb
	pop	{r0}
  	bx      lr

.endfunc	@ strickAlignCheck_enable


/*************** Strick align check disable ***************/
.global strickAlignCheck_disable
.func strickAlignCheck_disable
@ void strickAlignCheck_disable(void);
strickAlignCheck_disable:
	push	{r0}
	dmb
	mrc	p15, 0, r0, c1, c0, 0	
	bic 	r0, r0, #SCTLR_A_BIT		@ Clear A bit (bit 1)
	mcr 	p15, 0, r0, c1, c0, 0		
	dsb
	pop	{r0}
  	bx      lr

.endfunc	@ strickAlignCheck_disable


