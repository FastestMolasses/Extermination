// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Issues four func_001D64A0(self, g0[3], g1[3], &q, 0x40) primitive-setup calls,
// each preceded by writing two 3-int command groups (g0/g1) with fixed 16-bit
// literal id pairs. q is a 16-byte template copied from D_00253240 whose 4th word
// is overwritten with arg1 (the user/context value) before the calls; func_001D1F80
// resets state first. (self=arg0 held in s4 across the calls.)
typedef struct { int x[4]; } Qword;
extern Qword D_00253240;
extern void func_001D1F80(int, int, int);
extern void func_001D64A0(int, int *, int *, Qword *, int);

void func_001DB830(int a0, int a1) {
    int g0[4];
    int g1[4];
    Qword q;

    q = D_00253240;
    q.x[3] = a1;
    func_001D1F80(a0, 0, 1);

    g0[0] = 0x6FF0;
    g0[1] = 0x78F0;
    g0[2] = 0x0FFFFFFF;
    g1[0] = 0x9010;
    g1[1] = 0x7B20;
    g1[2] = 0x0FFFFFFF;
    func_001D64A0(a0, g0, g1, &q, 0x40);

    g0[0] = 0x6FF0;
    g0[1] = 0x8710;
    g0[2] = 0x0FFFFFFF;
    g1[0] = 0x9010;
    g1[1] = 0x84E0;
    g1[2] = 0x0FFFFFFF;
    func_001D64A0(a0, g0, g1, &q, 0x40);

    g0[0] = 0x77F0;
    g0[1] = 0x7B20;
    g0[2] = 0x0FFFFFFF;
    g1[0] = 0x8810;
    g1[1] = 0x7BA0;
    g1[2] = 0x0FFFFFFF;
    func_001D64A0(a0, g0, g1, &q, 0x40);

    g0[0] = 0x77F0;
    g0[1] = 0x84E0;
    g0[2] = 0x0FFFFFFF;
    g1[0] = 0x8810;
    g1[1] = 0x8460;
    g1[2] = 0x0FFFFFFF;
    func_001D64A0(a0, g0, g1, &q, 0x40);
}
