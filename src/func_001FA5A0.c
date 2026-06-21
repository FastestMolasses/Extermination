// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Ring-buffer push: append a0 into the next free slot of D_00281CF0[] (16-entry
// ring, head index in the gp-rel byte D_00275B30), only if that slot is empty
// (== -1); advance head (& 0xF) on insert. Always returns 1.
// 2.3.3-only match: the target keeps a DEAD recomputation of (head+1) that the
// 991202 build eliminates (early-return-then-store shape reproduces it on 2.3.3).
extern signed char D_00275B30;
extern int D_00281CF0[0x40];

int func_001FA5A0(int a0) {
    int idx = D_00275B30;
    int *p = &D_00281CF0[idx];
    if (*p != -1) {
        return 1;
    }
    *p = a0;
    D_00275B30 = (idx + 1) & 0xF;
    return 1;
}
