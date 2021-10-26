ENTRY(_start)
SECTIONS
{
	. = 0x200000;
	.init : { *(.init) }
	.text : { *(.text) }
}