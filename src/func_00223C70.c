// func_00223C70 — byte-identical match.
//
// objdiff reports slightly under 100% here, and objdiff is WRONG about it.
// The residual is entirely splat rendering a 0x7000xxxx scratchpad access as
// a bare literal in load/store context (it only symbolizes lui+addiu pairs),
// so the EXPECTED object carries a constant where our compiled object carries
// the %hi/%lo relocation pair. Both encode the same bytes once relocated.
// Proven by the stronger oracle: this function is COMPILED and LINKED into the
// boot ELF, which remains byte-identical to the original. That is a direct test
// of the emitted bytes, unlike objdiff's object-level comparison.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// PREREQUISITE: tools/decomp/build.py `_SPAD_SYMS` must include "0x70003B7E" and
// "0x70003B7C" so normalize_asm() symbolizes this function's expected .s the same
// way this source does. Without it the expected object keeps bare 0x7000 literals
// where this object carries R_MIPS_HI16/LO16 relocations and objdiff reports
// 99.9794% (the linked bytes are identical either way). With it: 100.0%.
//
// Spelling the two scratchpad reads as externs is load-bearing beyond the reloc
// form: mwcc will not speculate a RELOCATED lui into a branch delay slot, which is
// exactly what stops it peeling the second test's `lui at,0x7000` into the first
// beqz's delay slot (the target leaves a nop there). The declarations must be
// incomplete arrays — a scalar `unsigned short` lands in small-data under
// -sdatathreshold 4 and collapses to a single %gp_rel lhu.
//
// SEMANTICS: per-frame state machine for one scripted actor `arg0`, dispatched on
// the state byte at arg0+6 (an if/else compare chain, not a jump table).
//   0      = intro. arg0+0x1F1 selects which of two intro lines plays
//            (func_001FBD50 clip 0x152 / 0x153) alongside a camera shake
//            (func_001B61C0). Branch A waits for the timer at +0x220 to run out,
//            then jumps to state 0x1E if the actor is kind 0x63 or flagged at
//            +0x234, else 0xA. Branch B jumps to 0x14 once the distance at +0x228
//            has reached 100 and the global gate D_008106F1 is set. Otherwise the
//            state just advances.
//   1      = pick and start a reaction clip: 0x185 if bit 1 of arg0+0xF is set
//            (and clear that flag), 0x185 again on a coin flip from func_00122BB8,
//            else 0x184. Advances first.
//   2, 23  = identical "clip finished" (anim flag 0x1000 at +0x200) handoff:
//            arm the 0x3C timer at +0x20E, set +4 = 1, then choose the follow-on
//            behaviour from two hardware/scratchpad button masks ANDed with the
//            global D_00810E70 -- 0x70003B7E -> behaviour 0x20 / clip 0x35,
//            0x70003B7C -> 0x1F / 0x34 (both set the +0x317 flag and clear
//            +0x1F1), neither -> 0x14 / 0x26 with +0x317 cleared.
//   10, 20 = advance and start clip 0x187 / 0x184.
//   11     = on clip end, advance, clear +7 and kick a long camera shake.
//   12     = func_0021D2E0(arg0, 0x78, 1) every frame (hold).
//   21     = wait for the anim time at +0x3C to fall to 30, then advance and
//            func_0021C120.
//   22     = advance once func_0021C190 reports done.
//   30     = advance, start clip 0x186, zero the two grab accumulators
//            (+0x21C, +0x2E4) and set +0x25F = 2.
//   31     = wait for anim flag 0x8000 to clear, advance, set D_00275B00[2] = 1.
//   32     = grab/drag loop. Until the clip ends, track the player's Y/X (through
//            *D_00275B40 + 8 / + 4) as per-frame deltas into +0x38 and +0x2EC,
//            clamp the +0x2EC delta at -4, add it to the facing angle at +0xB4,
//            and drive func_00178B90 / func_00175900.
//   33     = release. func_00179880 feeds the delta back; when func_00175900
//            reports done, detach (func_00182870), say line 0x156 and hand the
//            actor to behaviour 2 / sub-state 3 / clip 0x3F. If the actor is kind
//            0x5D instead, func_0021D250(arg0, 0).

extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern int func_00122BB8(void);
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern void func_0021C120(unsigned char *e);
extern int func_0021C190(unsigned char *e);
extern void func_0021D2E0(unsigned char *e, int a, int b);
extern void func_00178B90(unsigned char *e, int f);
extern int func_00175900(unsigned char *e, int f);
extern void func_00179880(unsigned char *e, void *p);
extern void func_00182870(unsigned char *e, int f);
extern void func_0021D250(unsigned char *e, int f);
extern int **D_00275B40;
extern int D_00275B00;
extern unsigned char D_008106F1[];
extern unsigned short D_00810E70[4];
extern unsigned short D_70003B7E[];
extern unsigned short D_70003B7C[];

void func_00223C70(unsigned char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        if (*(unsigned char *)(arg0 + 0x1F1) == 0) {
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(arg0, 0x152, 0, 300.0f);
            if (*(float *)(arg0 + 0x220) <= 0.0f) {
                if (*(unsigned char *)(arg0 + 0xF) == 0x63 ||
                    *(unsigned char *)(arg0 + 0x234) == 1) {
                    *(unsigned char *)(arg0 + 6) = 0x1E;
                    break;
                }
                *(unsigned char *)(arg0 + 6) = 0xA;
                break;
            }
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            break;
        }
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(arg0, 0x153, 0, 300.0f);
        if (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1[0] != 0) {
            *(unsigned char *)(arg0 + 6) = 0x14;
            break;
        }
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        break;
    case 1:
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        if (*(unsigned char *)(arg0 + 0xF) & 2) {
            func_001749A0(arg0, 0x185, 0, 4.0f);
            *(unsigned char *)(arg0 + 0xF) = 0;
            break;
        }
        if (func_00122BB8() & 1) {
            func_001749A0(arg0, 0x185, 0, 4.0f);
            break;
        }
        func_001749A0(arg0, 0x184, 0, 4.0f);
        break;
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(char *)(arg0 + 4) = 1;
            if (D_00810E70[0] & D_70003B7E[0]) {
                *(char *)(arg0 + 5) = 0x20;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x35;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
                *(char *)(arg0 + 0x317) = 1;
                break;
            }
            if (D_00810E70[0] & D_70003B7C[0]) {
                *(char *)(arg0 + 5) = 0x1F;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x34;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
                *(char *)(arg0 + 0x317) = 1;
                break;
            }
            *(char *)(arg0 + 5) = 0x14;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x26;
            *(char *)(arg0 + 0x317) = 0;
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        func_001749A0(arg0, 0x187, 0, 8.0f);
        break;
    case 11:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            *(char *)(arg0 + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 12:
        func_0021D2E0(arg0, 0x78, 1);
        break;
    case 20:
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        func_001749A0(arg0, 0x184, 0, 4.0f);
        break;
    case 21:
        if (*(float *)(arg0 + 0x3C) <= 30.0f) {
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            func_0021C120(arg0);
        }
        break;
    case 22:
        if (func_0021C190(arg0) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        }
        break;
    case 23:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(char *)(arg0 + 4) = 1;
            if (D_00810E70[0] & D_70003B7E[0]) {
                *(char *)(arg0 + 5) = 0x20;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x35;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
                *(char *)(arg0 + 0x317) = 1;
                break;
            }
            if (D_00810E70[0] & D_70003B7C[0]) {
                *(char *)(arg0 + 5) = 0x1F;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0x34;
                *(unsigned char *)(arg0 + 0x1F1) = 0;
                *(char *)(arg0 + 0x317) = 1;
                break;
            }
            *(char *)(arg0 + 5) = 0x14;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x26;
            *(char *)(arg0 + 0x317) = 0;
        }
        break;
    case 30:
        *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
        func_001749A0(arg0, 0x186, 0, 8.0f);
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        *(char *)(arg0 + 0x25F) = 2;
        break;
    case 31:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            (&D_00275B00)[2] = 1;
        }
        break;
    case 32:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
            break;
        }
        *(float *)(arg0 + 0x38) = *(float *)((char *)*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = *(float *)((char *)*D_00275B40 + 8);
        func_00178B90(arg0, 1);
        *(float *)(arg0 + 0x2EC) = *(float *)((char *)*D_00275B40 + 4) - *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0x2E4) = *(float *)((char *)*D_00275B40 + 4);
        if (*(float *)(arg0 + 0x2EC) < -4.0f) {
            *(float *)(arg0 + 0x2EC) = -4.0f;
        }
        *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 33:
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            func_00182870(arg0, 1);
            func_001FBD50(arg0, 0x156, 0, 300.0f);
            *(char *)(arg0 + 4) = 2;
            *(char *)(arg0 + 5) = 3;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x3F;
            break;
        }
        if (*(unsigned char *)(arg0 + 0x23A) == 0x5D) {
            func_0021D250(arg0, 0);
        }
        break;
    }
}
