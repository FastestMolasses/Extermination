// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Object init/copy helper: stores four float params into dst at +0x44/+0x4C/
// +0x48/+0x50, then calls func_001D0540(src+0x30, &D_70003AC0, f16) and stows
// the returned float at dst+0x54; calls func_001CD370(a1) and stows its int
// result at dst+0x40; finally copies four 128-bit quadwords (0x00..0x3F) from
// src to dst via lq/sq. The 5th float param (f16) is forwarded as the float
// arg to func_001D0540 (the target's `mov.s f12,f16`).
typedef unsigned __int128 u128;

extern float D_70003AC0;
extern float func_001D0540(unsigned char *a, float *b, float c);
extern int func_001CD370(unsigned char *a);

void func_001CFB50(unsigned char *dst, unsigned char *a1, unsigned char *src,
                   float f12, float f13, float f14, float f15, float f16) {
    *(float *)(dst + 0x44) = f12;
    *(float *)(dst + 0x4C) = f13;
    *(float *)(dst + 0x48) = f14;
    *(float *)(dst + 0x50) = f15;
    *(float *)(dst + 0x54) = func_001D0540(src + 0x30, &D_70003AC0, f16);
    *(int *)(dst + 0x40) = func_001CD370(a1);
    *(u128 *)(dst + 0x00) = *(u128 *)(src + 0x00);
    *(u128 *)(dst + 0x10) = *(u128 *)(src + 0x10);
    *(u128 *)(dst + 0x20) = *(u128 *)(src + 0x20);
    *(u128 *)(dst + 0x30) = *(u128 *)(src + 0x30);
}
