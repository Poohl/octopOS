
.section .init, "ax"
.global _start
_start:
        ldr     SP,=0x00270000  // and we have a stack... randomly sitting in the SRAM
        bl      c_entry
        
.Lend:
        b       .Lend           // nirvana-loop
