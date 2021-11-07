MEMORY
{
    ram : ORIGIN = 0x8000, LENGTH = 0x1000
}

SECTIONS
{
	.init : { *(.init) }
    .text : { *(.text*) }  > ram

	/* ahh, yes. The empty & unused section ld is in love with */
	.data : { *(.data) } > ram
	/* and once more with feeling */
	.rodata : { *(.rodata*) } > ram

    .bss : { *(.bss*) }  > ram
}

