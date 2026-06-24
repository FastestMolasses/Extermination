// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Vector clamp + flag helper. Fills a 4-float stack buffer via func_001026A0,
// then scales components 0..2 by 16.0/buf[3], setting flag bit 8 (in flags,
// seeded from arg3>>1) whenever a scaled value is negative or out of range
// (> 65520.0). Calls func_001E00F0(arg0, buf, arg2); if any flag was set,
// stores 0x8000 to *(int*)(arg0+0x2C). Returns the flags word.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): under the pinned 991202 the
// residual is wall #13 (it fills the bc1tl float-compare delay slots that CW
// leaves as nop), stalling at 83.29%. 2.3.3 leaves those nops -> objdiff 100%.
extern void func_001026A0(float *, void *, int);
extern void func_001E00F0(char *, float *, int);
extern char D_70003AC0;

int func_001E0240(char *arg0, int arg1, int arg2, int arg3) {
    float buf[4];
    float scale;
    int flags;

    flags = arg3 >> 1;
    func_001026A0(buf, &D_70003AC0, arg1);
    scale = 16.0f / buf[3];
    if (scale < 0.0f) {
        flags |= 8;
    }
    buf[0] *= scale;
    buf[1] *= scale;
    buf[2] *= scale;
    if (buf[0] < 0.0f || !(buf[0] <= 65520.0f)) {
        flags |= 8;
    } else if (buf[1] < 0.0f || !(buf[1] <= 65520.0f)) {
        flags |= 8;
    }
    func_001E00F0(arg0, buf, arg2);
    if (flags != 0) {
        *(int *)(arg0 + 0x2C) = 0x8000;
    }
    return flags;
}
