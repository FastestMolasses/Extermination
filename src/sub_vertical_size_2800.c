// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00106948(int a0);
extern void func_0010A3A8(char *msg);
extern void func_001063E8(void);
extern void func_001063B8(unsigned int a0);
extern void func_0010AA80(unsigned int a0, int *a1);
extern void func_00106D80(void);
extern int func_0010A650(int *p);

extern int *D_002412F4;
extern int D_00241420;
extern int D_00241424;
extern int D_00241430;
extern int D_00241434;
extern int D_00241438;
extern int D_0024143C;
extern int D_00241440;
extern int D_00241BA0;
extern int D_00241BA4;
extern int D_00241380[];
extern int D_002413C0[];
extern char D_0026B698[];

void sub_vertical_size_2800(void)
{
    unsigned int v;
    int hsize;
    int *pa;
    int *pb;

    ((int *)D_002412F4[0x10])[0x35] = 0;

    v = (unsigned int)func_00106948(0x20);
    D_00241434 = v & 0xF;
    D_00241430 = (v >> 4) & 0xF;
    hsize = (v >> 8) & 0xFFF;
    D_00241424 = hsize;
    D_00241420 = v >> 20;
    if (hsize >= 0xAF1) {
        func_0010A3A8(D_0026B698);
    }

    v = (unsigned int)func_00106948(0x1E);
    D_00241440 = v & 1;
    D_0024143C = (v >> 1) & 0x3FF;
    D_00241438 = v >> 12;

    pa = &D_00241BA0;
    *pa = func_00106948(1);
    if (*pa != 0) {
        func_001063E8();
        func_001063B8(0x50000000);
        func_001063E8();
    } else {
        func_0010AA80(0x50000000, D_00241380);
    }

    pb = &D_00241BA4;
    *pb = func_00106948(1);
    if (*pb != 0) {
        func_001063E8();
        func_001063B8(0x58000000);
        func_001063E8();
    } else {
        func_0010AA80(0x58000000, D_002413C0);
    }

    func_00106D80();
    func_0010A650(D_002412F4);
}
