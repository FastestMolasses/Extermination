// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player rumble while +0x220 (a non-zero intensity distance) is set: within
// 10.0 the pad vibrates with strength 0xE0 and the cycle restarts every 0x3D
// frames; within 35.0 with strength 0xD0 every 0x79 frames; farther away the
// frame counter +0x210 is cleared. func_001B61C0(port 0, strength, 4, 0)
// starts the vibration when the counter is 0.
extern void func_001B61C0(int port, int strength, int mode, int arg3);

void func_0015D000(unsigned char *p) {
    float d = *(float *)(p + 0x220);

    if (d) {
        if (d <= 10.0f) {
            if (*(short *)(p + 0x210) == 0) {
                func_001B61C0(0, 0xE0, 4, 0);
            }
            (*(short *)(p + 0x210))++;
            if (*(short *)(p + 0x210) > 0x3C) {
                *(short *)(p + 0x210) = 0;
            }
        } else if (d <= 35.0f) {
            if (*(short *)(p + 0x210) == 0) {
                func_001B61C0(0, 0xD0, 4, 0);
            }
            (*(short *)(p + 0x210))++;
            if (*(short *)(p + 0x210) > 0x78) {
                *(short *)(p + 0x210) = 0;
            }
        } else {
            *(short *)(p + 0x210) = 0;
        }
    }
}
