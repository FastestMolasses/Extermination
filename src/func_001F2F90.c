// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Spawn/emit a particle or projectile instance (arg1) from an emitter template
// (arg2) anchored to a parent transform (arg0); arg3 selects the emission mode.
// Builds randomized position/orientation/scale, fills the instance header.
extern int func_001026A0(void *, void *, void *);
extern int func_001026D0(void *, void *, void *);
extern int func_001028D0(void *, void *, void *);
extern int func_00102948(void *, void *);
extern int func_001029C0(void *);
extern int func_00102BB0(void *, void *, float);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern int func_00122BB8(void);
extern int func_001C9E40(void *, void *);
extern int func_001CA3B0(void *, float, float, float);
extern char D_70003400;

void func_001F2F90(char *arg0, char *arg1, char *arg2, int arg3) {
    float ang;
    float spin;
    float t;
    char *p;

    ang = (6.2831855f * ((float)func_00122BB8() / 2147483648.0f)) - 3.1415927f;
    spin = 0.5f * (3.1415927f * ((float)func_00122BB8() / 2147483648.0f));
    switch (arg3) {
    case 3:
    case 7:
    case 8:
    case 9:
        p = arg0 + 0x30;
        func_00102948(arg1, p);
        *(int *)(arg1 + 0xC) = 0x3F800000;
        func_001026A0(arg1 + 0x10, arg0, arg2 + 0x10);
        func_001028D0(arg1 + 0x10, arg1 + 0x10, arg0 + 0x30);
        *(float *)(arg1 + 0x10) = *(float *)(arg1 + 0x10) + (0.04f * func_0011E2A8(ang));
        *(float *)(arg1 + 0x18) = *(float *)(arg1 + 0x18) + (0.04f * func_0011DE90(ang));
        func_001029C0(&D_70003400);
        func_00102BB0(&D_70003400, &D_70003400, 1.5707964f);
        func_001026D0(&D_70003400, arg0, &D_70003400);
        func_001C9E40(arg1 + 0x20, &D_70003400);
        break;
    default:
        *(float *)(arg1 + 0) = *(float *)(arg0 + 0x30) + (*(float *)(arg2 + 0) * func_0011DE90(ang));
        *(float *)(arg1 + 4) = *(float *)(arg0 + 0x34) + (*(float *)(arg2 + 4) * func_0011DE90(spin));
        *(float *)(arg1 + 8) = *(float *)(arg0 + 0x38) + (*(float *)(arg2 + 8) * func_0011E2A8(ang));
        *(int *)(arg1 + 0xC) = 0x3F800000;
        *(float *)(arg1 + 0x10) = (*(float *)(arg2 + 0x10) * ((float)func_00122BB8() / 2147483648.0f)) - *(float *)(arg2 + 0x20);
        *(float *)(arg1 + 0x14) = (*(float *)(arg2 + 0x14) * ((float)func_00122BB8() / 2147483648.0f)) - *(float *)(arg2 + 0x24);
        *(float *)(arg1 + 0x18) = (*(float *)(arg2 + 0x18) * ((float)func_00122BB8() / 2147483648.0f)) - *(float *)(arg2 + 0x28);
        *(int *)(arg1 + 0x1C) = 0x3F800000;
        func_001026A0(arg1 + 0x10, arg0, arg1 + 0x10);
        func_001028D0(arg1 + 0x10, arg1 + 0x10, arg0 + 0x30);
        t = (3.1415927f * (360.0f * ((float)func_00122BB8() / 2147483648.0f))) / 180.0f;
        func_001CA3B0(arg1 + 0x20, t, t, t);
        break;
    }
    func_00102948(arg1 + 0x40, arg2 + 0x30);
    t = *(float *)(arg2 + 0x40) + (*(float *)(arg2 + 0x44) * ((float)func_00122BB8() / 2147483648.0f));
    *(float *)(arg1 + 0x30) = t;
    *(float *)(arg1 + 0x34) = t;
    *(float *)(arg1 + 0x38) = t;
    *(int *)(arg1 + 0x3C) = 0x3F800000;
    *(int *)(arg1 + 0x78) = 0;
    *(int *)(arg1 + 0x70) = 0;
    *(short *)(arg1 + 0x80) = 0;
    *(int *)(arg1 + 0x7C) = 0;
    *(int *)(arg1 + 0x74) = 0x2710;
    *(short *)(arg1 + 0x82) = arg3;
    *(int *)(arg1 + 0x84) = 0x3F800000;
}
