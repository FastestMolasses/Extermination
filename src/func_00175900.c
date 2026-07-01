// NEARMISS func_00175900  (vram 0x00175900, 0x3EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-permutation wall already characterized on sibling func_001756E0 (identical D_00248950-walk loop): mwcc233 colors the loop counter and walk-pointer into $s0/$s1 in the opposite order from the target; live-range-driven, not moved by declaration reordering. Every instruction outs...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 88.9% (mwcc 2.3.3; pinned 991202 build 78.0%). Logic fully recovered;
// every byte past the loop-counter wall is byte-identical to target.
//
// Per-entity look-ahead/collision-avoidance + status-effect update. arg0 is
// the entity, arg1 is a 'has-real-player-target' flag. First tests a direct
// line-of-sight/collision probe (func_0019AB20) along arg0's forward vector
// (arg0+0xB0) against the world (arg0+0x280, kind 6); if blocked, records the
// hit via func_00175CF0. Otherwise, if arg1 is set, sweeps up to 8 candidate
// yaw offsets from the D_00248950[] table, building a probe direction by
// composing a yaw rotation (func_001029C0/func_00102BB0/func_00102918) with
// arg0's forward vector and re-testing func_0019AB20 each iteration, stopping
// at the first clear direction. Next it re-probes forward with an 18-unit
// extension and, if that also collides (func_0019B6C0), sets the 'blocked'
// flag arg0[0xB], reads the current animation/action id into arg0[0x23A] +
// a wall-clearance timer arg0[0x250], and on ids 0x5A/0x5B/0x5C fires
// one-shot handlers func_00187DC0/00187DE0/00187EA0 (0x5B additionally
// re-probes a shortened distance to pick a 1-vs-2 'partial-clearance' state
// at arg0[0x23C]). If unblocked, it resets those state bytes and re-seeds
// the clearance timer from arg0[0xB4]. Finally, when arg0[0xA] (an
// enemy-alert/nav flag) is set and arg0[0xB]==0, does a secondary
// door/obstacle probe func_0019B8C0 that either copies a player-supplied
// action id or, failing that, derives one from arg0's nav-link record
// (arg0[0x214]) when its type byte masks to 4 and its sub-id is one of
// {2,10,12,24,40,42} (all map to action 4); then clears arg0[0x25F] and, for
// entities not in state 0x1C, zeroes arg0[0xC0]. Returns arg0[0xA].
//
// WALL: body/structure/signature/every constant, the func_001749A0-class
// call-arg fixups, and the id2 case-set (rewritten as a single merged
// if(id2==2||10||12||24||42||40) matching the target's shared store site,
// not a switch) are ALL fully recovered -- every instruction after the
// candidate-yaw loop is byte-identical to the target. Sole residual: the
// classic register-allocation-permutation wall already characterized on the
// sibling func_001756E0 (same D_00248950-walk loop shape) -- mwcc233 colors
// the loop counter 'i' and the D_00248950 walk-pointer 'p' into $s0/$s1
// while the target uses $s1/$s0; per the sibling's finding this is
// live-range-driven and NOT moved by declaration reordering (confirmed here
// too: swapping declaration order only marginally changed the score). This
// wall shifts every branch target inside/after the loop by a constant 4
// bytes but changes no other instruction.
extern float func_001B1470(float);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, char *);
extern void func_00102948(void *, void *);
extern void func_001026A0(void *, void *, int *);
extern int func_0019AB20(char *, void *, char *, int);
extern void func_00175CF0(char *, int, int, void *);
extern int func_0019B6C0(void *, char *);
extern int func_0019B8C0(char *, void *, char *, int);
extern void func_001031E0(void *, char *);
extern void func_00187DC0(char *);
extern void func_00187DE0(char *);
extern void func_00187EA0(char *);
extern float D_00248950;
extern int D_700036A0;
extern int D_700038A0;

unsigned char func_00175900(char *arg0, int arg1) {
    float sp50[4];
    void *lookVec;
    int sp60;
    int sp64;
    int sp68;
    int sp6C;
    int i;
    float *p;
    int hit;
    unsigned char v;

    if (*(unsigned char *)(arg0 + 0x1F0) != 0x30) {
        if (*(unsigned char *)(arg0 + 0x23B) != 0x35) {
            *(unsigned char *)(arg0 + 0x23B) = 0;
            *(int *)(arg0 + 0x9C) = 0;
        }
    }
    lookVec = arg0 + 0xB0;

    hit = func_0019AB20(arg0, lookVec, arg0 + 0x280, 6);
    if (hit != 0) {
        func_00102948(sp50, arg0 + 0xB0);
        func_00175CF0(arg0, hit, 0, arg0 + 0xB0);
    } else if (arg1 != 0) {
        sp60 = 0;
        sp64 = 0;
        sp68 = 0x40000000;
        sp6C = 0x3F800000;
        p = &D_00248950;
        i = 0;
        do {
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + *p));
            func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
            func_001026A0(&D_700038A0, &D_700036A0, &sp60);
            hit = func_0019AB20(arg0, &D_700038A0, arg0 + 0x280, 6);
            if (hit != 0) {
                func_00102948(sp50, &D_700038A0);
                func_00175CF0(arg0, hit, i + 1, &D_700038A0);
                break;
            }
            i += 1;
            p += 1;
        } while (i < 8);
    }

    func_001031E0(&D_700038A0, arg0 + 0xB0);
    *(float *)0x700038A4 = *(float *)0x700038A4 + 18.0f;
    if (func_0019B6C0(&D_700038A0, arg0 + 0xB0) != 0) {
        *(unsigned char *)(arg0 + 0xB) = 1;
        *(unsigned char *)(arg0 + 0x23A) = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
        *(float *)(arg0 + 0x250) = *(float *)0x700031B4;
        v = *(unsigned char *)(arg0 + 0x23A);
        if (v == 0x5A) {
            if (*(unsigned char *)(arg0 + 0x23D) == 0) {
                *(unsigned char *)(arg0 + 0x23D) = 1;
                func_00187DC0(arg0);
            }
        } else if (v == 0x5B) {
            if (*(unsigned char *)(arg0 + 0x23C) == 0) {
                int c;
                func_001031E0(&D_700038A0, arg0 + 0xB0);
                *(float *)0x700038A4 = *(float *)(arg0 + 0x250) - 4.01f;
                if (func_0019AB20(arg0, &D_700038A0, arg0 + 0x280, 6) != 0) {
                    c = 1;
                } else {
                    c = 2;
                }
                *(unsigned char *)(arg0 + 0x23C) = c;
                func_00187DE0(arg0);
            }
        } else if (v == 0x5C) {
            if (*(unsigned char *)(arg0 + 0x23E) == 0) {
                *(unsigned char *)(arg0 + 0x23E) = 1;
                func_00187EA0(arg0);
            }
        }
    } else {
        *(unsigned char *)(arg0 + 0x23C) = 0;
        *(unsigned char *)(arg0 + 0x23D) = 0;
        *(unsigned char *)(arg0 + 0x23E) = 0;
        *(float *)(arg0 + 0x250) = *(float *)(arg0 + 0xB4);
    }

    if (*(unsigned char *)(arg0 + 0xA) != 0) {
        if (*(unsigned char *)(arg0 + 0xB) == 0) {
            if (func_0019B8C0(arg0, sp50, arg0 + 0x280, 7) != 0) {
                *(unsigned char *)(arg0 + 0x23A) = *(unsigned char *)(*(int *)0x700031D0 + 0x1A);
            } else {
                *(unsigned char *)(arg0 + 0x23A) = 0;
                if (*(unsigned char *)(arg0 + 0xA) & 0x80) {
                    char *m = *(char **)(arg0 + 0x214);
                    if ((*(unsigned char *)(m + 2) & ~0xE0) == 4) {
                        unsigned char id2 = *(unsigned char *)(m + 3);
                        if (id2 == 2 || id2 == 10 || id2 == 12 || id2 == 24 || id2 == 42 || id2 == 40) {
                            *(unsigned char *)(arg0 + 0x23A) = 4;
                        }
                    }
                }
            }
        }
        *(char *)(arg0 + 0x25F) = 0;
        if (*(unsigned char *)(arg0 + 5) != 0x1C) {
            *(int *)(arg0 + 0xC0) = 0;
        }
    }

    return *(unsigned char *)(arg0 + 0xA);
}
