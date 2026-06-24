// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001026A0(void *a, void *b, void *c);
extern void func_00182F90(void *a, void *b);
extern float func_001B1470(float);
extern void func_001BA1A0(int, void *);
extern void func_001BA1F0(void *);
extern char D_0024E3A0[];
extern char D_0024E560[];
extern char D_008102B0[];
extern char D_700038A0[];

int func_001BC740(char *arg0, int arg1) {
    char *g = D_008102B0;
    if (*(unsigned char *)(arg0 + 0xB) & 4) {
        *(float *)(g + 0xC4) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
        *(int *)0x700038A0 = 0x3E99999A;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x40A00000;
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0(D_700038A0, arg0 + 0xD0, D_700038A0);
        *(float *)0x700038A4 = *(float *)(g + 0xA4);
        func_00182F90(g, D_700038A0);
        if (*(unsigned char *)(arg0 + 3) == 0) {
            func_001BA1A0(arg1, D_0024E3A0);
        } else {
            func_001BA1A0(arg1, D_0024E560);
        }
        func_001BA1F0(arg0);
        *arg0 = 2;
        return 1;
    }
    return 0;
}
