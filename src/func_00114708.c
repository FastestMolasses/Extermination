// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 63.02% NEARMISS).
// Fixes read from the original instructions:
//  - the call passes NINE arguments like its twin func_00114630: the client
//    D_0027B0C0, request 6, mode 1, the request block, size 0x30, the receive
//    buffer D_0027C680, its size 4, and no callback / callback data (0, 0); the
//    old C passed only seven, omitting D_0027C680 and the callback data;
//  - the request block D_0027B140 is a struct written through field stores
//    (as in func_00114630), with the leading-bytes copy into its 0x20 field;
//  - the leading-byte count is spelled as the distance from the buffer up to
//    the next 16-byte boundary, (((a1 - 1) & ~15) + 16) - a1, in unsigned
//    arithmetic (the 0xfffffff0 mask is an unsigned constant);
//  - store order: short branch writes count, size, then pointer; long branch
//    writes the remaining size, the count, then the aligned pointer.
//
// SDK RPC wrapper (request 6): unless a request is already pending
// (D_00241D68) or the client is unbound (-100), store a0 in the request block;
// buffers of up to 16 bytes are sent whole inside the block (count = a2, no
// separate transfer); longer buffers send their unaligned leading bytes inside
// the block and the 16-byte-aligned rest (pointer + size) as a separate
// transfer. Copy the leading bytes, flush the data cache, issue the call, and
// on success mark request 6 pending.

extern int FlushCache(int ch);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, void *a7, void *a8);

extern int D_00241D68;
extern int D_0027B0C0[];
/* Request block sent to the IOP (0x30 bytes; only the fields used here are named). */
struct cfg {
    int f00, f04, f08, f0C, f10, f14;
    unsigned char *f18;
    void *f1C;
    unsigned char f20[0x10];
};
extern struct cfg D_0027B140;
extern int D_0027C680[];

int func_00114708(int a0, unsigned char *a1, int a2)
{
    int r;
    unsigned int i;

    if (D_00241D68 != 0) {
        return D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }
    D_0027B140.f00 = a0;
    if (a2 < 0x11) {
        D_0027B140.f14 = a2;
        D_0027B140.f0C = 0;
        D_0027B140.f18 = 0;
    } else {
        unsigned int len = (((unsigned int)a1 - 1) & 0xfffffff0) + 0x10 - (unsigned int)a1;
        D_0027B140.f0C = a2 - len;
        D_0027B140.f14 = len;
        D_0027B140.f18 = a1 + len;
    }
    for (i = 0; i < D_0027B140.f14; i++) {
        D_0027B140.f20[i] = a1[i];
    }
    FlushCache(0);
    r = func_0010E8A8(D_0027B0C0, 6, 1, &D_0027B140, 0x30, D_0027C680, 4, 0, 0);
    if (r == 0) {
        D_00241D68 = 6;
    }
    return r;
}
