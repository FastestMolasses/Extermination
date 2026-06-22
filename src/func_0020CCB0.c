// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int float_to_int(float);
extern void func_00207F80(int, int, int, int, int, int);

void func_0020CCB0(unsigned char *p) {
    int n = (p[6] == 0) ? 0xFD : 0x14F;
    int r1 = float_to_int(16.0f * (float)(n + 0x700));
    int r2 = float_to_int(16.0f * (float)(n + 0x70C));
    func_00207F80(1, r1, 0x85E0, r2, 0x8640, 0x80CE6000);
}
