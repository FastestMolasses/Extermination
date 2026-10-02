// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Proximity test between a and b: when the horizontal distance
// sqrt(dx^2 + dz^2) (func_0011E748) of b+0xB0/+0xB8 from a's is at most 7.0 plus
// the float at *(a+0x30), and |6.0 + b+0xB4 - a+0xB4| (func_0011DF78) is at
// most 8.0, it sets b+0x36 = 20 and the halfword 0x70003B88 = 0.
extern float func_0011E748(float x);
extern float func_0011DF78(float x);

void func_001A8E80(char *a, char *b) {
    float dx = *(float *)(b + 0xB0) - *(float *)(a + 0xB0);
    float dz = *(float *)(b + 0xB8) - *(float *)(a + 0xB8);
    float dy;
    if (func_0011E748(dx * dx + dz * dz) <= 7.0f + **(float **)(a + 0x30)) {
        dy = 6.0f + *(float *)(b + 0xB4);
        dy -= *(float *)(a + 0xB4);
        if (func_0011DF78(dy) <= 8.0f) {
        *(short *)(b + 0x36) = 0x14;
        *(volatile short *)0x70003B88 = 0;
        }
    }
}
