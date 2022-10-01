// Based on code from https://github.com/ucb-bar/hwacha-template/blob/master/tests/rocc.h

#ifndef C_FLUSH_H
#define C_FLUSH_H

#include <stdint.h>
#include <assert.h>

#define STR1(x) #x
#ifndef STR
#define STR(x) STR1(x)
#endif

#define CFLUSH_D_L1   0xCC000073 //Original opcode is 0xFC000073
#define CDISCARD_D_L1 0xCC200073

#define CUSTOMX(X, xd, xs1, xs2, rd, rs1, rs2) 		  \
  (X)                                   |             \
  (rd                 << (7))           |             \
  (xs2                << (7+5))         |             \
  (xs1                << (7+5+1))       |             \
  (xd                 << (7+5+2))       |             \
  (rs1                << (7+5+3))       |             \
  (rs2                << (7+5+3+5))


#define CFLUSH_D_L1_ALL() CUSTOMX(CFLUSH_D_L1,0,0,0,0,0,0)
#define CFLUSH_D_L1_REG(rs1) CUSTOMX(CFLUSH_D_L1,0,0,0,0,rs1,0)
#define CDISCARD_D_L1_REG(rs1) CUSTOMX(CDISCARD_D_L1,0,0,0,0,rs1,0)

#define FLUSH_D_ALL() { 								   \
    asm volatile(".word " STR(CFLUSH_D_L1_ALL()) "\n\t" :: \
				 : "memory");                              \
}


//rs1 is address
//rs1_n is the register number to use
#define CFLUSH_D_L1_INST(rs1, rs1_n) {                                          \
	register uint64_t rs1_ asm("x" #rs1_n) = (uint64_t) rs1;                    \
	asm volatile(".word " STR(CFLUSH_D_L1_REG(rs1_n)) "\n\t" ::[_rs1] "r"(rs1_) \
			     : "memory");                                                   \
}

#define FLUSH_D_REG(rs1) CFLUSH_D_L1_INST(rs1, 13)

#define CDISCARD_D_L1_INST(rs1, rs1_n) {                                          \
	register uint64_t rs1_ asm("x" #rs1_n) = (uint64_t) rs1;                      \
	asm volatile(".word " STR(CDISCARD_D_L1_REG(rs1_n)) "\n\t" ::[_rs1] "r"(rs1_) \
			     : "memory");                                                     \
}

#define DISCARD_D_REG(rs1) CDISCARD_D_L1_INST(rs1, 13)

void flush_dcache_all(void) {
	FLUSH_D_ALL();
}

void flush_dcache_range(unsigned long start, unsigned long end) {
	start = (start >> 6) * 64;
	end = (end >> 6) * 64;
	for (unsigned long addr = start; addr <= end; addr = addr + 64) {
        FLUSH_D_REG(addr);
	}
} 

#endif  // C_FLUSH_H
