// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001029C0(void *);
extern int func_00102BB0(void *, void *, float);
extern int func_00102918(void *, void *, void *);
extern int func_001026A0(void *, void *, void *);
extern int func_0019AD00(unsigned char *, void *, int);
extern int func_001762E0(unsigned char *);
extern float func_001B1470(float);
extern float D_0024895C;
extern int D_700036A0;
extern int D_700038A0;
extern int D_700038B0;

int func_00176DC0(unsigned char *arg0) {
    int i = 3;
    float *p = &D_0024895C;
    *(char *)(arg0 + 0x314) = 0;
    do {
        func_001029C0(&D_700036A0);
        func_00102BB0(&D_700036A0, &D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + *p));
        func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
        *(int *)0x700038A0 = 0;
        *(float *)0x700038A4 = 4.01f;
        *(float *)0x700038A8 = 5.5f;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
        if (func_0019AD00(arg0, &D_700038B0, 6) & 2) {
            if (func_001762E0(arg0)) {
                goto done;
            }
        }
        *(float *)0x700038A4 = 10.0f;
        func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
        if (func_0019AD00(arg0, &D_700038B0, 6) & 2) {
            if (func_001762E0(arg0)) {
                goto done;
            }
        }
        *(float *)0x700038A4 = 18.0f;
        func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
        if (func_0019AD00(arg0, &D_700038B0, 6) & 2) {
            if (func_001762E0(arg0)) {
                goto done;
            }
        }
        i += 1;
        p += 1;
    } while (i < 8);
done:
    return 0;
}
