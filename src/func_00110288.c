// COMPILER: eegcc
// CFLAGS: -O2
struct cfg {
    int f0;     // 0x0
    int f4;     // 0x4
    int f8;     // 0x8
};
extern struct cfg D_00279440;
extern int D_00279640[];
extern int func_0010FAD0(int a0, void *a1, int a2);
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_00110288(int a0, unsigned char *a1, int a2) {
    int r;
    if (func_0010FAD0(a0, a1, a2) < 0) {
        return 0xFFFF0000;
    }
    D_00279440.f0 = a0;
    D_00279440.f4 = a2;
    if (a2 == 0) {
        *(unsigned char *)&D_00279440.f8 = a1[0];
    } else if (a2 == 1) {
        *(unsigned short *)&D_00279440.f8 = *(unsigned short *)a1;
    } else if (a2 == 2) {
        D_00279440.f8 = *(int *)a1;
    } else {
        return 0xFFFEFFFE;
    }
    r = func_0010E8A8(D_00279640, 2, 0, &D_00279440, 0x20, &D_00279440, 0x10, 0, 0);
    if (r < 0) {
        return 0xFFFEFFFF;
    }
    return 0;
}
