// NEARMISS func_00112C58  (vram 0x00112C58, 0xC0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.67% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Exact twin of func_00112AD0 (only constants differ: func_00111F18(0xC), D_00241D48=7, func_0010E8A8 a1=8). Same eegcc volatile-vs-delay-slot deadlock, same single-instruction residual: fall-through `lw $a0,%lo(D_00241D08)($v0)` does not enter the `jal CreateSema` delay slot, giving `lw; jal; nop`...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char D_00279E00[];
extern volatile int D_00241D08;
extern volatile int D_00241D48;
extern volatile int D_00241D14;
extern int func_00112088(void);
extern int func_00111F18(int);
extern int func_001115D0();
extern int func_0010E8A8(void *, int, int, int, int, int, int, void *, void *);
extern int CreateSema(int);

int func_00112C58(void)
{
    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(0xC) == 0) {
        return 0;
    }
    D_00241D48 = 7;
    D_00241D14 = 1;
    if (func_0010E8A8(D_00279E00, 8, 1, 0, 0, 0, 0, func_001115D0,
                      (void *)&D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        CreateSema(D_00241D08);
        return 0;
    }
    CreateSema(D_00241D08);
    return 1;
}
