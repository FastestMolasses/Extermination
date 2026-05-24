// CFLAGS: -O4,p -sdatathreshold 0
extern short D_00289B40;
extern short D_0024A850;

void func_001B0BA0(void) {
    int v1;
    short *a0;
    short *a1;
    int a2;
    int a3;
    a0 = &D_00289B40;
    a1 = &D_0024A850;
    a2 = 0;
    a3 = 0;
    while (a3 < 0x17) {
        a0[0] = a2;
        v1 = a1[0];
        a0[1] = v1;
        v1 = a1[0];
        if (v1 == 0) {
            a2++;
        } else {
            a2 += v1;
        }
        a3++;
        a0 += 2;
        a1++;
    }
}
