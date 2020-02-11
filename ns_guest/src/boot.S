
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

#define SHAREABLE_DDR


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

.set TblBase ,		MMUTable
.set CRValMmuCac,	0b01000000000101
.set FPEXC_EN,		0x40000000

.set L2CCAuxControl,	0x72360000		/* Enable all prefetching, Cache replacement policy, Parity enable,
                                        Event monitor bus enable and Way Size (64 KB) */
.set L2CCControl,	0x01			/* Enable L2CC */
.set L2CCTAGLatency,	0x0111			/* latency for TAG RAM */
.set L2CCDataLatency,	0x0121			/* latency for DATA RAM */

.set SLCRlockKey,	        0x767B			/* SLCR lock key */
.set SLCRUnlockKey,	        0xDF0D			/* SLCR unlock key */
.set SLCRL2cRamConfig,      0x00020202      /* SLCR L2C ram configuration */

.set LRemap,		0xFE00000F		/* set the base address of the peripheral block as not shared */
.set L2CCWay,		(PSS_L2CC_BASE_ADDR + 0x077C)	/*(PSS_L2CC_BASE_ADDR + PSS_L2CC_CACHE_INVLD_WAY_OFFSET)*/
.set L2CCSync,		(PSS_L2CC_BASE_ADDR + 0x0730)	/*(PSS_L2CC_BASE_ADDR + PSS_L2CC_CACHE_SYNC_OFFSET)*/
.set L2CCCrtl,		(PSS_L2CC_BASE_ADDR + 0x0100)	/*(PSS_L2CC_BASE_ADDR + PSS_L2CC_CNTRL_OFFSET)*/
.set L2CCAuxCrtl,	(PSS_L2CC_BASE_ADDR + 0x0104)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_AUX_CNTRL_OFFSET)*/
.set L2CCTAGLatReg,	(PSS_L2CC_BASE_ADDR + 0x0108)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_TAG_RAM_CNTRL_OFFSET)*/
.set L2CCDataLatReg,	(PSS_L2CC_BASE_ADDR + 0x010C)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_DATA_RAM_CNTRL_OFFSET)*/
.set L2CCIntClear,	(PSS_L2CC_BASE_ADDR + 0x0220)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_IAR_OFFSET)*/
.set L2CCIntRaw,	(PSS_L2CC_BASE_ADDR + 0x021C)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_ISR_OFFSET)*/

.set SLCRlockReg,	    (PSS_SLCR_BASE_ADDR + 0x04)	/*(PSS_SLCR_BASE_ADDR + XPSS_SLCR_LOCK_OFFSET)*/
.set SLCRUnlockReg,     (PSS_SLCR_BASE_ADDR + 0x08)	/*(PSS_SLCR_BASE_ADDR + XPSS_SLCR_UNLOCK_OFFSET)*/
.set SLCRL2cRamReg,     (PSS_SLCR_BASE_ADDR + 0xA1C) /*(PSS_SLCR_BASE_ADDR + XPSS_SLCR_L2C_RAM_OFFSET)*/
.set SLCRCPURSTReg,     (0xF8000000 + 0x244)           /*(XPS_SYS_CTRL_BASEADDR + A9_CPU_RST_CTRL_OFFSET)*/
.set EFUSEStaus,        (0xF800D000 + 0x10)            /*(XPS_EFUSE_BASEADDR + EFUSE_STATUS_OFFSET)*/

.set PSS_L2CC_BASE_ADDR, 0xF8F02000
.set PSS_SLCR_BASE_ADDR, 0xF8000000

.global main
.global ctor_init
.global _vector_table

.code 32


.section ".startup", "ax"
.global _startp
.func _startp
_startp:

/* copy coprocessor CP15 register to r0 and extract CPU ID */
run_cpu0:
	mrc	p15, 0, r0, c0, c0, 5	@ MPIDR
	and	r0, r0, #0x03		@ Mask, leaving only CPU id
	cmp	r0, #0x00		@ CPU0?
	bne	stuck_loop


	/** Set VBAR */
vector_table:
	//ldr	r0, =vectors
	ldr	r0, =_vector_table
	mcr	p15, 0, r0, c12, c0, 0

#ifdef SHAREABLE_DDR
	/* Mark the entire DDR memory as shareable */
	ldr	r3, =0x3ff			/* 1024 entries to cover 1G DDR */
	ldr	r0, =TblBase			/* MMU Table address in memory */
	ldr	r2, =0x15de6			/* S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 */
shareable_loop:
	str	r2, [r0]			/* write the entry to MMU table */
	add	r0, r0, #0x4			/* next entry in the table */
	add	r2, r2, #0x100000		/* next section */
	subs	r3, r3, #1
	bge	shareable_loop			/* loop till 1G is covered */
#endif

/********************** Init stacks **********************/
init_stacks:

	@ IRQ mode
   	msr	CPSR_c, #(IRQ_MODE | CPSR_I_BIT | CPSR_F_BIT)
    	ldr     sp, =__irq_stack_top

	@ ABORT mode
   	msr	CPSR_c, #(ABORT_MODE | CPSR_I_BIT | CPSR_F_BIT)
    	ldr     sp, =__abort_stack_top

	@ UNDEF mode
   	msr	CPSR_c, #(UNDEF_MODE | CPSR_I_BIT | CPSR_F_BIT)
    	ldr     sp, =__undef_stack

	@ USER/SYS mode
   	msr	CPSR_c, #(SYS_MODE | CPSR_F_BIT)
    	ldr     sp, =__sys_stack_top

	@ SVC 	mode
   	msr	CPSR_c, #(SVC_MODE | CPSR_F_BIT)
    	ldr     sp, =__svc_stack_top


/****** Init memory (MMU, cache, branch prediction) ******/
init_memory_sys:

	@ MMU
	bl	mmu_disable
	//bl mmu_enable

	/* MMU now enabled - Virtual Address Space active */

	@ Branch Prediction
	bl	branchPrediction_disable

	@ L1 data Cache
	bl	dCache_disable
	//bl	dCache_enable

	@ L1 instruction Cache
	bl	iCache_disable
	//bl	iCache_enable


	ldr	r0,=TblBase			/* Load MMU translation table base */
	orr	r0, r0, #0x5B			/* Outer-cacheable, WB */
	mcr	p15, 0, r0, c2, c0, 0		/* TTB0 */

	mvn	r0,#0				/* Load MMU domains -- all ones=manager */
	mcr	p15,0,r0,c3,c0,0

	//bl mmu_enable

	ldr	r0,=CRValMmuCac
	mcr	p15,0,r0,c1,c0,0		/* Enable cache and MMU */
	dsb					/* dsb	allow the MMU to start up */
	isb					/* isb	flush prefetch buffer */

	/* Write to ACTLR */
	mrc	p15, 0, r0, c1, c0, 1		/* Read ACTLR*/
	orr	r0, r0, #(0x01 << 6)		/* set SMP bit */
	orr	r0, r0, #(0x01 )		/* Cache/TLB maintenance broadcast */
	mcr	p15, 0, r0, c1, c0, 1		/* Write ACTLR*/

	ldr	r0,=L2CCCrtl			/* Load L2CC base address base + control register */
	mov	r1, #0				/* force the disable bit */
	str	r1, [r0]			/* disable the L2 Caches */

	ldr	r0,=L2CCAuxCrtl			/* Load L2CC base address base + Aux control register */
	ldr	r1,[r0]				/* read the register */
	ldr	r2,=L2CCAuxControl		/* set the default bits */
	orr	r1,r1,r2
	str	r1, [r0]			/* store the Aux Control Register */

	ldr	r0,=L2CCTAGLatReg		/* Load L2CC base address base + TAG Latency address */
	ldr	r1,=L2CCTAGLatency		/* set the latencies for the TAG*/
	str	r1, [r0]			/* store the TAG Latency register Register */

	ldr	r0,=L2CCDataLatReg		/* Load L2CC base address base + Data Latency address */
	ldr	r1,=L2CCDataLatency		/* set the latencies for the Data*/
	str	r1, [r0]			/* store the Data Latency register Register */

	ldr	r0,=L2CCWay			/* Load L2CC base address base + way register*/
	ldr	r2, =0xFFFF
	str	r2, [r0]			/* force invalidate */

	ldr	r0,=L2CCSync			/* need to poll 0x730, PSS_L2CC_CACHE_SYNC_OFFSET */
						/* Load L2CC base address base + sync register*/
	/* poll for completion */
Sync:	ldr	r1, [r0]
	cmp	r1, #0
	bne	Sync

	ldr	r0,=L2CCIntRaw			/* clear pending interrupts */
	ldr	r1,[r0]
	ldr	r0,=L2CCIntClear
	str	r1,[r0]

	ldr	r0,=SLCRUnlockReg		/* Load SLCR base address base + unlock register */
	ldr	r1,=SLCRUnlockKey	    	/* set unlock key */
	str	r1, [r0]		    	/* Unlock SLCR */

	ldr	r0,=SLCRL2cRamReg		/* Load SLCR base address base + l2c Ram Control register */
	ldr	r1,=SLCRL2cRamConfig        	/* set the configuration value */
	str	r1, [r0]	        	/* store the L2c Ram Control Register */

	ldr	r0,=SLCRlockReg         	/* Load SLCR base address base + lock register */
	ldr	r1,=SLCRlockKey	        	/* set lock key */
	str	r1, [r0]	        	/* lock SLCR */

	ldr	r0,=L2CCCrtl			/* Load L2CC base address base + control register */
	ldr	r1,[r0]				/* read the register */
	mov	r2, #L2CCControl		/* set the enable bit */
	orr	r1,r1,r2
	str	r1, [r0]			/* enable the L2 Caches */

	mov	r0, r0
	mrc	p15, 0, r1, c1, c0, 2		/* read cp access control register (CACR) into r1 */
	orr	r1, r1, #(0xf << 20)		/* enable full access for p10 & p11 */
	mcr	p15, 0, r1, c1, c0, 2		/* write back into CACR */

	fmrx	r1, FPEXC			/* read the exception register */
	orr	r1,r1, #FPEXC_EN		/* set VFP enable bit, leave the others in orig state */
	fmxr	FPEXC, r1			/* write back the exception register */

	mrc	p15,0,r0,c1,c0,0		/* flow prediction enable */
	orr	r0, r0, #(0x01 << 11)		/* #0x8000 */
	mcr	p15,0,r0,c1,c0,0

	mrc	p15,0,r0,c1,c0,1		/* read Auxiliary Control Register */
	orr	r0, r0, #(0x1 << 2)		/* enable Dside prefetch */
	orr	r0, r0, #(0x1 << 1)		/* enable L2 Prefetch hint */
	mcr	p15,0,r0,c1,c0,1		/* write Auxiliary Control Register */

	//mrs	r0, cpsr			/* get the current PSR */
	//bic	r0, r0, #0x100			/* enable asynchronous abort exception */
	//msr	cpsr_xsf, r0



/********************** Enable FPU **********************/
enable_fpu:

	/********* TODO *********/


/********************** Clear bss **********************/
zero_bss:
	bl	clear_bss


/************** Disable strict align check *************/
strick_align_check:
	bl	strickAlignCheck_disable


/********************** Call main **********************/
call_main:
	ldr     r12, =main
	blx	r12

	@ Should never return here!
    	b	.

.endfunc	@_startp


/********************** Stuck loop **********************/
stuck_loop:
	b	stuck_loop

/**********************************************************************************
 * Clear bss
 * This function is responsible for clearing (zero init) the *.bss section
 **********************************************************************************/
.global clear_bss
.func clear_bss
clear_bss:
	push	{r1-r3}
	ldr	r1,=__bss_start__
	ldr     r2,=__bss_end__
	mov     r3,#0
1:
	cmp     r1,r2
	stmltia r1!,{r3}
	blt     1b
	pop	{r1-r3}
	bx	lr

.endfunc	@clear_bss

.end
