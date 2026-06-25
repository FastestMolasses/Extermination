// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011DE90(float);
extern float func_0011DF78(float);
extern float func_0011E2A8(float);
extern int func_00182F90(void *, void *);
extern float func_001B1240(char *, float, float);
extern float func_001B1470(float);
extern int func_001BA1A0(int, void *);
extern int func_001BA1F0(char *);
extern int func_001BBD60(char *, void *);

extern char D_0024D900[];
extern char D_0024D980[];
extern char D_0024DA40[];
extern int D_0024E140[];
extern char D_008102B0[];
extern int D_008106C8;
extern unsigned char D_00810700;
extern char D_700038A0[];

int func_001BB560(char *arg0, int arg1, int arg2) {
    char *p = D_008102B0;
    int v;
    int idx;

    if (*(unsigned char *)(arg0 + 0xB) & 4) {
        if (func_0011DF78(func_001B1470(func_001B1240(arg0 + 0xB0, *(float *)(p + 0xA0), *(float *)(p + 0xA8)) - *(float *)(arg0 + 0xC4))) <= 1.5707964f) {
            *(float *)(p + 0xC4) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
            *(short *)(arg0 + 0x2E) = 0;
        } else {
            *(float *)(p + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4));
            *(short *)(arg0 + 0x2E) = 1;
        }
        v = *(unsigned char *)(arg0 + 3);
        if (v == 8 || v == 0x16) {
            *(unsigned short *)(arg0 + 0x2E) = 1 - *(unsigned short *)(arg0 + 0x2E);
        }
        *(float *)0x700038A0 = *(float *)(arg0 + 0xB0) - 6.0f * func_0011E2A8(*(float *)(p + 0xC4));
        *(float *)0x700038A4 = *(float *)(p + 0xA4);
        *(float *)0x700038A8 = *(float *)(arg0 + 0xB8) - 6.0f * func_0011DE90(*(float *)(p + 0xC4));
        *(int *)0x700038AC = 0x3F800000;
        func_00182F90(p, D_700038A0);
        if (arg2 == 0) {
            func_001BBD60(arg0, D_0024D980);
            func_001BA1A0(arg1, D_0024D900);
            if (D_00810700 == 0x16) {
                if (!(*(short *)(arg0 + 0x34) & 0x80)) {
                    unsigned char *q = (unsigned char *)D_0024E140[D_00810700 & 0xFF];
                    q += (*(unsigned char *)(arg0 + 0x34) & 0x7F) * 4;
                    idx = *(q + *(unsigned short *)(arg0 + 0x2E));
                    if (idx == 1) {
                        D_008106C8 |= 0x80;
                    } else if (idx == 2) {
                        D_008106C8 &= ~0x80;
                    }
                }
            }
        } else {
            func_001BA1A0(arg1, D_0024DA40);
        }
        func_001BA1F0(arg0);
        return 1;
    }
    return 0;
}
