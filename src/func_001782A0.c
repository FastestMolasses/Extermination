// CFLAGS: -O4,p -sdatathreshold 0
extern int func_00199FA0(int a0, int a1);
extern int float_to_int(float);
extern void func_00199DB0(void *p);
extern int D_700038A0;

int func_001782A0(char *a0) {
    float buf0[4];
    float buf1[4];
    float f20;
    float t;
    float sixtenths = 0.6f;
    float s;

    if (func_00199FA0((int)buf0, (int)buf1)) {
        f20 = buf0[1];
        t = (3.0f + *(float *)(a0 + 0xB4) - f20) / 3.0f;
        *(float *)0x70003A20 = t;
        if (t >= 0.0f) {
            s = *(float *)0x70003A20 + sixtenths;
            *(float *)0x70003A20 = s;
            *(float *)(a0 + 0x2E4) = f20 + 3.0f * (float)float_to_int(s);
            func_00199DB0(&D_700038A0);
            *(float *)(a0 + 0x2E0) = *(float *)0x700038A0;
            *(float *)(a0 + 0x2E8) = *(float *)0x700038A8;
            *(float *)(a0 + 0x218) = *(float *)(a0 + 0xC4);
            return 1;
        }
    }
    return 0;
}
