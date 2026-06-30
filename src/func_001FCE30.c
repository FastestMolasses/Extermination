// NEARMISS func_001FCE30  (vram 0x001FCE30, 0xE0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 74.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure/addressing/both dispatch arms byte-identical. Sole residual is the packed-param field extract: target reads the 32-bit 5th arg directly out of $t0 (bare dsll32 $t0,8 / dsrl32 ,8 low-24 zero-extend + srl $t0,24 high byte) with NO operand extension; mwcc materializes a zero/sign-exte...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// objdiff 74.4% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8).
// Body, control flow, addressing and call args are fully recovered: the table
// header walk (D_0028A498[0] -> base = tbl + *tbl, row = tbl + 0x10,
// entry = base + row[idx*4]), the gp-rel scratch stores D_00275828/D_0027582C
// (sdatathreshold 8), the far D_0028A498 forced absolute via the [8]
// over-declaration, and BOTH dispatch arms (the mode-8 centering call to
// func_001CC1E0 and the default func_001FC770 draw) are byte-identical.
//
// Sole residual: the packed-param field extract. The target reads the 32-bit
// 5th arg directly out of $t0 with a bare dsll32 $t0,8 / dsrl32 ,8 (low-24
// zero-extend) plus srl $t0,24 (high byte), inserting no operand extension;
// mwcc materializes a zero-/sign-extension of the incoming arg (dsll32/dsrl32
// ,0) before the shift and re-extends the store value, which also reschedules
// the surrounding loads. Param-extension / instruction-scheduling artifact
// (NOT the clean-store delay nop). -> permuter.
//
// 'flags' packs a context word: low 24 bits -> D_00275828, high byte ->
// D_0027582C. arg2==8 selects centered HUD text.
extern int func_001FE480(int *desc, int a1);
extern int func_001CC170(int s);
extern int func_001CC1E0(int a0, int a1, int a2, int a3, int t0, int t1, int *t2);
extern void func_001FC770(int a0, int a1, int s, void *a3);
extern int *D_0028A498[8];
extern int D_00275828;
extern char D_0027582C;
extern char D_00264CB0[];

void func_001FCE30(int arg0, int arg1, int arg2, int arg3, unsigned int flags) {
    char *tbl;
    int *row;
    char *base;
    int s;

    tbl = (char *)D_0028A498[0];
    row = (int *)(tbl + 0x10);
    base = tbl + *(int *)tbl;
    D_00275828 = (unsigned long long)flags << 0x28 >> 0x28;
    D_0027582C = (char)(flags >> 0x18);
    s = func_001FE480((int *)(base + row[arg2 * 4]), arg3);
    if (arg2 == 8) {
        func_001CC1E0(1, 0x800 - (func_001CC170(s) >> 1), arg1 + 0x790, 0xA, 0x14, s, &D_00275828);
        return;
    }
    func_001FC770(arg0, arg1, s, D_00264CB0);
}
