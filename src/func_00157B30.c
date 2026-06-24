// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern float func_001B1240(void *, float, float);
extern void func_001BA1A0(int, void *);
extern void func_001BA1F0(void *);
extern void func_001FB9F0(int, int, int, int);
extern int D_00247E20;
extern int D_00247E74;
extern int D_00247EE0;
extern int D_00247F34;
extern int D_00247FA0;
extern int D_00810350;
extern float D_00810374;
extern float D_00810858;
extern float D_0081085C;
extern unsigned char D_00810C84;

int func_00157B30(unsigned char *arg0, int arg1) {
    if (*(unsigned char *)(arg0 + 0xB) & 4) {
        D_00810374 = func_001B1240(&D_00810350, *(float *)(arg0 + 0xB0), *(float *)(arg0 + 0xB8));
        if (D_00810C84 == 0) {
            D_00247E74 = 0x8000000E;
            func_001BA1A0(arg1, &D_00247E20);
            func_001BA1F0(arg0);
            *(signed char *)(arg0 + 0) = 2;
            return 2;
        }
        if (D_0081085C == 0.0f && !(D_00810858 < 100.0f)) {
            if (*(unsigned char *)(arg0 + 0xB) & 1) {
                func_001FB9F0(0x3EC, 0x1000, 0x1000, 0x1000);
                func_001BA1A0(arg1, &D_00247FA0);
                func_001BA1F0(arg0);
            } else {
                D_00247E74 = 0x80000020;
                func_001BA1A0(arg1, &D_00247E20);
                func_001BA1F0(arg0);
            }
            *(signed char *)(arg0 + 0) = 2;
            return 2;
        }
        if (*(unsigned char *)(arg0 + 0xB) & 1) {
            *(signed char *)(arg0 + 0xA) = 1;
            *(signed char *)(arg0 + 0) = 2;
            return 3;
        }
        D_00247F34 = 0x8000000E;
        func_001BA1A0(arg1, &D_00247EE0);
        func_001BA1F0(arg0);
        return 1;
    }
    return 0;
}
