// NEARMISS func_001BF6B0  (vram 0x001BF6B0, 0x8DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.958% via mwcc 2.3.3 (-O4,p -sdatathreshold 4); 7 diff rows / 567 instructions.
// (Was 95.90%. s86: fixed 6 externs, applied idiom-27 + idiom-28, and switched the two
// block-leading scratchpad zero-stores to the SYMBOL form.)
//
// Residual, exactly:
//   (a) 4 rows: `lui at,%hi(D_700038A0)` / `sw zero,%lo(D_700038A0)(at)` at the head of
//       case 0 and of the case-3 spread branch. These RELOCATE to precisely the target's
//       `lui at,0x7000` / `sw zero,0x38a0(at)` — byte-identical after linking. They only
//       show as a diff because splat symbolizes lui+addiu pairs for 0x700038A0 but leaves
//       the lui+sw pairs as raw constants, so the expected object cannot carry the reloc.
//       PROVEN (minimal probe, build/agent_ps_1/probe/p1..p6.c): mwcc 2.3.3 speculates a
//       RAW `lui at,0x7000` into the preceding conditional-branch delay slot (and
//       re-materializes it dead at the label) for EVERY raw spelling — absolute cast,
//       volatile, base+index, live-zero value, added `default:`, pointer local. Only the
//       RELOCATED lui is never speculated, which is what leaves the target's NOP. The
//       original source therefore referenced these words by symbol.
//   (b) 3 rows: `sll v1,v0,3 / sra v1,v1,15 / addiu v0,v1,0x12c` vs the same in v0 — a
//       pure two-register colouring permutation in the cooldown RNG. No source spelling
//       moves it (12 respellings tried; a minimal probe colours it differently again, so
//       it is set by surrounding pressure, not by the expression). Permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: per-frame update for one actor (arg0), dispatched on the actor's
// life-cycle state byte arg0[4]: 0 = spawn/init, 1 = active, 2 = dying/fade,
// 3 = despawn. arg0+0x1F0 is the actor's per-behaviour scratch block (`sub`).
//   0: allocate the effect at 0x700038A0 = (0, 2, 5, 1) via func_001BEAC0 and a
//      companion actor via func_001AFA90(2); on either failure jump to state 3.
//      The companion gets our two transforms (+0xB0, +0xC0), type 0x12, flag
//      0xD = 1, think-fn func_001BFFD0 and a back-pointer to us; we cache it at
//      +0x24.  Then bind the model (func_001D0C80/func_001D0D40 with 0x5B),
//      reset the bone pose, seed hp +0x34 = 0x50 and the two gp-rel tables, and
//      precompute the facing frame: rotate (0,30,0,1) by our orientation +0xD0,
//      cross it with the base vector into sub+0xC..0x14 (normalised), do the
//      same for (0,-30,0,1) into a second vector, and store their dot product
//      in sub+8. Advance to state 1.
//   1: func_001B17A0, clear arg0[1]; bail out entirely if func_001B2140 says the
//      actor is not in the player's room; otherwise set arg0[1] = 1 and run the
//      6-step behaviour sub-state machine on arg0[5] (jtbl_0026E260):
//        0: reset the timer +0x28 and advance (falls into 1).
//        1: while func_001BF630 tracks, tick +0x28 and advance at 0x3D frames;
//           a failed track resets the timer.
//        2: wait for the anim flag sub[0] & 0x1000, then advance - but restart
//           at sub-state 0 if the global at 0x700031F4 has reached 0xB.
//        3: after 0x15 frames aim: rotate (0,10,0,1) by +0xD0; if the cached dot
//           sub+8 is below 0.8 use the arctangent of (sub+0xC, sub+0x14) as the
//           spread, else fire straight; on a hit play sound 0x445.
//        4: on the anim flag, release (func_001BF5B0) and set a random cooldown
//           of 300 + rand*360/32768 frames.
//        5: count the cooldown down and wrap to sub-state 0 at zero.
//      Every sub-state first plays sound 0x444 when the room's byte at
//      D_70003B64 + (D_70003B8A << 6) is zero. Tail: decay sub+0x1C, then apply
//      damage flags +0x36 (0x5000 = hit) against hp +0x34, dying at hp <= 0
//      (state 2, sounds 0x15D/0x448 and the death shake func_001C1500).
//      Finally refresh the model and call the actor's own vtable hook +0x4C.
//   2: fade - func_001C1570 ends it; alpha +0x64 decays by 0.01 and +0x38 by
//      0.4, both clamped at 0, and the vtable hook only runs while alpha > 0.
//   3: free the model, the sound slot and the actor.
extern int func_001BEAC0();
extern unsigned char *func_001AFA90();
extern int func_00102948();
extern int func_001D0C80();
extern int func_001D0D40();
extern int bone_init_default_1();
extern int func_001C6380();
extern void func_001026A0(void *dst, void *m, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern float func_00102738(void *u, void *v);
extern int func_001B17A0();
extern int func_001B2140();
extern int func_001BF630();
extern void func_001FBD50(void *actor, int id, int flags, float dist);
extern int func_001BF5B0();
extern int func_001284E0();
extern float func_0011E620(float y, float x);
extern float func_001B1470(float x);
extern int func_00122BB8();
extern int func_001EFE00(int a, void *b);
extern void func_001C1500(void *actor, int mode, float x, float y, float z);
extern int func_001C1570();
extern int func_001D0D60(int model, float rate);
extern int func_001AF890();
extern void func_001B1190(int a);
extern int func_001AFC10();
extern int func_001BFFD0();

extern unsigned char D_008102B0[];
extern int D_0024FD50[];
extern int D_0028A518[2];
extern int D_0028A51C[2];
extern int D_00275638;
extern int D_00275640;
extern int D_700038A0[4];
extern int D_700038B0[4];
extern int D_700038E0[4];
extern int D_700038F0[4];

void func_001BF6B0(unsigned char *a) {
    unsigned char *sub;
    unsigned char *tbl;
    unsigned char *mate;
    short t;
    int anim;
    int flags;

    sub = a + 0x1F0;
    tbl = D_008102B0;
    switch (a[4]) {
    case 0:
        D_700038A0[0] = 0;
        *(volatile int *)0x700038A4 = 0x40000000;
        *(volatile int *)0x700038A8 = 0x40A00000;
        *(volatile int *)0x700038AC = 0x3F800000;
        *(int *)(a + 0x20) = func_001BEAC0(a, D_700038A0, 0x64, 1);
        if (*(int *)(a + 0x20) == 0) {
            a[4] = 3;
            break;
        }
        mate = func_001AFA90(2);
        if (mate != 0) {
            func_00102948(mate + 0xB0, a + 0xB0);
            func_00102948(mate + 0xC0, a + 0xC0);
            mate[3] = 0x12;
            mate[0xD] = 1;
            *(int *)(mate + 0x10) = (int)func_001BFFD0;
            *(int *)(mate + 0x20) = (int)a;
            *(int *)(a + 0x24) = (int)mate;
        } else {
            a[4] = 3;
            break;
        }
        func_001D0C80(a, D_0028A518[0]);
        func_001D0D40(a, D_0024FD50, 0x5B, 1);
        *(short *)(sub + 2) = 0;
        *(short *)(sub + 0) = 0;
        bone_init_default_1(a);
        anim = D_0028A51C[0];
        *(int *)(a + 0x58) = anim;
        *(short *)(a + 0x52) = 1;
        a[0] = 1;
        a[4] = 1;
        *(short *)(a + 0x34) = 0x50;
        *(int *)(a + 0x30) = (int)&D_00275638;
        *(int *)(sub + 0x18) = (int)&D_00275640;
        func_001C6380(a, anim);
        func_00102948(D_700038A0, a + 0xB0);
        *(volatile int *)0x700038B0 = 0;
        *(volatile int *)0x700038AC = 0x3F800000;
        *(volatile int *)0x700038B4 = 0x41F00000;
        *(volatile int *)0x700038B8 = 0;
        *(volatile int *)0x700038BC = 0x3F800000;
        func_001026A0(D_700038B0, a + 0xD0, D_700038B0);
        *(volatile int *)0x700038BC = 0x3F800000;
        func_001028D0(D_700038E0, D_700038B0, D_700038A0);
        *(float *)(sub + 0xC) = *(volatile float *)0x700038E0;
        *(float *)(sub + 0x10) = *(volatile float *)0x700038E4;
        *(float *)(sub + 0x14) = *(volatile float *)0x700038E8;
        func_00102760(D_700038E0, D_700038E0);
        *(volatile int *)0x700038B0 = 0;
        *(volatile int *)0x700038B4 = 0xC1F00000;
        *(volatile int *)0x700038B8 = 0;
        *(volatile int *)0x700038BC = 0x3F800000;
        func_001028B8(D_700038B0, D_700038A0, D_700038B0);
        *(volatile int *)0x700038BC = 0x3F800000;
        func_001028D0(D_700038F0, D_700038B0, D_700038A0);
        func_00102760(D_700038F0, D_700038F0);
        *(float *)(sub + 8) = func_00102738(D_700038E0, D_700038F0);
        *(int *)(sub + 0x1C) = 0;
        break;
    case 1:
        func_001B17A0(a, a[4]);
        a[1] = 0;
        if (func_001B2140(a) == 0) {
            break;
        }
        a[1] = 1;
        switch (a[5]) {
        case 0:
            *(short *)(a + 0x28) = 0;
            a[5] = a[5] + 1;
            /* fallthrough */
        case 1:
            if (func_001BF630(tbl, a, sub) != 0) {
                if (((*(volatile int *)0x70003B64 + (*(volatile short *)0x70003B8A << 6)) & 0xFF) == 0) {
                    func_001FBD50(a, 0x444, 0, 300.0f);
                }
                *(short *)(a + 0x28) += 1;
                if (*(short *)(a + 0x28) > 0x3C) {
                    a[5] = a[5] + 1;
                }
            } else {
                *(short *)(a + 0x28) = 0;
            }
            break;
        case 2:
            if (((*(volatile int *)0x70003B64 + (*(volatile short *)0x70003B8A << 6)) & 0xFF) == 0) {
                func_001FBD50(a, 0x444, 0, 300.0f);
            }
            if (*(short *)(sub + 0) & 0x1000) {
                a[5] = a[5] + 1;
                if (*(volatile int *)0x700031F4 > 0xA) {
                    a[5] = 0;
                } else {
                    func_001BF5B0(a, sub, 1);
                    *(short *)(a + 0x28) = 0;
                }
            }
            break;
        case 3:
            *(short *)(a + 0x28) += 1;
            if (*(short *)(a + 0x28) > 0x14) {
                *(volatile int *)0x700038B0 = 0;
                *(volatile int *)0x700038B4 = 0x41200000;
                *(volatile int *)0x700038B8 = 0;
                *(volatile int *)0x700038BC = 0x3F800000;
                func_001026A0(D_700038B0, a + 0xD0, D_700038B0);
                *(volatile int *)0x700038BC = 0x3F800000;
                if (!(*(float *)(sub + 8) < 0.8f)) {
                    if (func_001284E0(a, D_700038B0, 0, D_700038A0) != 0) {
                        func_001FBD50(a, 0x445, 0, 300.0f);
                    }
                } else {
                    D_700038A0[0] = 0;
                    *(volatile float *)0x700038A4 = func_001B1470(func_0011E620(*(float *)(sub + 0xC), *(float *)(sub + 0x14)));
                    *(volatile int *)0x700038A8 = 0;
                    *(volatile int *)0x700038AC = 0x3F800000;
                    if (func_001284E0(a, D_700038B0, 1, D_700038A0) != 0) {
                        func_001FBD50(a, 0x445, 0, 300.0f);
                    }
                }
                a[5] = a[5] + 1;
            }
            break;
        case 4:
            if (((*(volatile int *)0x70003B64 + (*(volatile short *)0x70003B8A << 6)) & 0xFF) == 0) {
                func_001FBD50(a, 0x444, 0, 300.0f);
            }
            if (*(short *)(sub + 0) & 0x1000) {
                func_001BF5B0(a, sub, 0);
                *(short *)(a + 0x28) = (((func_00122BB8() >> 16) * 0x168) >> 15) + 0x12C;
                a[5] = a[5] + 1;
            }
            break;
        case 5:
            if (((*(volatile int *)0x70003B64 + (*(volatile short *)0x70003B8A << 6)) & 0xFF) == 0) {
                func_001FBD50(a, 0x444, 0, 300.0f);
            }
            t = *(short *)(a + 0x28) - 1;
            *(short *)(a + 0x28) = t;
            if (t == 0) {
                a[5] = 0;
            }
            break;
        }
        if (*(int *)(sub + 0x1C) != 0) {
            *(int *)(sub + 0x1C) = *(int *)(sub + 0x1C) - 1;
        }
        flags = *(short *)(a + 0x36);
        if (flags != 0) {
            if ((short)flags & 0x5000) {
                if (*(int *)(sub + 0x1C) == 0) {
                    *(int *)(sub + 0x1C) = 0x3C;
                    if (*(short *)(a + 0x36) & 0x4000) {
                        func_001EFE00(0x80000027, a);
                    }
                } else {
                    *(short *)(a + 0x36) = 0;
                }
            }
            *(short *)(a + 0x34) -= *(short *)(a + 0x36) & 0xFFF;
            if (*(short *)(a + 0x34) <= 0) {
                a[0] = 2;
                *(short *)(a + 0x34) = 0;
                a[4] = 2;
                a[5] = 0;
                func_001FBD50(a, 0x15D, 0, 300.0f);
                func_001FBD50(a, 0x448, 0, 300.0f);
                func_001C1500(a, 1, 4.0f, 6.0f, 4.0f);
            } else {
                a[0] = 1;
                *(short *)(a + 0x36) = 0;
            }
        }
        func_001C6380(a);
        *(short *)(sub + 0) = func_001D0D60(*(int *)(a + 0x90), 1.0f);
        (*(void (**)(unsigned char *))(a + 0x4C))(a);
        break;
    case 2:
        if (func_001C1570(a) != 0) {
            a[4] = 3;
        }
        *(float *)(a + 0x64) -= 0.01f;
        if (*(float *)(a + 0x64) < 0.0f) {
            *(int *)(a + 0x64) = 0;
        }
        *(float *)(a + 0x38) -= 0.4f;
        if (*(float *)(a + 0x38) < 0.0f) {
            *(int *)(a + 0x38) = 0;
        }
        func_001BF5B0(a, sub, 0);
        func_001D0D60(*(int *)(a + 0x90), 1.0f);
        func_001C6380(a);
        if (*(float *)(a + 0x64) != 0.0f) {
            (*(void (**)(unsigned char *))(a + 0x4C))(a);
        }
        break;
    case 3:
        func_001AF890(*(int *)(a + 0x90), a[4]);
        func_001B1190(a[0x9A]);
        func_001AFC10(a);
        break;
    }
}
