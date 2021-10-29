ENTRY(_start)
SECTIONS
{
	. = 0x200000;
	.init : { *(.init) }
	.text : { *(.text) }
	
	/* ahh, yes. The empty & unused section ld is in love with */
	.data : { *(.data) }
	
	/* and once more with feeling */
	.rodata : { *(.rodata*) }

	.bss : { *(.bss)}

	/* FUCK OFF LD, We don't want your bullshit */
	/DISCARD/ : {*(*)}
}
