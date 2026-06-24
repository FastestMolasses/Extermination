// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sets up a GS/GIF register sequence for one of seven UI/HUD states selected
// by the byte field p[0x11]. arg1 selects init mode: nonzero clears p[0x11]=0,
// zero calls func_0020D930(p,2). Seeds two floats at the scratch block
// D_700038A0 (0x43540000/0x432A0000), runs func_0020AC70(p,scratch,arg1), then
// emits func_00207E40 register writes bracketed by func_00207D00. Each of the
// six per-state blocks issues a base func_00207E40 (color 0x80808080), and when
// p[0x11] matches that state, swaps to a highlighted color (0x805FFF6E) plus an
// extra func_00207E40. Closes with func_00207D00(1,3) and a final func_00207E40.
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern void func_0020AC70(unsigned char *p, void *q, int n);
extern void func_0020D930(unsigned char *a, int b);
extern char D_700038A0[];

void func_002125B0(unsigned char *p, int arg1) {
    if (arg1 != 0) {
        p[0x11] = 0;
    } else {
        func_0020D930(p, 2);
    }
    *(int *)0x700038A0 = 0x43540000;
    *(int *)0x700038A4 = 0x432A0000;
    func_0020AC70(p, D_700038A0, arg1);
    func_00207D00(1, 0);
    if (p[0x11] == 1) {
        func_00207E40(1, 0x7000, 0x7C10, 0x80, 0x20, 0x805FFF6E, 0x20047F655D4223A0ULL);
        func_00207E40(1, 0x7800, 0x7C10, 0x80, 0x20, 0x805FFF6E, 0x20047FC55D4223A4ULL);
        func_00207E40(1, 0x7940, 0x7C10, 0x20, 0x20, 0x805FFF6E, 0x2004678555322338ULL);
    } else {
        func_00207E40(1, 0x7000, 0x7C10, 0x80, 0x20, 0x80808080, 0x20047F655D4223A0ULL);
        func_00207E40(1, 0x7800, 0x7C10, 0x80, 0x20, 0x80808080, 0x20047FC55D4223A4ULL);
    }
    if (p[0x11] == 2) {
        func_00207E40(1, 0x74C0, 0x7AD0, 0x80, 0x20, 0x805FFF6E, 0x20047F455D422394ULL);
        func_00207E40(1, 0x7AD0, 0x7AC0, 0x20, 0x20, 0x805FFF6E, 0x2004678555322338ULL);
    } else {
        func_00207E40(1, 0x74C0, 0x7AD0, 0x80, 0x20, 0x80808080, 0x20047F455D422394ULL);
    }
    if (p[0x11] == 3) {
        func_00207E40(1, 0x7E20, 0x7A80, 0x80, 0x20, 0x805FFF6E, 0x20047D655D422380ULL);
        func_00207E40(1, 0x7DC0, 0x7AC0, 0x20, 0x20, 0x805FFF6E, 0x2004678555322338ULL);
    } else {
        func_00207E40(1, 0x7E20, 0x7A80, 0x80, 0x20, 0x80808080, 0x20047D655D422380ULL);
    }
    if (p[0x11] == 4) {
        func_00207E40(1, 0x7F20, 0x7C10, 0x80, 0x20, 0x805FFF6E, 0x20047DC55D422384ULL);
        func_00207E40(1, 0x7F30, 0x7C10, 0x20, 0x20, 0x805FFF6E, 0x2004678555322338ULL);
    } else {
        func_00207E40(1, 0x7F20, 0x7C10, 0x80, 0x20, 0x80808080, 0x20047DC55D422384ULL);
    }
    if (p[0x11] == 5) {
        func_00207E40(1, 0x7E20, 0x7DB0, 0x80, 0x20, 0x805FFF6E, 0x20047DE55D422390ULL);
        func_00207E40(1, 0x7DC0, 0x7D60, 0x20, 0x20, 0x805FFF6E, 0x2004678555322338ULL);
    } else {
        func_00207E40(1, 0x7E20, 0x7DB0, 0x80, 0x20, 0x80808080, 0x20047DE55D422390ULL);
    }
    if (p[0x11] == 6) {
        func_00207E40(1, 0x74C0, 0x7D50, 0x80, 0x20, 0x805FFF6E, 0x20047F455D422394ULL);
        func_00207E40(1, 0x7AC0, 0x7D60, 0x20, 0x20, 0x805FFF6E, 0x2004678555322338ULL);
    } else {
        func_00207E40(1, 0x74C0, 0x7D50, 0x80, 0x20, 0x80808080, 0x20047F455D422394ULL);
    }
    func_00207D00(1, 3);
    func_00207E40(1, 0x70B0, 0x7C00, 0x20, 0x20, 0x80808080, 0x20047BC5554223BCULL);
}
