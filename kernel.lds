ENTRY(_start)
SECTIONS
{
	. = 0x200000;
	.init : { *(.init) }
	.text : { *(.text) }
	
	/* ahh, yes. The empty & unused section ld is in love with */
	.data : { *(.data) }

	/* FUCK OFF LD, We don't want your bullshit */
	/DISCARD/ : {*(*)}
}