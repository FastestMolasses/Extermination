// NEARMISS func_001747F0  (vram 0x001747F0, 0x1A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.86% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two register-coloring/scheduling permutations (body byte-identical): (1) original CW defers the arg0->s0 callee-save spill into the first switch `beq` delay slot, keeping the dispatch byte in a1; mwcc spills arg0->s0 eagerly at entry and holds the byte in a0, cascading a1/a0 arg mismatches on the...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Per-frame state driver for actor arg0, dispatched on the state byte at arg0+6.
// State 0 (spawn/intro): advances state, clears arg0+7, fires a sound/cue via
// func_001749A0(0.0f, id, 0) where id is 0x3E or 0x5D depending on the flag at
// arg0+0x236, zeroes the 0x38 timer / 0x21C accumulator / 0x2E4 field, sets the
// 0x25C flag, and kicks func_001FBD50(self, 0x186, 0, 300.0f). State 1: once the
// 0x3C field drops to <= 10.0f, advances state and sets arg0[0]=1; then falls
// through into the state-2 body. State 2 (and the state-1 fallthrough): if the
// 0x1000 bit of the int at arg0+0x200 is set, clears arg0+5/+6/+0x1F0; otherwise
// updates arg0+0x38 = (*D_00275B40)->[8] - arg0+0x21C, latches arg0+0x21C, and
// calls func_00178B90(self, 0). Always: func_001764E0(self); decays arg0+0xB4 by
// 0.2; func_00175900(self, 1); func_001796C0(self); and if arg0[4]==1 &&
// arg0[5]!=0x24 && arg0[0]!=1, sets arg0[0]=1.
//
// NEARMISS: mwcc 2.3.3 reaches 97.857 (991202 89.333). Logic byte-identical; two
// residual compiler artifacts: (1) the original CW defers the arg0->s0 spill into
// the first switch `beq` delay slot, keeping the dispatch byte in a1, whereas mwcc
// spills arg0->s0 eagerly at entry and keeps the byte in a0 (cascades into the
// a1/a0 register-coloring deltas on the dispatch/state-advance); (2) FP coloring
// on the arg0+0xB4 decay add (fv0f vs fv1). Both are scheduling/register-coloring
// permutations, not the clean-store nop; 2.3.3 does not fix. Logic fully recovered.
extern int func_001749A0(float a, int b, int c);
extern void func_00175900(char *p, int a);
extern void func_001764E0(char *p);
extern void func_00178B90(char *p, int a);
extern void func_001796C0(char *p);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char **D_00275B40;

void func_001747F0(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg0 + 7) = 0;
        if (*(unsigned char *)(arg0 + 0x236) == 0) {
            func_001749A0(0.0f, 0x3E, 0);
        } else {
            func_001749A0(0.0f, 0x5D, 0);
        }
        *(int *)(arg0 + 0x38) = 0;
        *(char *)(arg0 + 0x25C) = 1;
        *(int *)(arg0 + 0x21C) = 0;
        *(int *)(arg0 + 0x2E4) = 0;
        func_001FBD50(arg0, 0x186, 0, 300.0f);
        break;
    case 1:
        if (*(float *)(arg0 + 0x3C) <= 10.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(char *)(arg0 + 0) = 1;
        }
        /* fallthrough */
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0;
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(arg0, 0);
        }
        break;
    }
    func_001764E0(arg0);
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -0.2f;
    func_00175900(arg0, 1);
    func_001796C0(arg0);
    if (*(unsigned char *)(arg0 + 4) == 1 && *(unsigned char *)(arg0 + 5) != 0x24 && *(unsigned char *)(arg0 + 0) != 1) {
        *(char *)(arg0 + 0) = 1;
    }
}
