// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0x20
// Dispatch helper: when either IOP HW-register bit (0x70003B7C / 0x70003B7E,
// masked by global event flags D_00810E70) is set, latch state 0x13 into the
// actor and snapshot a set of floats (pos/scale shadow copies) into gp-rel
// globals + the actor's 0xB0/0xB8 slots. Always returns 0 (the 0x13/1 stores
// are state side-effects; the function's return is unconditionally zeroed in a
// branch delay slot). 0x70003B7C/3B7E are absolute HW-register reads.
extern unsigned short D_00810E70[32];
extern float D_00275B00[4];
extern float D_00275B10;
extern int D_00275B14;
extern float D_00281B64[16];

int func_001696A0(char *arg0) {
    int flags = D_00810E70[0];

    if (flags & *(unsigned short *)0x70003B7C) {
        goto store;
    }
    if (flags & *(unsigned short *)0x70003B7E) {
store:
        *(char *)(arg0 + 5) = 0x13;
        *(char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 0x1F0) = 0x25;
        D_00275B14 = 0x34;
        D_00275B10 = *(float *)(arg0 + 0x2E0);
        D_00275B00[3] = *(float *)(arg0 + 0x2E8);
        D_00281B64[0] = *(float *)(arg0 + 0xC4);
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x290);
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x298);
        return 1;
    }
    return 0;
}
