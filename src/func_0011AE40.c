// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU0: reads one quadword from VU0 data memory at quadword address addr and
// returns it by value (16-byte struct: the caller passes the result buffer as the
// hidden first argument and gets it back). vi1 and vf1 are preserved.
// Note: func_0011B028 declares this as (unsigned short *out, int id), the same
// calling convention seen from the caller's side.

typedef unsigned int u128 __attribute__((mode(TI)));
typedef struct {
    unsigned long d[2];
} vu_qword;
typedef union {
    vu_qword s;
    u128 q;
} vu_qword_u;
vu_qword func_0011AE40(unsigned short addr) {
    vu_qword_u v;
    int vi1;
    u128 vf1;
    u128 q;

    asm("cfc2.ni %0, $vi1" : "=r"(vi1));
    asm("qmfc2.ni %0, $vf1" : "=r"(vf1));
    asm volatile("ctc2.ni %0, $vi1" : : "r"(addr));
    asm volatile("vlqi.xyzw $vf1, ($vi1++)");
    asm volatile("qmfc2.ni %0, $vf1" : "=r"(q));
    asm volatile("ctc2.ni %0, $vi1" : : "r"(vi1));
    asm volatile("qmtc2.ni %0, $vf1" : : "r"(vf1));
    v.q = q;
    return v.s;
}
