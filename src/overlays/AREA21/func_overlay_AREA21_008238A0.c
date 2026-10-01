// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008238E0 (splat/link name 008238A0; overlay code
//  is linked 0x40 below where it runs), 0x12C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: group 0x82BA70 member (x3). State 0: +0x28 = 0, +0x2A = +1 or -1
//  by func_00122BB8() % 2. State 1: a func_001CD520 quad (bucket 0, mode 1,
//  size 20 x 15, depth 5, colour 0x40808080) at +0xB0 with x + 0.03 * +0x28,
//  then +0x28 += +0x2A.
#define S16(o) (*(short *)(self + (o)))
extern float D_700038A0[4];
extern int func_00122BB8(void);
extern void func_00102948(void *dst, void *src);
extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         float w, float h, float zbias, unsigned int rgba);

void func_overlay_AREA21_008238A0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        S16(0x28) = 0;
        S16(0x2A) = (func_00122BB8() % 2) ? 1 : -1;
        break;
    case 1:
        func_00102948(D_700038A0, self + 0xB0);
        D_700038A0[0] += 0.03f * (float)S16(0x28);
        func_001CD520(0, 1, D_700038A0, 0x2004128555322090ULL, 20.0f, 15.0f, 5.0f, 0x40808080);
        S16(0x28) += S16(0x2A);
        break;
    case 2:
    case 3:
        break;
    }
}
