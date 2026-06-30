// NEARMISS func_00228320  (vram 0x00228320, 0x8C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.63% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation / instruction-scheduling: CW keeps the 3-int stack temp via dead-store+reload through computed pointers and splits the additive constant as 0xA then +8 (two addiu); mwcc reorders lui/reload and folds 0xA+8 into one 0x12 addiu. Body, struct copy, and subu/bgez compare all recov...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS (79.2% vs build/expected/func_00228320.o, mwcc 991202): full logic.
// Copies the 12-byte block at D_00266B18 into a 3-int stack temp, then
// overwrites it: [0] = (D_0028A4A0->field_0x18 + 0x3FF) >> 10, [1]=0, [2]=0.
// Returns 1 if arg0->field_0x5C - (0xA + temp[0] + temp[1] + temp[2] + 8) >= 0,
// else 0 (PADDUB zero,zero idiom = return 0).
// Wall: register-allocation / instruction-scheduling. CodeWarrior keeps the
// three temp words live with dead-store + reload through computed pointers and
// splits the additive constant as 0xA then +8 in two addiu; mwcc reorders the
// lui/reload sequence and folds 0xA+8 into a single 0x12. Pure codegen artifact;
// body and the subu/bgez compare shape are exact.
extern int D_00266B18;
extern int *D_0028A4A0;

int func_00228320(char *arg0) {
    int sp[3];

    *(long long *)sp = *(long long *)&D_00266B18;
    sp[2] = *(int *)((char *)&D_00266B18 + 8);
    sp[0] = (*(int *)((char *)D_0028A4A0 + 0x18) + 0x3FF) >> 0xA;
    sp[1] = 0;
    sp[2] = 0;
    if (*(int *)(arg0 + 0x5C) - (0xA + sp[0] + sp[1] + sp[2] + 8) < 0) {
        return 0;
    }
    return 1;
}
