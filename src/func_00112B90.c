// NEARMISS func_00112B90  (vram 0x00112B90, 0xC4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.73% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Identical family member to func_00112AD0 — same eegcc volatile-vs-delay-slot deadlock, same single-instruction residual (fall-through `lw %lo(D_00241D08)` fails to fill the `jal CreateSema` delay slot because the volatile qualifier that is required to force the `bgezl` annul also makes resource_c...
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
    if (func_0010E8A8(D_00279E00, 7, 1, 0, 0, 0, 0, func_001115D0,
                      (void *)&D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        CreateSema(D_00241D08);
        return 0;
    }
    CreateSema(D_00241D08);
    return 1;
}
