// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001026A0(void *, void *, void *);
extern void func_001028B8(void *, void *, char *);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102C58(void *, void *, void *);
extern int func_00122BB8(int);
extern int func_0019AD00(char *, void *, int);
extern float func_001B1470(int, float);
extern char D_700036A0[];
extern char D_700038A0[];

int func_0013C1F0(char *arg0, char *arg1) {
    char *p;
    float var_f20;
    int a0;
    int v0;
    int v1;
    int fl;
    unsigned short t;

    p = arg0;
    t = *(unsigned short *)(arg1 + 0x28);
    if (t != 0) {
        *(unsigned short *)(arg1 + 0x28) = t - 1;
    }
    v1 = *(unsigned short *)(arg1 + 0x2A);
    *(unsigned short *)(arg1 + 0x2A) = v1 + 1;
    if (v1 >= 0xB4 || (*(char *)(arg1 + 0x81) & 6)) {
        a0 = *(char *)(arg1 + 0x80);
        if (!(a0 & 3)) {
            fl = *(char *)(arg1 + 0x81);
            if (fl & 6) {
                if (fl & 4) {
                    *(char *)(arg1 + 0x80) = a0 | 2;
                } else {
                    *(char *)(arg1 + 0x80) = a0 | 1;
                }
            } else {
                a0 = (func_00122BB8(a0) >> 0xE) & 1;
                v1 = (char)(1 << a0);
                *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) | v1;
            }
        }
    }
    if (*(char *)(arg1 + 0x80) & 8) {
        *(float *)(p + 0xC4) = func_001B1470(a0, 0.047123894f + *(float *)(p + 0xC4));
        var_f20 = -0.39269909f;
    } else {
        *(float *)(p + 0xC4) = func_001B1470(a0, *(float *)(p + 0xC4) - 0.047123894f);
        var_f20 = 0.39269909f;
    }
    func_001029C0(D_700036A0);
    *(int *)0x700038A0 = 0;
    *(float *)0x700038A4 = *(float *)(p + 0xC4);
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    func_00102C58(D_700036A0, D_700036A0, D_700038A0);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(int *)0x700038A8 = 0x41C80000;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    func_001028B8(D_700038A0, D_700038A0, p + 0xB0);
    if (func_0019AD00(p, D_700038A0, 6) != 0) {
        return 1;
    }
    func_00102BB0(D_700036A0, D_700036A0, var_f20);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(int *)0x700038A8 = 0x41C80000;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    func_001028B8(D_700038A0, D_700038A0, p + 0xB0);
    if (func_0019AD00(p, D_700038A0, 6) != 0) {
        return 1;
    }
    if (*(unsigned short *)(arg1 + 0x28) != 0) {
        return 1;
    }
    *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) & 3;
    *(float *)(arg1 + 0x58) = *(float *)(p + 0xC4);
    return 0;
}
