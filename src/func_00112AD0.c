// NEARMISS func_00112AD0  (vram 0x00112AD0, 0xC0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.12% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// branch-likely wall: expected emits a forward 'bgezl v0' on the func_0010E8A8(...) < 0 check (with the lui of D_00241D08 in the branch-likely delay slot). ee-gcc 2.9-991111-01 will not generate bgezl from the equivalent C 'if (...) < 0' forward conditional (it emits plain bgez + reorders the D_00241D48/D_00241D14 sto...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char D_00279E00[];
extern volatile int D_00241D08;
extern int D_00241D48;
extern int D_00241D14;
extern int func_00112088(void);
extern int func_00111F18(int a);
extern int func_001115D0();
extern int func_0010E8A8(void *a, int b, int c, int d, int e, int f, int g, void *h, void *i);
extern int CreateSema(int a);

int func_00112AD0(void) {
    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(0xA) == 0) {
        return 0;
    }
    D_00241D48 = 5;
    D_00241D14 = 1;
    if (func_0010E8A8(D_00279E00, 6, 1, 0, 0, 0, 0, func_001115D0, &D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        CreateSema(D_00241D08);
        return 0;
    }
    CreateSema(D_00241D08);
    return 1;
}
