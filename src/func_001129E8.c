// NEARMISS func_001129E8  (vram 0x001129E8, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 79.49% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely wall (confirmed s84): body matches; sole blocker is the func_0010E8A8(...) < 0 syscall-success check where expected emits bgezl $v0 (branch-likely) and our ee-gcc 2.9-991111-01 emits bgez $v0 (non-annul) for the forward conditional. Not source/flag/ISA-crackable; fast-parked.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_0027AB10;
extern int D_00241D48;
extern int D_00241D14;
extern int D_00279E00;
extern int D_00241D08;

extern int func_00112088(void);
extern int func_00111F18(int);
extern void func_0010DFD8(int *, int);
extern int func_0010E8A8(int, int, int, int *, int *, int, int, int);
extern int CreateSema(int);
extern void func_001115D0(void);

int func_001129E8(int a0) {
    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(9) == 0) {
        return 0;
    }

    D_0027AB10 = a0;
    func_0010DFD8(&D_0027AB10, 4);
    D_00241D48 = 4;
    D_00241D14 = 1;

    if (func_0010E8A8((int)&D_00279E00, 5, 1, &D_0027AB10,
                      &D_00241D48, 4, 0, 0) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        CreateSema(D_00241D08);
        return 0;
    }
    CreateSema(D_00241D08);
    return 1;
}
