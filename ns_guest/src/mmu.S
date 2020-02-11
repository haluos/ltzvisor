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


/********************** Enable MMU ************************/
.globl mmu_enable
.func mmu_enable
@ void mmu_enable(void);
mmu_enable:
	mrc     p15, 0, r1, c1, c0, 0		@ Read SCTLR
	orr     r1, r1, #SCTLR_M_BIT       	@ Set M bit (MMU)
    	orr    	r1, r1, #SCTLR_U_BIT          	@ Set U bit (Unaligned data access)
    	orr    	r1, r1, #SCTLR_XP_BIT         	@ Set XP bit (Extended page tables)
	mcr     p15, 0, r1, c1, c0, 0  		@ Write SCTLR
	dsb
	isb
	bx	lr

.endfunc	@ enable


/********************** Disable MMU ***********************/
.globl mmu_disable
.func mmu_disable
@ void mmu_disable(void);
mmu_disable:
	mrc     p15, 0, r1, c1, c0, 0		@ Read SCTLR
	bic     r1, r1, #SCTLR_M_BIT       	@ Set M bit (MMU)
    bic    	r1, r1, #SCTLR_U_BIT          	@ Set U bit (Unaligned data access)
    bic    	r1, r1, #SCTLR_XP_BIT         	@ Set XP bit (Extended page tables)
	mcr     p15, 0, r1, c1, c0, 0  		@ Write SCTLR
	dsb
	isb
	bx	lr

.endfunc	@ disable


/********** Set Translation Table Base Register ***********/
.globl mmu_ttbr0_set
.func mmu_ttbr0_set
@ void mmu_ttbr0_set(uint32_t ttbr0_val);
@ r0 - ttbr0_val
mmu_ttbr0_set:
	mcr    p15, 0, r0, c2, c0, 0	@ Write TTBR0
	dsb
	isb
	bx	lr

.endfunc	@ mmu_ttbr0_set


/*********** Set Domain Access Control Register ***********/
.globl mmu_domainAccess_set
.func mmu_domainAccess_set
@ void mmu_domainAccess_set(uint32_t access_domain);
@ r0 - access_domain
mmu_domainAccess_set:
	mcr    p15, 0, r0, c3, c0, 0	@ Write DACR
    	dsb
	isb
	bx	lr

.endfunc	@ mmu_domainAccess_set


/********************* Invalidate TLBs ********************/
.globl mmu_tlbs_invalidate
.func mmu_tlbs_invalidate
@ void mmu_tlbs_invalidate(void);
mmu_tlbs_invalidate:
	mov    r0, #0
	mcr    p15, 0, r0, c8, c7, 0	@ TLBIALL
    	dsb
	isb
	bx	lr

.endfunc	@ mmu_tlbs_invalidate



