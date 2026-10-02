// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0
// Probe: probe+0xC = 1.0, func_0019BC40(probe), then
// r = func_0019AD00(world, probe, 7). r == 0 gives func_001B30E0(probe, arg). Else
// the result is 4 when r has bit 1 and the scratchpad word 0x700031D4 is 0, or
// when the halfword +0x1A of the record pointed to by 0x700031D0 has any of bits
// 0x1800. Otherwise func_0019A310(&d), then func_001B2E50(probe, arg) | 4 when
// that halfword has bit 0x2000 or d is not at most limit; d at most limit gives
// func_001B30E0(probe, arg).
extern int D_700031D4;
extern char *D_700031D0;
extern void func_0019BC40(void *probe);
extern int func_0019AD00(char *world, char *probe, int mask);
extern void func_0019A310(float *depth);
extern int func_001B2E50(char *probe, int arg);
extern int func_001B30E0(char *probe, int arg);

int func_001B2BF0(char *world, char *probe, int arg, float limit) {
    float depth;
    int hit;
    short flags;
    int r;

    *(float *)(probe + 0xC) = 1.0f;
    func_0019BC40(probe);
    hit = func_0019AD00(world, probe, 7);
    if (hit) {
        if ((hit & 2) && D_700031D4 == 0) {
            return 4;
        }
        flags = *(short *)(D_700031D0 + 0x1A);
        if (flags & 0x1800) {
            return 4;
        }
        func_0019A310(&depth);
        r = func_001B2E50(probe, arg);
        if (flags & 0x2000) {
            return r | 4;
        }
        if (!(depth <= limit)) {
            return r | 4;
        }
    }
    return func_001B30E0(probe, arg);
}
