// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Emits a draw/transform command twice via func_001CFB50 (build packet) +
// func_001CFBE0 (submit). Each pass loads two camera/view floats from the
// gp-relative global D_00275C34 (+0x54, +0x5C), passes the unit scale 1.0f,
// the small epsilon 0x358637BD (~1e-6), and 0.0f into the builder. The two
// submits differ only in mode (2 vs 1) and the register-block source table
// (D_00256DC0 vs D_00256E50), both targeting the shared packet D_0081F8F0.
extern void func_001CFB50(void *a0, int a1, int a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern unsigned char D_00256DC0[16];
extern unsigned char D_00256E50[16];
extern int D_00275C34;
extern unsigned char D_0081F8F0[16];

void func_001ECA20(int arg0, int arg1) {
    func_001CFB50(&D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54), *(float *)(D_00275C34 + 0x5C), 1.0f, 1.0000000116860974e-06f, 0.0f);
    func_001CFBE0(arg1, 2, &D_00256DC0, &D_0081F8F0, 0);
    func_001CFB50(&D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54), *(float *)(D_00275C34 + 0x5C), 1.0f, 1.0000000116860974e-06f, 0.0f);
    func_001CFBE0(arg1, 1, &D_00256E50, &D_0081F8F0, 0);
}
