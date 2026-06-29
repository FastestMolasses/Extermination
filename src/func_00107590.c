// COMPILER: eegcc
// CFLAGS: -O2
extern int *D_002412F4;
extern int D_002414AC;
extern int D_0024147C;
extern int D_00241300;
extern int D_00241308;
extern int D_0024130C;
extern int D_00241314;
extern int D_00241318;
extern int D_00241320;

extern void func_00107E88(int a, int b);
extern void func_00107FA0(int a, int b, int c);

void func_00107590(int a0, int a1) {
    int *s0 = (int *)D_002412F4[0x10];
    if (a1 != 0) {
        if (D_002414AC == 3) {
            int a;
            if (D_0024147C == 3) {
                a = D_00241308;
            } else {
                a = D_00241300;
            }
            func_00107E88(a, a0 - 1);
        } else {
            int a, b;
            if (D_0024147C == 3) {
                a = D_00241314;
                b = D_00241320;
            } else {
                a = D_0024130C;
                b = D_00241318;
            }
            func_00107FA0(a, b, a0 - 1);
        }
    }
    if (s0[0x3E] == 1) {
        s0[0x3E] = 2;
    }
}
