// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Keep f12 as the caller's blend and pass explicit source frame0 in f13.
// The old no-float prototype omitted this real initialization argument.
extern void anim_clip_init(int, int, float, float);

int func_001749A0(int a0, short a1, int a2, float blend) {
    if (a2 == 0) {
        short v0 = *(short *)(a0 + 0x20C);
        if ((short)a1 == v0) {
            return 1;
        }
    }
    *(short *)(a0 + 0x20C) = a1;
    anim_clip_init(a0, *(short *)(a0 + 0x20C), blend, 0.0f);
    return 0;
}
