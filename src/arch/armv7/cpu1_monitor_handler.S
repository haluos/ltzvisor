.align 8
.global _cpu1_monitor_vector_table
	/** Monitor Vector Table definition */
_cpu1_monitor_vector_table:
	b	.				@ Not available on MON
	b	.				@ Not available on MON
	b .
	b .
	b .
	b	.				@ Reserved for HYP
	b .
	b .
