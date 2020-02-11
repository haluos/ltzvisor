

/* The OUTPUT_FORMAT: names the BFD format to use for the output file. */
OUTPUT_FORMAT ("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")

/* Entry point */
/* ENTRY(_startp) */
ENTRY(_vector_table)

/* STACK SIZES */
IRQ_STACK_SIZE = 4k;
FIQ_STACK_SIZE = 4k;
SVC_STACK_SIZE = 4k;
ABT_STACK_SIZE = 4k;
UND_STACK_SIZE = 4k;
SYS_STACK_SIZE = 4k;

/* HEAP SIZE */
HEAP_SIZE = 1M;

STACK_SIZE = 1M;


MEMORY
{

	/************************* DDR RAM ************************/
    TABLE (rwx): ORIGIN = 0x00000000, LENGTH = 0x00030000
	DDR  (rwx)  : ORIGIN = 0x00100000, LENGTH = 0x3FF00000
}


SECTIONS
{

	/************************* DDR RAM ************************/

	/* Startup Code */
	.startup : ALIGN(32) {
		__startup_start = .;
      KEEP (*(.vectors))
			*(.startup)
			*(.startup.secondary)
		__startup_end = .;
	} > DDR

	.vectors : ALIGN(32) {
		__vectors_start = .;
			*(.vectors)
		__vectors_end = .;
	} > DDR

	/* Exceptions Code */
	.exceptions : ALIGN(32) {
		__exceptions_start = .;
			*(.exceptions)
		__exceptions_end = .;
	} > DDR

	/* Text section */
	.text : ALIGN(32) {
		__text_start = .;
			*(.text)
			*(.text.*)
		__text_end = .;
	} > DDR

	/* Data section: data and read only data */
	.data : ALIGN(32) {
		__data_start = .;
			*(.data)
			*(.data.*)
			*(.rodata)
			*(.rodata.*)
		__data_end = .;
	} > DDR

	.mmu_tbl (ALIGN(16384)) : {
	   __mmu_tbl_start = .;
	   *(.mmu_tbl)
	   __mmu_tbl_end = .;
	} > DDR

	/* Uninitialized data section */
	.bss : ALIGN(32) {
		__bss_start__ = .;
			*(.bss)
			*(.bss.*)
			*(COMMON)
		__bss_end__ = .;
	} > DDR

	/* Stack section */
	.stacks : ALIGN(32) {
    _stack_end = .;
    . += STACK_SIZE;
    __stack = .;
    __stack_low = . ;
		. += ABT_STACK_SIZE;
			. = ALIGN (8);
		__abort_stack_top = .;
    __abort_stack = .;

		. += UND_STACK_SIZE;
			. = ALIGN (8);
		__undef_stack = . ;

		. += IRQ_STACK_SIZE;
			. = ALIGN (8);
		__irq_stack_top = . ;
    __irq_stack = .;

		. += FIQ_STACK_SIZE;
			. = ALIGN (8);
		__fiq_stack_top = . ;
    __fiq_stack = .;

    . += SVC_STACK_SIZE;
			. = ALIGN (8);
		__svc_stack_top = . ;
    __supervisor_stack = .;

		. += SYS_STACK_SIZE;
			. = ALIGN (8);
		__sys_stack_top = . ;

		__stack_high = .;
	} > DDR

	/* Heap section */
	.heap : ALIGN(32) {
		_heap_start = .;
		HeapBase = .;
			. += HEAP_SIZE;
			/* LONG(0xFFFFFFFF) */
		_heap_end = .;
		HeapLimit = .;
	} > DDR

	_end = .;
}
