// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS: options/difficulty-select screen tick. arg0 points at the current
// menu cursor byte (0..2). The screen block at *(void**)0x70003B6C holds the
// per-screen phase byte at +0xD and a saved-cursor byte at +0x13:
//   phase 0: first frame — bump the phase and snapshot the incoming cursor into
//            +0x13 so a cancel can restore it; falls through into phase 1.
//   phase 1: pad handling from the button word D_00810E74 — 0x2000 (down) steps
//            the cursor up, clamping at 2 (no click at the clamp); 0x8000 (up)
//            steps it down, clamping at 0. Every accepted move clicks
//            func_0020CDA0. The cursor then indexes D_00264F98 for the row of
//            seven 64-bit sprite descriptors drawn on the right-hand gauge.
// The body then draws the whole page: title sprite, the 0x12/0x16 header text,
// the three difficulty buttons (highlighted variant for the selected row), the
// seven option labels 0x1B..0x20 + 0x24, the seven gauge icons from the table,
// and the footer sprite.
// Returns 0 while the screen stays up; on 0x40 (confirm) it commits the choice
// through func_001AF470 and returns 1; on 0x30 (cancel/back) it restores the
// saved cursor from +0x13 and returns 2 for 0x10, else 1.

extern void func_001AF470(unsigned char);
extern int func_001FCBD0(int, int, int, int, unsigned int);
extern int func_001FCE30(int, int, int, int, unsigned int);
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, unsigned int, long long);
extern int func_0020A7A0(long long);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void);
extern int D_00282240;
extern unsigned short D_00810E74;
extern long long *D_00264F98[];

int func_00202D10(unsigned char *arg0) {
    unsigned char *g;
    unsigned char *p;
    long long *tbl;

    g = *(unsigned char **)0x70003B6C;
    p = g + 0xD;
    switch (*(unsigned char *)(g + 0xD)) {
    case 0:
        *p = *p + 1;
        g = *(unsigned char **)0x70003B6C;
        g[0x13] = *arg0;
        /* fallthrough */
    case 1:
        if (D_00810E74 & 0x2000) {
            *arg0 += 1;
            if (*arg0 > 2) {
                *arg0 = 2;
            } else {
                func_0020CDA0();
            }
        } else if (D_00810E74 & 0x8000) {
            if (*arg0 != 0) {
                *arg0 -= 1;
                func_0020CDA0();
            }
        }
        tbl = D_00264F98[*arg0];
        break;
    }

    func_0020A7A0(((long long)0x20043525 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2180);
    func_001FCE30(0xB0, 0x12, D_00282240, 0x16, 0x80808080);
    func_00207D00(1, 0);
    func_00207E40(1, 0x7B40, 0x7F80, 0x100, 0x100, 0x80808080,
                  ((long long)0x20043206 << 0x20) | 0x21321D00);
    if (*arg0 == 0) {
        func_00207E40(1, 0x74B0, 0x7BB0, 0x80, 0x40,
                      0x80808080,
                      ((long long)0x20043EE5 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2120);
    } else {
        func_00207E40(1, 0x74B0, 0x7BB0, 0x80, 0x40,
                      0x80808080,
                      ((long long)0x20043385 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2150);
    }
    if (*arg0 == 1) {
        func_00207E40(1, 0x7CB0, 0x7BB0, 0x80, 0x40,
                      0x80808080,
                      ((long long)0x20043305 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2130);
    } else {
        func_00207E40(1, 0x7CB0, 0x7BB0, 0x80, 0x40,
                      0x80808080,
                      ((long long)0x200433A5 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2160);
    }
    if (*arg0 == 2) {
        func_00207E40(1, 0x84B0, 0x7BB0, 0x80, 0x40,
                      0x80808080,
                      ((long long)0x20043325 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2140);
    } else {
        func_00207E40(1, 0x84B0, 0x7BB0, 0x80, 0x40,
                      0x80808080,
                      ((long long)0x20043505 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2170);
    }
    func_001FCBD0(0x32, 0x3E, D_00282240, 0x1B, 0x80808080);
    func_001FCBD0(0x32, 0x4A, D_00282240, 0x1C, 0x80808080);
    func_001FCBD0(0x32, 0x56, D_00282240, 0x1D, 0x80808080);
    func_001FCBD0(0x32, 0x62, D_00282240, 0x1E, 0x80808080);
    func_001FCBD0(0x32, 0x6E, D_00282240, 0x1F, 0x80808080);
    func_001FCBD0(0x32, 0x7A, D_00282240, 0x20, 0x80808080);
    func_001FCBD0(0x32, 0x86, D_00282240, 0x24, 0x80808080);
    func_00207E40(1, 0x8AE0, 0x7CE0, 0x20, 0x20, 0x80808080, tbl[0]);
    func_00207E40(1, 0x8AE0, 0x7DA0, 0x20, 0x20, 0x80808080, tbl[1]);
    func_00207E40(1, 0x8AE0, 0x7E60, 0x20, 0x20, 0x80808080, tbl[2]);
    func_00207E40(1, 0x8AE0, 0x7F20, 0x20, 0x20, 0x80808080, tbl[3]);
    func_00207E40(1, 0x8AE0, 0x7FE0, 0x20, 0x20, 0x80808080, tbl[4]);
    func_00207E40(1, 0x8AE0, 0x80A0, 0x20, 0x20, 0x80808080, tbl[5]);
    func_00207E40(1, 0x8AE0, 0x8160, 0x20, 0x20, 0x80808080, tbl[6]);
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080,
                  ((long long)0x20043845 << 0x20) | ((long long)0xDD42 << 0x10) | 0x2100);
    if (D_00810E74 & 0x40) {
        func_001AF470(*arg0);
        func_0020CD40();
        return 1;
    }
    if (D_00810E74 & 0x30) {
        func_0020CD60();
        g = *(unsigned char **)0x70003B6C;
        *arg0 = g[0x13];
        return (D_00810E74 & 0x10) ? 2 : 1;
    }
    return 0;
}
