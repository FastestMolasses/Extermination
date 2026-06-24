// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_0020A7A0(long long);
extern int func_001FCE30(int, int, int, int, int);
extern int func_001FCBD0(int, int, int, int, int);
extern int func_00207D00(int, int);
extern int func_00207E40(int, int, int, int, int, int, long long);
extern int func_0020CD40(void);
extern int D_00282240;
extern unsigned short D_00810E74;

int func_00202BA0(void) {
    int r;

    func_0020A7A0(((long long)0x20043525 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2180);
    func_001FCE30(0x70, 0x20, D_00282240, 0x14, 0x80808080);
    func_001FCBD0(0x89, 0xAF, D_00282240, 0x15, 0x80808080);
    func_00207D00(1, 3);
    func_00207E40(1, 0x7000, 0x7B90, 0x100, 0x100, 0x80808080,
                  ((long long)0x20043AE6 << 0x20) | 0x21422000);
    func_00207E40(1, 0x8000, 0x7B90, 0x100, 0x100, 0x80808080,
                  ((long long)0x20043C46 << 0x20) | 0x21422040);
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080,
                  ((long long)0x20043845 << 0x20) | ((long long)0xDD42 << 0x10) | 0x2100);
    if (D_00810E74 & 0x70) {
        func_0020CD40();
        r = (D_00810E74 & 0x10) ? 2 : 1;
        return r;
    }
    return 0;
}
