// NEARMISS func_00112610  (vram 0x00112610, 0x148 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.93% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler + GPR-coloring wall (no permuter lever for this ee-gcc, confirmed s84). Frame size, callee-save set (s0-s4), branch-likely bgezl, and overall body all match after moving the struct-pointer load below the early-return checks (38%->88%). Residual is purely (a) list-scheduler ordering of independen...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00279F40;
extern int D_00279E80;
extern int D_0027A2C0;
extern int D_00241D48;
extern int D_00241D14;
extern int D_00279E00;
extern int D_00241CF8;
extern int D_00241D08;
extern char D_0026BC78;

extern int func_00112088(void);
extern int func_00111F18(int a0);
extern void func_0010DFD8(void);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int t0, int t1, int t2, void *t3, int *sp0);
extern int CreateSema(int param);
extern void func_00122B58(const char *fmt);
extern void func_001115D0(void);

int func_00112610(int a0, int a1, int a2, unsigned char *a3) {
    int *s;
    int r;

    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(5) == 0) {
        return 0;
    }
    s = &D_00279F40;
    s[0] = a0;
    s[1] = a1;
    s[2] = a2;
    ((unsigned char *)s)[0xC] = a3[0];
    ((unsigned char *)s)[0xD] = a3[1];
    ((unsigned char *)s)[0xE] = a3[2];
    s[4] = (int)&D_00279E80;
    s[5] = (int)&D_0027A2C0;
    func_0010DFD8();
    D_00241D48 = 1;
    D_00241D14 = 1;
    r = func_0010E8A8(&D_00279E00, 0xD, 1, s, 0x18, 0, 0, func_001115D0, &D_00241D48);
    if (r < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        CreateSema(D_00241D08);
        return 0;
    }
    if (D_00241CF8 > 0) {
        func_00122B58((const char *)&D_0026BC78);
    }
    CreateSema(D_00241D08);
    return 1;
}
