// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// State machine on *(int*)(arg0+0x22C): case 1 rebuilds two transform
// matrices (D_700038A0/B0 -> D_700036A0/E0 via build/normalize/concat), draws
// two sprites via func_001CFA60/func_001CFBE0, decays *(p+0x38) by 1/(int)(arg0+0x2C4)
// and on underflow sets *(p+0x3C)=2; case 0 does the single-matrix variant;
// case 2 returns 0; all other states return 1.
extern void func_00102760(void *, void *);
extern void func_001028D0(void *, char *, char *);
extern void func_00102918(void *, void *, char *);
extern int func_001CCF70(void *);
extern void func_001CD390(void *, void *);
extern void func_001CFA60(void *, void *, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);
extern char D_002668A0;
extern float D_002668A8;
extern int D_00266920;
extern char D_700036A0;
extern char D_700036D0;
extern char D_700036E0;
extern char D_70003710;
extern char D_700038A0;
extern char D_700038B0;

int func_0021A180(char *arg0) {
    char sp40[0x60];
    char *p;
    int h;
    float z;

    p = arg0 + 0x1F0;
    switch (*(int *)(arg0 + 0x22C)) {
    case 0:
        func_001028D0(&D_700038A0, p + 0x10, p);
        func_00102760(&D_700038A0, &D_700038A0);
        func_001CD390(&D_700036A0, &D_700038A0);
        func_00102918(&D_700036A0, &D_700036A0, p);
        h = func_001CCF70(&D_700036D0);
        D_00266920 = 0x40;
        D_002668A8 = *(float *)(p + 0x30);
        func_001CFA60(sp40, &D_700036A0, *(float *)(p + 0x38), (z = 0.0f));
        func_001CFBE0(h, 4, &D_002668A0, sp40, 1);
        goto block_8;
    case 1:
        func_001028D0(&D_700038A0, p + 0x20, p);
        func_00102760(&D_700038A0, &D_700038A0);
        func_001CD390(&D_700036A0, &D_700038A0);
        func_00102918(&D_700036A0, &D_700036A0, p);
        func_001028D0(&D_700038B0, p + 0x20, p + 0x10);
        func_00102760(&D_700038B0, &D_700038B0);
        func_001CD390(&D_700036E0, &D_700038B0);
        func_00102918(&D_700036E0, &D_700036E0, p + 0x10);
        h = func_001CCF70(&D_700036D0);
        D_00266920 = 0x40;
        D_002668A8 = *(float *)(p + 0x30);
        func_001CFA60(sp40, &D_700036A0, *(float *)(p + 0x38), (z = 0.0f));
        func_001CFBE0(h, 4, &D_002668A0, sp40, 1);
        h = func_001CCF70(&D_70003710);
        D_00266920 = 0x40;
        D_002668A8 = *(float *)(p + 0x34);
        func_001CFA60(sp40, &D_700036E0, *(float *)(p + 0x38), (z = 0.0f));
        func_001CFBE0(h, 4, &D_002668A0, sp40, 1);
        *(float *)(p + 0x38) = *(float *)(p + 0x38) - 1.0f / (float)*(int *)(arg0 + 0x2C4);
        if (*(float *)(p + 0x38) < 0.0f) {
            *(int *)(p + 0x3C) = 2;
        }
        goto block_8;
    case 2:
        return 0;
    default:
        ;
    }
block_8:
    return 1;
}
