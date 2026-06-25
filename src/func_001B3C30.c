// COMPILER: mwcc233
// CFLAGS: -O4,p
extern unsigned char D_008102B0[];
extern float D_00810320[];
extern float D_008104D4[];
extern float D_700031B0[];
extern float D_70003600[];

extern void func_00102948(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern void func_001028B8(void *, void *, void *);
extern void func_001028D0(void *, void *, void *);
extern void func_00102760(void *, void *);
extern int func_0021BE40(void *, int);
extern int func_0019AA80(void *, void *, int);
extern int func_0019A570(void *, void *, int, int);
extern void func_001EFD90(int, void *, void *);
extern void func_001E2BA0(void *, void *, int, float);
extern int func_001F4F40(int);
extern void copy_qw4(int, void *);
extern short float_to_int(float);

void func_001B3C30(int arg0, char *arg1, int arg2, float fparg0, float fparg1) {
    float sp70[4];
    float sp80[4];
    struct { float v[3]; int w; } sp90;
    float spA0[4];
    int var_s0;
    char *e;
    int *one;

    var_s0 = 0;
    func_00102948(sp70, arg1 + 0x30);
    *(int *)(arg1 + 0x38) = 0;
    *(int *)(arg1 + 0x34) = 0;
    *(int *)(arg1 + 0x30) = 0;
    sp80[0] = fparg0;
    sp80[1] = 0.0f;
    sp80[2] = 0.0f;
    one = (int *)&sp80[3];
    *one = 0x3F800000;
    func_001026A0(sp80, arg1, sp80);
    func_001028B8(sp80, sp70, sp80);
    func_00102948(arg1 + 0x30, sp70);
    func_001028D0(spA0, sp80, sp70);
    func_00102760(spA0, spA0);
    if (func_0021BE40(D_008102B0, arg0) == 0) {
        if (func_0019AA80(sp70, sp80, 0x20) != 0) {
            var_s0 = 1;
            func_00102948(sp80, D_700031B0);
            e = *(char **)0x700031D0;
            sp90.v[0] = *(float *)(e + 0x24);
            sp90.v[1] = *(float *)(e + 0x28);
            sp90.v[2] = *(float *)(e + 0x2C);
            sp90.w = 0x3F800000;
            *one = 0x3F800000;
        }
    }
    if (func_0019A570(sp70, sp80, 7, 0x20) != 0) {
        func_00102948(sp80, D_700031B0);
        e = *(char **)0x700031D0;
        *(float *)0x70003600 = *(float *)(e + 0x24);
        *(float *)0x70003604 = *(float *)(e + 0x28);
        *(float *)0x70003608 = *(float *)(e + 0x2C);
        *(int *)0x7000360C = 0x3F800000;
        *one = 0x3F800000;
        if (*(int *)0x700031D8 != 1) {
            func_001EFD90(0x80000003, sp80, D_70003600);
            if (*(int *)0x700031D8 == 2) {
                char *t = *(char **)0x700031D4;
                if (t != 0 && *(unsigned char *)(t + 2) == 4) {
                    unsigned char st = *(unsigned char *)(t + 3);
                    if (st != 6 && st != 0x1C && st != 0x1E && st != 0x1F)
                        *(short *)(t + 0x36) = float_to_int(fparg1);
                }
            }
        }
    } else if (var_s0 != 0) {
        func_001EFD90(0x80000006, sp80, sp90.v);
        D_008104D4[0] = fparg1;
        D_008102B0[0] |= 2;
        func_00102948(D_00810320, spA0);
    }
    func_001E2BA0(sp70, sp80, arg2, fparg0);
    e = (char *)func_001F4F40(0);
    if (e != 0) {
        copy_qw4((int)(e + 0xD0), arg1);
        func_00102948(e + 0xB0, arg1 + 0x30);
    }
}
