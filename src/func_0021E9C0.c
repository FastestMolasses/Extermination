// NEARMISS func_0021E9C0  (vram 0x0021E9C0, 0x10C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-coloring + constant-CSE artifact: target uses call-clobbered a1 for the switch-dispatch constant 1 and reloads 1 per call arg; mwcc233 colors it into preserved temp v1 and CSEs it into the func_0017C540/func_00178B90 arg moves (a1-vs-v1 mismatch + 2 extra paddub a0,v1 moves, 4 insns). St...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 96.87% (mwcc233): body + control flow fully recovered. Sole residual
// is a register-coloring + constant-CSE artifact: target materializes the
// switch-dispatch constant 1 in call-clobbered a1 (addiu a1,zero,1; beq v0,a1,
// s0=e in delay slot) and freshly reloads 1 for each (1) call arg, while mwcc
// colors it into preserved temp v1 and CSEs it into the func_0017C540/func_00178B90
// argument moves (paddub a0,v1,zero) -> a1-vs-v1 mismatch + 2 extra moves
// (4 differing insns). Stable across -O3/-O4/-O4,s/-inline off and
// switch/default/if-else restructurings. Not the clean-store nop; permuter-class.
//
// Entity per-state tick dispatch on state byte +6.
//  state 0 (spawn): play sfx func_001FBD50(...,300.0f), request rumble
//    func_001B61C0(0,0xC0,5,1), advance state++ and clear +7, run
//    func_001749A0(e,0x20,0, 1.0f), zero motion accumulators +0x38/+0x21C/+0x2EC.
//  state 1: if (flags +0x200 & 0x1000) clear +0xF, set +0x20E=0x3C, +0x25C=0,
//    func_0017C540(1); else integrate vertical pos delta from *D_00275B40+8 into
//    +0x38 and store it to +0x21C, func_00178B90(1).
//  default: fall through directly to the common tail.
//  Common tail: func_00179880(e, e+0x2EC); func_00175900(e, 1).
extern int func_001FBD50(char *a0, int a1, int a2, float f12);
extern void func_001B61C0(int big, int small, int dur, int force);
extern int func_001749A0(int a0, short a1, int a2, float f12);
extern void func_00179880(void *a0, float *a1);
extern void func_00175900(void *a0, int a1);
extern void func_0017C540(int a0);
extern void func_00178B90(int a0);
extern char **D_00275B40;

void func_0021E9C0(char *e) {
    switch (*(unsigned char *)(e + 6)) {
    case 0:
        func_001FBD50(e, 0x154, 0, 300.0f);
        func_001B61C0(0, 0xC0, 5, 1);
        *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
        *(char *)(e + 7) = 0;
        func_001749A0((int) e, 0x20, 0, 1.0f);
        *(float *)(e + 0x38) = 0.0f;
        *(float *)(e + 0x21C) = 0.0f;
        *(int *)(e + 0x2EC) = 0;
        break;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(char *)(e + 0xF) = 0;
            *(short *)(e + 0x20E) = 0x3C;
            *(char *)(e + 0x25C) = 0;
            func_0017C540(1);
        } else {
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(1);
        }
        break;
    }
    func_00179880(e, (float *)(e + 0x2EC));
    func_00175900(e, 1);
}
