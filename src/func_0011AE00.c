// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU0: writes one quadword (*src) to VU0 data memory at quadword address
// addr; addresses 0x400 and up reach the VU1 registers. vi1 and vf1 are used as
// the transfer registers and restored afterwards.

typedef unsigned int u128 __attribute__((mode(TI)));
typedef struct {
    unsigned long d[2];
} vu_qword;
typedef union {
    vu_qword s;
    u128 q;
} vu_qword_u;
void func_0011AE00(unsigned short addr, vu_qword *src) {
    vu_qword_u v;
    int vi1;
    u128 vf1;

    v.s = *src;
    asm("cfc2.ni %0, $vi1" : "=r"(vi1));
    asm("qmfc2.ni %0, $vf1" : "=r"(vf1));
    asm volatile("qmtc2.ni %0, $vf1" : : "r"(v.q));
    asm volatile("ctc2.ni %0, $vi1" : : "r"(addr));
    asm volatile("vsqi.xyzw $vf1, ($vi1++)");
    asm volatile("ctc2.ni %0, $vi1" : : "r"(vi1));
    asm volatile("qmtc2.ni %0, $vf1" : : "r"(vf1));
}
