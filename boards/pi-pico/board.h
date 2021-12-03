

/*
 * Memory Map
 */

#define SRAM_BASE 0x10000000
#define VECTORS_BASE 0x10000100

#define INIT_MAIN_STACK 0x20001000
#define INIT_PROCESS_STACK 0x20002000
#define SYSTICK 0xE000E010

#define SYS_CTL_VTOR 0xE000ED08

#define ISR_RETURN_HANDLER 0xFFFFFFF1
#define ISR_RETURN_THREAD_MAIN 0xFFFFFFF9
#define ISR_RETURN_THREAD_PROC 0xFFFFFFFD


/*
 * CPU constants
 */

#define CONTROL_UNPRIV 1
#define CONTROL_PROC_STACK 2

#define REG_ALIAS_RW_BITS  (0x0u << 12u)
#define REG_ALIAS_XOR_BITS (0x1u << 12u)
#define REG_ALIAS_SET_BITS (0x2u << 12u)
#define REG_ALIAS_CLR_BITS (0x3u << 12u)

#define hw_set_alias_untyped(addr) ((void *)(REG_ALIAS_SET_BITS | (uintptr_t)(addr)))
#define hw_clear_alias_untyped(addr) ((void *)(REG_ALIAS_CLR_BITS | (uintptr_t)(addr)))
#define hw_xor_alias_untyped(addr) ((void *)(REG_ALIAS_XOR_BITS | (uintptr_t)(addr)))

// Typed conversion alias pointer generation macros
#define hw_set_alias(p) ((typeof(p))hw_set_alias_untyped(p))
#define hw_clear_alias(p) ((typeof(p))hw_clear_alias_untyped(p))
#define hw_xor_alias(p) ((typeof(p))hw_xor_alias_untyped(p))


#ifndef __ASSEMBLER__

void _start(void);

typedef u16 default_instruction;

#define TO_DEFAULT_FUNC_PTR(PTR) \
	(((u32) PTR) | 1)

#else

#endif