#include "delay.h"

/*do a million nops */
void worstdelayever(int n) {
    n *= 1000000;
    for (int i = 0; i < n; ++i) 
        asm volatile ("nop" : : : );
}