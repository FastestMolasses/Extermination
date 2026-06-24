// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
extern int func_001FEF70(int);
extern void func_001FF080(int, int);
extern void func_001FB9F0(int, int, int, int);
extern int func_0015C7B0(void *);
extern void func_0020E080(void);
extern void func_001AFEB0(void);
extern signed char D_008106CC[16];
extern int D_002821B4[16];
extern char D_008102B0[16];
extern unsigned char D_00810C60[16];
extern unsigned char D_00275BD8;

int func_0020E0C0(char *arg0) {
    char *self;
    int r;

    self = arg0;
    switch (*(unsigned char *)(arg0 + 2)) {
    case 0:
        D_008106CC[0] = 1;
        D_002821B4[0] = 2;
        *(char *)(self + 2) = 2;
        r = func_001FEF70(2);
        if (r != -1 && *(int *)(self + 8) != r) {
            D_00275BD8 = 1;
            func_001FF080(0, r & 0xFF);
            *(char *)(self + 2) = 1;
        }
        goto ret0;
    case 1:
        if (D_00275BD8 == 0) {
            *(char *)(self + 2) = 2;
            func_001FB9F0(0xD, 0x1000, 0x1000, 0x1000);
        }
        goto ret0;
    case 2:
        if (*(unsigned char *)(self + 0xC) == 1 && D_00810C60[0] == 2) {
            func_0015C7B0(D_008102B0);
        }
        func_0020E080();
        func_001AFEB0();
        *(char *)(self + 0) = 0;
        *(char *)(self + 1) = 0;
        *(char *)(self + 2) = 0;
        *(char *)(self + 3) = 0;
        return 1;
    default:
    ret0:
        return 0;
    }
}
