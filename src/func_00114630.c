// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.93% NEARMISS).
// Fixes read from the original instructions:
//  - the request block D_0027B140 is a struct written through field stores, not
//    an int array indexed by constants; the struct spelling gives ee-gcc's
//    scheduler the original order (the %lo add of the reply buffer ahead of the
//    first store, and the second cache-flush argument set before the first);
//  - callee prototypes taken from their definitions/twins: func_0010DFD8 takes
//    (void *addr, int size); the completion callback func_001145A0 takes one int.
//
// SDK RPC wrapper (request 5): unless a request is already pending
// (D_00241D68) or the client is unbound (-100), fill the request block with
// a0, the caller's buffer (a1) and its size (a2) plus the reply buffer
// D_0027B5C0, flush both buffers from the data cache, and issue the call with
// completion callback func_001145A0; on success mark request 5 pending.

extern int D_00241D68;
extern int D_0027B0C0[];
/* Request block sent to the IOP (0x30 bytes; only the fields used here are named). */
struct cfg {
    int f00, f04, f08, f0C, f10, f14, f18;
    void *f1C;
};
extern struct cfg D_0027B140;
extern int D_0027B5C0[];
extern int D_0027C680[];
extern void func_001145A0(int a0);
extern void func_0010DFD8(void *a0, int a1);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, void *a7, void *a8);

int func_00114630(int a0, int a1, int a2) {
    int r;

    if (D_00241D68 != 0) {
        return D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }
    D_0027B140.f00 = a0;
    D_0027B140.f18 = a1;
    D_0027B140.f0C = a2;
    D_0027B140.f1C = D_0027B5C0;
    func_0010DFD8((void *)a1, a2);
    func_0010DFD8(D_0027B5C0, 0xC0);
    r = func_0010E8A8(D_0027B0C0, 5, 1, &D_0027B140, 0x30, D_0027C680, 4,
                      func_001145A0, D_0027B5C0);
    if (r == 0) {
        D_00241D68 = 5;
    }
    return r;
}
