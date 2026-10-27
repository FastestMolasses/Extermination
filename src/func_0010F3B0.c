// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 64.12% NEARMISS).
// Fixes read from the original instructions:
//  - Shape taken from the byte-matched twin func_0010F2F8: the request block
//    pointer is a local taken before the lookup call, and the block is filled
//    in field order ([0], [+4], [+8]).
//  - func_0010EFA8 takes the caller's first argument (its definition in src/
//    has one parameter); a0 passes straight through.
//  - The mode argument (bit 7 of byte 5 of the entry) is shifted as a 64-bit
//    value: it is held in an unsigned long and the callee's third parameter is
//    declared 64-bit in this file, which gives the original's 64-bit shift.
//    The ee-gcc "forward-branch-likely wall" verdict was wrong.
//
// Wrapper over the SIF RPC call: looks up the entry for a0; returns -1 if
// D_00241C88 is 0, -9 if there is no entry or its word [1] is 0;
// fills the 12-byte request at D_00278C00 (entry id, a1, a2), calls
// func_0010E8A8 with function number 4 and bit 7 of entry byte 5 as mode,
// receiving 4 bytes into D_00279040. Returns -1 on RPC failure, 0 when the
// entry's 0x8000 flag is set, otherwise the received word.

extern int func_0010EFA8(int a0);
extern int func_0010E8A8(void *a0, int a1, unsigned long a2, void *a3, int t0, void *t1, int t2, int t3, int s9);
extern int D_00241C88;
extern int D_00278C00;
extern int D_00279040;
extern int D_00279210;

int func_0010F3B0(int a0, int a1, int a2) {
    int *p;
    int *q;
    unsigned long m;

    q = &D_00278C00;
    p = (int *)func_0010EFA8(a0);
    if (D_00241C88 == 0) {
        return -1;
    }
    if (p == 0) {
        return -9;
    }
    if (p[1] == 0) {
        return -9;
    }
    *q = p[0];
    q[1] = a1;
    q[2] = a2;
    m = ((unsigned char *)p)[5];
    if (func_0010E8A8(&D_00279210, 4, m >> 7, q, 0xC, &D_00279040, 4, 0, 0) < 0) {
        return -1;
    }
    if (p[1] & 0x8000) {
        return 0;
    }
    return D_00279040;
}
