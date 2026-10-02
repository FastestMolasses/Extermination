// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Scaled copy of a 0x90-byte record: block_copy(dst, src, 0x90), then
// func_00103230(v, v, s) on dst+0 and dst+0x10, func_00102900(v, v, s) on
// dst+0x40 and dst+0x50, +0x78, +0x7C and +0x84 multiplied by s and the integer
// +0x80 replaced by float_to_int(+0x80 * s).
extern void block_copy(void *dst, void *src, int len);
extern void func_00103230(void *a, void *b, float s);
extern void func_00102900(void *dst, void *src, float s);
extern int float_to_int(float f);

void func_001D0400(char *dst, char *src, float s) {
    block_copy(dst, src, 0x90);
    func_00103230(dst, dst, s);
    func_00103230(dst + 0x10, dst + 0x10, s);
    func_00102900(dst + 0x40, dst + 0x40, s);
    func_00102900(dst + 0x50, dst + 0x50, s);
    *(float *)(dst + 0x78) *= s;
    *(float *)(dst + 0x7C) *= s;
    *(float *)(dst + 0x84) *= s;
    *(int *)(dst + 0x80) = float_to_int((float)*(int *)(dst + 0x80) * s);
}
