// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Per-frame decay of two float counters on an entity record, propagated into
// four sub-objects hung off the global D_00275B40 (a struct whose +4/+8/+0xC/+0x10
// slots are object pointers, each carrying a float at +0x80). Field +0x14 decays
// by 0.2/frame; if it is still <= 8.0 the +0x10 counter also decays by 0.2; both
// clamp at 0.0. The +0x80 floats of the first two objects get (+0x10 + +0x14),
// the last two get just +0x14. Returns 1 iff the +0x10 counter has hit 0.0.
extern char *D_00275B40;

int func_001BD270(char *arg0) {
    float a, b;
    int r;

    a = *(float *)(arg0 + 0x14) - 0.2f;
    *(float *)(arg0 + 0x14) = a;
    if (a <= 8.0f) {
        if (a <= 0.0f) {
            *(float *)(arg0 + 0x14) = 0.0f;
        }
        b = *(float *)(arg0 + 0x10) - 0.2f;
        *(float *)(arg0 + 0x10) = b;
        if (b <= 0.0f) {
            *(float *)(arg0 + 0x10) = 0.0f;
        }
    }
    *(float *)(*(int *)(D_00275B40 + 4) + 0x80) = *(float *)(arg0 + 0x10) + *(float *)(arg0 + 0x14);
    *(float *)(*(int *)(D_00275B40 + 8) + 0x80) = *(float *)(arg0 + 0x10) + *(float *)(arg0 + 0x14);
    *(float *)(*(int *)(D_00275B40 + 0xC) + 0x80) = *(float *)(arg0 + 0x14);
    *(float *)(*(int *)(D_00275B40 + 0x10) + 0x80) = *(float *)(arg0 + 0x14);
    if (*(float *)(arg0 + 0x10) == 0.0f) {
        r = 1;
    } else {
        r = 0;
    }
    return r;
}
