// NEARMISS func_00112B90  (vram 0x00112B90, 0xC4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 75.67% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Branch-likely wall: byte-identical through offset 0x74; sole divergence is expected bgezl (branch-likely) at the func_0010E8A8 return-value check vs our bgez. This ee-gcc 2.9-991111-01 build never emits bnel/bgezl/beql (documented hard wall); needs a different SDK compiler, not a permutation. Best 75.7%.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00112088(void);
extern int func_00111F18(int);
extern int func_0010E8A8(void *a, int b, int c, int d, int e, int f, int g, void *h, void *i);
extern int CreateSema(void *);
extern void func_001115D0(void);

extern int D_00241D48;
extern int D_00241D14;
extern int D_00241D08;
extern int D_00279E00;

int func_00112B90(void)
{
    if (func_00112088() == 6) {
        return 0;
    }

    if (func_00111F18(0xB) == 0) {
        return 0;
    }

    D_00241D48 = 6;
    D_00241D14 = 1;

    if (func_0010E8A8(&D_00279E00, 7, 1, 0, 0, 0, 0, func_001115D0, &D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        CreateSema((void *)D_00241D08);
        return 0;
    }

    CreateSema((void *)D_00241D08);
    return 1;
}
