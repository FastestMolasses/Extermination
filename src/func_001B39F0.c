// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Run two opposing probe passes (forward +200 and backward -200 along the
// object's facing quaternion) through func_0019A6F0; on a hit, project the
// resulting scratchpad vector and store its magnitude (sqrt of sum of the three
// squared components, via func_0011E748) into arg2[0]/arg2[1]. Decide a result
// from the two distances: when they are within 1.5, return a 1-bit flag derived
// from func_00122BB8; otherwise return whether the backward distance is the
// larger of the two.
extern void func_001B2B10(unsigned char *a, float *b, float *c);
extern void func_001028B8(float *a, float *b, int c);
extern void func_001028D0(float *a, float *b, int c);
extern int func_0019A6F0(unsigned char *a, int b, float *c, int d, int e);
extern float func_0011E748(float v);
extern float func_0011DF78(float v);
extern int func_00122BB8(void);

extern float D_70003600;
extern float D_70003604;
extern float D_70003608;
extern float D_7000360C;
extern float D_700031B0;

int func_001B39F0(unsigned char *arg0, int arg1, float *arg2) {
    arg2[1] = 500.0f;
    arg2[0] = 500.0f;
    *(float *)0x70003600 = 200.0f;
    *(float *)0x70003604 = 0.0f;
    *(int *)0x70003608 = 0;
    *(int *)0x7000360C = 0x3F800000;
    func_001B2B10(arg0, &D_70003600, &D_70003600);
    func_001028B8(&D_70003600, &D_70003600, arg1);
    if (func_0019A6F0(arg0, arg1, &D_70003600, 7, 0x40) != 0) {
        func_001028D0(&D_70003600, &D_700031B0, arg1);
        arg2[0] = func_0011E748(*(float *)0x70003600 * *(float *)0x70003600 +
                                *(float *)0x70003604 * *(float *)0x70003604 +
                                *(float *)0x70003608 * *(float *)0x70003608);
    }
    *(float *)0x70003600 = -200.0f;
    *(float *)0x70003604 = 0.0f;
    *(int *)0x70003608 = 0;
    *(int *)0x7000360C = 0x3F800000;
    func_001B2B10(arg0, &D_70003600, &D_70003600);
    func_001028B8(&D_70003600, &D_70003600, arg1);
    if (func_0019A6F0(arg0, arg1, &D_70003600, 7, 0x40) != 0) {
        func_001028D0(&D_70003600, &D_700031B0, arg1);
        arg2[1] = func_0011E748(*(float *)0x70003600 * *(float *)0x70003600 +
                                *(float *)0x70003604 * *(float *)0x70003604 +
                                *(float *)0x70003608 * *(float *)0x70003608);
    }
    if (func_0011DF78(arg2[0] - arg2[1]) <= 1.5f) {
        return (signed char)((func_00122BB8() >> 0xB) & 1);
    }
    if (arg2[0] < arg2[1]) {
        return 0;
    }
    return 1;
}
