// NEARMISS func_00209860 (0x00209860, 1412 original bytes).
// Readable source; the original assembly remains linked.
// Corrected ABI and ordinary-secondary digit count: 95.430595% similarity,
// 1436 candidate bytes with mwcc233 -O4,p -sdatathreshold 0.
// Supported inventory branches are checked against original instruction
// execution. Invalid secondary selectors use an incoming saved register
// as TEX0; this C does not define that caller-dependent state.
// See docs/STATUS_AMMO.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SPR4 / ammo HUD block (docs/FINDINGS.md "10. SPR4 / ammo"): draws the 8x8
// blue marker + "SPR4" label, the bullet icon, the reserve-ammo count
// (D_00810CB4, digits via func_001C5FB0/func_00123168 into scratch string
// D_002862C0, blitted via func_001CBA50 with glyph table D_00265510), and
// the optional secondary-weapon row keyed on D_00810CA4/D_00810CA6 (fuel
// percent = D_00810CAE + 100*D_00810CAC when D_00810CA6==4).
extern int float_to_int(float);
extern void func_00122EF0(void *a, void *b);
extern void func_00123168(int a, int b);
extern char *func_001C5FB0(int arg0, int arg1, int arg2);
extern void func_001CBA50(int a, int b, int c, int d, int e, void *f, void *g);
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern void func_00207F80(int a, int b, int c, int d, int e, int f);
extern int D_002672A0;
extern char D_00265510[];
extern char D_00273570;
extern char D_002862C0;
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA6;
extern short D_00810CA8;
extern short D_00810CAA;
extern short D_00810CAC;
extern short D_00810CAE;
extern short D_00810CB0;
extern short D_00810CB4;

void func_00209860(void *ui, int arg0, int arg1) {
    float x0f;
    int x0, y0, x1, y1;
    int reserveX, reserveX2, reserveX3;
    int secX;
    int n;
    int haveSecond;
    unsigned long long secColor;
    char *s;

    haveSecond = 0;
    func_00207D00(1, 3);

    x0f = 16.0f * (float) (arg0 + 0x700);
    x0 = float_to_int(x0f);
    y0 = float_to_int(16.0f * (float) (((arg1 + 2) >> 1) + 0x790));
    x1 = float_to_int(16.0f * (float) (arg0 + 0x708));
    y1 = float_to_int(16.0f * (float) (((arg1 + 0xA) >> 1) + 0x790));
    func_00207F80(1, x0, y0, x1, y1, 0x80CE6000);

    reserveX = float_to_int(16.0f * (float) (arg0 + 0x70C)) >> 4;
    func_001CBA50(1, reserveX, float_to_int(16.0f * (float) ((arg1 >> 1) + 0x790)) >> 4, 0xC, 0xC,
                  (void *) D_002672A0, &D_00265510);

    reserveX = float_to_int(x0f);
    func_00207E40(1, reserveX, float_to_int(16.0f * (float) (((arg1 + 0x48) >> 1) + 0x790)), 0x18,
                  0x18, 0x80808080, 0x2004518555422196ULL);

    reserveX2 = D_00810CB4;
    s = func_001C5FB0(reserveX2, 4, 1);
    func_00123168((int) &D_002862C0, (int) s);

    reserveX3 = float_to_int(16.0f * (float) (arg0 + 0x71A)) >> 4;
    func_001CBA50(1, reserveX3, float_to_int(16.0f * (float) (((arg1 + 0x4C) >> 1) + 0x790)) >> 4,
                  0x10, 0x10, &D_002862C0, &D_00265510);

    n = reserveX2;
    if (D_00810CA4 == 2) {
        n = D_00810CB0;
        secColor = 0x20045385554221C2ULL;
    } else {
        switch (D_00810CA6) {
        case 0:
            goto skip_second;
        case 3:
        case 2:
            n = D_00810CAA;
            secColor = 0x20045305554221A6ULL;
            break;
        case 1:
            n = D_00810CA8;
            secColor = 0x200451A5554221A2ULL;
            break;
        case 4:
            haveSecond = 1;
            n = D_00810CAE + D_00810CAC * 100;
            secColor = 0x20045325554221B2ULL;
            break;
        default:
            /* Invalid secondary selectors reach the draw with incoming s0
             * as TEX0. That register state is not defined by this C ABI;
             * the original assembly remains authoritative for this path. */
            break;
        }
    }

    if (haveSecond == 0) {
        s = func_001C5FB0(n, 3, 1);
        func_00123168((int) &D_002862C0, (int) s);
        func_001CBA50(1, reserveX3, float_to_int(16.0f * (float) (((arg1 + 0x64) >> 1) + 0x790)) >> 4,
                      0x10, 0x10, &D_002862C0, &D_00265510);
    } else {
        s = func_001C5FB0(n, 4, 1);
        func_00123168((int) &D_002862C0, (int) s);
        func_00122EF0(&D_002862C0, &D_00273570);
        if (D_002862C0 == 0x20) {
            secX = float_to_int(16.0f * (float) (arg0 + 0x70A)) >> 4;
            func_001CBA50(1, secX, float_to_int(16.0f * (float) (((arg1 + 0x64) >> 1) + 0x790)) >> 4,
                          0x10, 0x10, &D_002862C0, &D_00265510);
        } else {
            func_001CBA50(1, reserveX3, float_to_int(16.0f * (float) (((arg1 + 0x64) >> 1) + 0x790)) >> 4,
                          0x10, 0x10, &D_002862C0, &D_00265510);
        }
    }
    x0 = float_to_int(x0f);
    func_00207E40(1, x0, float_to_int(16.0f * (float) (((arg1 + 0x60) >> 1) + 0x790)), 0x18, 0x18,
                  0x80808080, secColor);
skip_second:;
}
