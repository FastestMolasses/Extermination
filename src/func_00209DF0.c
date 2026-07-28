// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Region-select screen render (region byte at arg0+0x11, values 1..4).
// Draws four 3-vertex marker strips via func_00208750 (highlight table
// D_00265570 when the strip index matches the selected region, else
// D_00265540), writes the 432x272 config pair to scratchpad 0x700038A0 and
// calls func_0020AC70, then animates four rotating quad groups
// (D_002651B0/D_00265210 spinners stepping +90 deg, D_00265270/2D0/330
// position trio driven by a per-region w/h table, color flip to 240/200
// when i == region-1). Finishes with the header/percent readout
// (float_to_int(D_0081085C), 100% shows the D_00265520 string, otherwise
// formats the number via func_001C5FB0/func_00123168/func_00122EF0 and
// blits with func_001CBA50), four 0x20-px glyph quads, two 0x80-px panels,
// four small text rows, and three marker dots plus one full-screen quad.
// Matching keys: the d/c/b/a declaration order in the last loop is
// load-bearing (it colors the four float_to_int results s2/s1/s0/t0 to
// match the target); x/y must be the affine forms i*0xC+0x4E / i*0xC+0x54
// (explicit += induction variables make mwcc strength-reduce (x>>1) into
// extra halved induction registers).

extern int float_to_int(float);
extern void func_00122EF0(void *, void *);
extern void func_00123168(void *, int);
extern int func_001C5FB0(int, int, int);
extern void func_001CBA50(int, int, int, int, int, void *, void *);
extern void func_001CC1E0(int, int, int, int, int, void *, void *);
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, unsigned int, long long);
extern void func_00207F80(int, int, int, int, int, unsigned int);
extern void func_002082B0(int, volatile float *);
extern void func_00208750(int, float *, void *);
extern void func_00208AD0(void *, int, int);
extern void func_00209280(void *, int, int, long long, int);
extern void func_00209860(void *, int, int);
extern void func_0020AC70(void *, float *, int);

extern volatile float D_002651B0;
extern volatile float D_002651B4;
extern volatile float D_002651B8;
extern volatile float D_002651BC;
extern volatile float D_00265210;
extern volatile float D_00265214;
extern volatile float D_00265218;
extern volatile float D_0026521C;
extern volatile float D_00265270;
extern volatile float D_00265274;
extern volatile float D_002652D0;
extern volatile float D_002652D4;
extern volatile float D_00265330;
extern volatile float D_00265334;
extern volatile float D_00265354;
extern volatile float D_00265358;
extern volatile float D_00265364;
extern volatile float D_00265368;
extern volatile float D_00265374;
extern volatile float D_00265378;
extern volatile float D_00265384;
extern volatile float D_00265388;
extern char D_00265510[];
extern char D_00265520[];
extern char D_00265530[];
extern char D_00265538[];
extern char D_00265540[];
extern char D_00265570[];
extern void *D_00267290;
extern void *D_00267294;
extern void *D_002672A4;
extern void *D_002672A8;
extern void *D_002672AC;
extern void *D_002672B0;
extern char D_00273570[];
extern char D_002862C0[];
extern float D_0081085C;
extern float D_700038A0;

void func_00209DF0(unsigned char *arg0) {
    float buf[16];
    int i;
    int w;
    int h;
    int n;
    int x;
    int y;

    func_00207D00(1, 0);

    buf[0] = 35584.0f;
    buf[1] = 33952.0f;
    buf[2] = 0.0f;
    buf[3] = 0.0f;
    buf[4] = 35584.0f;
    buf[5] = 34496.0f;
    buf[6] = 0.0f;
    buf[7] = 0.0f;
    buf[8] = 35584.0f;
    buf[9] = 36096.0f;
    buf[10] = 0.0f;
    buf[11] = 0.0f;
    if (arg0[0x11] == 1) {
        func_00208750(0x10, buf, D_00265570);
    } else {
        func_00208750(0x10, buf, D_00265540);
    }

    buf[0] = 36288.0f;
    buf[1] = 33536.0f;
    buf[2] = 0.0f;
    buf[3] = 0.0f;
    buf[4] = 37376.0f;
    buf[5] = 33536.0f;
    buf[6] = 0.0f;
    buf[7] = 0.0f;
    buf[8] = 42496.0f;
    buf[9] = 33536.0f;
    buf[10] = 0.0f;
    buf[11] = 0.0f;
    if (arg0[0x11] == 2) {
        func_00208750(0x10, buf, D_00265570);
    } else {
        func_00208750(0x10, buf, D_00265540);
    }

    buf[0] = 35584.0f;
    buf[1] = 33120.0f;
    buf[2] = 0.0f;
    buf[3] = 0.0f;
    buf[4] = 35584.0f;
    buf[5] = 32576.0f;
    buf[6] = 0.0f;
    buf[7] = 0.0f;
    buf[8] = 35584.0f;
    buf[9] = 30976.0f;
    buf[10] = 0.0f;
    buf[11] = 0.0f;
    if (arg0[0x11] == 3) {
        func_00208750(0x10, buf, D_00265570);
    } else {
        func_00208750(0x10, buf, D_00265540);
    }

    buf[0] = 34880.0f;
    buf[1] = 33536.0f;
    buf[2] = 0.0f;
    buf[3] = 0.0f;
    buf[4] = 33792.0f;
    buf[5] = 33536.0f;
    buf[6] = 0.0f;
    buf[7] = 0.0f;
    buf[8] = 28672.0f;
    buf[9] = 33536.0f;
    buf[10] = 0.0f;
    buf[11] = 0.0f;
    if (arg0[0x11] == 4) {
        func_00208750(0x10, buf, D_00265570);
    } else {
        func_00208750(0x10, buf, D_00265540);
    }

    *(float *)0x700038A0 = 432.0f;
    *(float *)0x700038A4 = 272.0f;
    func_0020AC70(arg0, &D_700038A0, 0);
    func_00207D00(1, 0);

    D_002651B0 = 35584.0f;
    D_002651B4 = 33536.0f;
    D_00265210 = 35584.0f;
    D_00265214 = 33536.0f;
    D_002651B8 = -44.0f;
    D_002651BC = 44.0f;
    D_00265218 = -44.0f;
    D_0026521C = 44.0f;

    for (i = 0; i < 4; i++) {
        func_002082B0(1, &D_002651B0);
        func_002082B0(1, &D_00265210);
        D_002651B8 += 90.0f;
        D_002651BC += 90.0f;
        D_00265218 += 90.0f;
        D_0026521C += 90.0f;
        switch (i) {
        case 3:
            w = 0x184;
            h = 0x140;
            break;
        case 1:
            w = 0x1DC;
            h = 0x140;
            break;
        case 2:
            w = 0x1B0;
            h = 0x108;
            break;
        case 0:
            w = 0x1B0;
            h = 0x178;
            break;
        }
        D_00265270 = 16.0f * (float)(w + 0x700);
        D_00265274 = 16.0f * (float)((h >> 1) + 0x790);
        D_002652D0 = 16.0f * (float)(w + 0x700);
        D_002652D4 = 16.0f * (float)((h >> 1) + 0x790);
        D_00265330 = 16.0f * (float)(w + 0x700);
        D_00265334 = 16.0f * (float)((h >> 1) + 0x790);
        if (arg0[0x11] != 0 && i == arg0[0x11] - 1) {
            D_00265354 = 240.0f;
            D_00265358 = 0.0f;
            D_00265364 = 200.0f;
            D_00265368 = 0.0f;
            D_00265374 = 240.0f;
            D_00265378 = 0.0f;
            D_00265384 = 200.0f;
            D_00265388 = 0.0f;
        } else {
            D_00265354 = 128.0f;
            D_00265358 = 255.0f;
            D_00265364 = 64.0f;
            D_00265368 = 64.0f;
            D_00265374 = 128.0f;
            D_00265378 = 255.0f;
            D_00265384 = 64.0f;
            D_00265388 = 64.0f;
        }
        func_002082B0(1, &D_00265270);
        func_002082B0(1, &D_002652D0);
        func_002082B0(1, &D_00265330);
    }

    func_00208AD0(arg0, 0xD0, 0xC4);
    func_00209280(arg0, 0x10, 0x76,
                  ((long long)0x20045125 << 0x20) | 0x15422288, 0);
    func_00209860(arg0, 0x10, 0xBE);

    n = float_to_int(D_0081085C);
    if (n == 0x64) {
        func_001CC1E0(1, 0x822, 0x812, 0xA, 0x14, D_00267290, D_00265520);
    } else {
        func_001CC1E0(1, 0x828, 0x812, 0xA, 0x14, D_00267294, 0);
        func_00123168(D_002862C0, func_001C5FB0(n, 3, 1));
        func_00122EF0(D_002862C0, D_00273570);
        func_001CBA50(1, 0x828, 0x820, 0x10, 0x10, D_002862C0, D_00265510);
    }

    func_00207D00(1, 3);
    func_00207E40(1, 0x70B0, 0x8280, 0x20, 0x20, 0x80808080,
                  ((long long)0x20045EC5 << 0x20) | 0x55422186);
    func_00207E40(1, 0x8E40, 0x8280, 0x20, 0x20, 0x80808080,
                  ((long long)0x20045EC5 << 0x20) | 0x554221F0);
    func_00207E40(1, 0x8A10, 0x7950, 0x20, 0x20, 0x80808080,
                  ((long long)0x20045EC5 << 0x20) | 0x55422192);
    func_00207E40(1, 0x8A10, 0x85B0, 0x20, 0x20, 0x80808080,
                  ((long long)0x20045EC5 << 0x20) | 0x554221F4);
    func_00207D00(1, 0);
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080,
                  ((long long)0x20045505 << 0x20) | ((long long)0xDD42 << 0x10) | 0x1D40);
    func_00207E40(1, 0x7100, 0x7900, 0x80, 0x40, 0x80808080,
                  ((long long)0x200453A5 << 0x20) | ((long long)0x9D42 << 0x10) | 0x1E50);
    func_001CBA50(1, 0x710, 0x7AC, 0xC, 0x10, D_002672A4, D_00265538);
    func_001CBA50(1, 0x718, 0x7B5, 0xA, 0xA, D_002672A8, D_00265530);
    func_001CBA50(1, 0x718, 0x7BB, 0xA, 0xA, D_002672AC, D_00265530);
    func_001CBA50(1, 0x718, 0x7C1, 0xA, 0xA, D_002672B0, D_00265530);

    for (i = 0; i < 3; i++) {
        int d;
        int c;
        int b;
        int a;
        x = i * 0xC + 0x4E;
        y = i * 0xC + 0x54;
        a = float_to_int(28960.0f);
        b = float_to_int(16.0f * (float)((x >> 1) + 0x790));
        c = float_to_int(29024.0f);
        d = float_to_int(16.0f * (float)((y >> 1) + 0x790));
        func_00207F80(1, a, b, c, d, 0x80CE6000);
    }
    func_00207D00(1, 0);
    func_00207F80(1, 0x7800, 0x8380, 0x8800, 0x8680, 0x40404040);
}
