// NEARMISS func_001861C0  (vram 0x001861C0, 0x7D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 62.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// large-function (7 saved regs, ~500 instructions, dense 0x700038xx GS/VU0 scratch traffic) register-allocation/scheduling divergence -- structurally and semantically correct (verified branch-by-branch against the .s, incl. the subtle func_001839A0 reaction-code retention that a naive read would mi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Weapon-fire raycast/hit-reaction dispatcher. arg0 is the shooter actor.
//
// First decides which "aim origin" record (var_s5) to use: normally none
// (var_a1=0, meaning "use the shooter's own forward vector" path below), but
// if the game mode is 0xC or 0x29 (D_008104E0) AND the weapon-lock mode
// D_00810CA4 is 0 or 1, it picks one of three lock targets (D_008106E0/E4/E8,
// keyed by D_00810CA4 and the actor's target-class byte at +0x2F0) and if a
// non-NULL record is found, calls func_00183C40(record, &D_700038A0) to fetch
// its world position and takes the "locked" path (var_a1=1).
//
// var_a1==0 (no lock): builds arg0+0xC0's forward vector scaled by 4.5 into
// D_700038A0..AC (func_00103230/func_001028B8), then rotates it by the
// shooter's world matrix (func_001028D0) into D_700038D0.. with w=1.0.
// var_a1==1 (locked): rotates the locked position straight into D_700038D0..,
// normalizes it (func_00102760) and scales by 5.0 (func_00102900), then adds
// the shooter's forward-vector offset back in.
//
// Either way ends up with a world-space aim point in D_700038A0.. and fires a
// raycast query (func_0019A570(arg0+0xA0, &D_700038A0, 7, 0x20)) to find the
// first thing hit; on a miss (var_s1==0) it instead just resolves any
// existing surface impact via func_001860A0(arg0+0xA0, &D_700038F0) and
// returns 0.
//
// On a hit it always calls func_001860A0(arg0+0xA0, &D_700038A0) (surface FX
// for the impact point) and folds a -1 "no valid target" sentinel into the
// local hit-reaction code (var_s0) if that fails. It then classifies the hit
// record (*(void**)0x700031D0 = the primary hit, *(void**)0x700031D4 = a
// secondary/actor hit) by var_s1 (1 = actor hit, 2 = locked-target hit,
// other = plain world hit):
//  - var_s1==1 (direct actor hit): if the secondary hit record's flags byte
//    (+0) == 1 and its class (+2, masked ~0xE0) == 2, applies knockback via
//    func_001B41F0(actor, &D_700038A0, arg0+0xC0, aux); on failure sets the
//    -1 sentinel. Else the hit-reaction code is fixed at 0x100.
//  - var_s1!=1: if the LOCKED record (var_s5) is a live actor (class ~0xE0
//    == 2) close enough (normalized aim-delta magnitude ratio < 4.5), applies
//    knockback to it too and promotes var_s1 to 1 (re-fetches the FX vector
//    from it). Otherwise (or if var_s1==2), the hit-reaction code (var_s0)
//    starts from func_001839A0(secondary-hit)&0xFFFF (or, if there is no
//    secondary-hit record / its flags byte is 0, defaults to 0x101), then a
//    shared surface-type dispatch on s2 (the primary hit's +0x1A byte) can
//    promote it to 0x201 (s2 in [2,4]) or fire an func_001EFD90 FX cue
//    (0x8000002C for s2==5, 0x80000067 for s2==8 or for an initial 0x300
//    code) and set the -1 sentinel; a secondary-hit record additionally gets
//    its hit-reaction timer (+0x36) stamped to 5 via
//    func_00102948(actor+0x70, arg0+0xC0).
//
// Finally allocates an FX/decal slot via func_001AFA90(1); if none is free,
// returns 0. Otherwise stores whether the sentinel fired (+0xD = 0 or 1),
// the hit-reaction code (+0x2E), fixed state (+3=3), the impact point
// (+0xB0/+0xB4/+0xB8 from D_700038A0..A8) and the FX vtable pointer
// (+0x10 = &func_0018ABA0); if this was NOT the var_s1==1 actor-hit path, it
// also stamps a secondary FX vector (func_00102948(slot+0xC0, &D_700038E0));
// otherwise it just zeroes slot+0xCC. Returns 1.
extern void func_00102718(void *dst, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102900(void *dst, void *src, float s);
extern void func_00102948(void *dst, void *src);
extern void func_00103230(void *dst, void *src, float s);
extern void func_001031E0(void *dst, void *src);
extern float func_0011E748(float x);
extern short func_001839A0(void *a0);
extern void func_00183C40(void *p, void *out);
extern int func_001860A0(int a0, void *a1);
extern int func_0019A570(void *a0, void *a1, int a2, int a3);
extern void *func_001AFA90(int a0);
extern int func_001B41F0(void *e, void *a, void *b, int c);
extern void func_001EFD90(int code, void *a, void *b);

extern int D_008104E0;
extern unsigned char D_00810CA4;
extern void *D_008106E0;
extern void *D_008106E4;
extern void *D_008106E8;
extern char D_008102B0[];
extern void func_0018ABA0(void);

int func_001861C0(int arg0) {
    char *s6 = (char *) arg0;
    void *s5;
    int locked; /* var_a1: 0 = no lock target, 1 = locked target used */
    int s1;     /* hit-kind: 0 = miss, 1 = actor hit, 2 = locked-target hit, other = world hit */
    int s0;     /* hit-reaction code, high 16 bits of func_001839A0's result classification */
    void *s4;   /* secondary/actor hit record (*(void**)0x700031D4) */
    int s2;     /* surface-type byte (+0x1A) of the secondary hit record */
    int s3;     /* aux field (+0x1C) of the secondary hit record */
    void *hit;  /* primary hit record (*(void**)0x700031D0) */
    void *slot;
    unsigned char lockClass;

    s5 = 0;
    s0 = 0;
    locked = 0;

    if ((D_008104E0 == 0xC || D_008104E0 == 0x29) && (D_00810CA4 == 1 || D_00810CA4 == 0)) {
        if (D_00810CA4 == 1) {
            s5 = D_008106E0;
            if (s5 != 0) {
                func_00183C40(s5, (void *) 0x700038A0);
                locked = 1;
            }
        } else {
            lockClass = *(unsigned char *) (D_008102B0 + 0x2F0);
            if (lockClass == 2) {
                s5 = D_008106E8;
                if (s5 != 0) {
                    func_00183C40(s5, (void *) 0x700038A0);
                    locked = 1;
                } else {
                    s5 = D_008106E0;
                    if (s5 != 0) {
                        func_00183C40(s5, (void *) 0x700038A0);
                        locked = 1;
                    }
                }
            } else if (lockClass == 1) {
                s5 = D_008106E4;
                if (s5 != 0) {
                    func_00183C40(s5, (void *) 0x700038A0);
                    locked = 1;
                } else {
                    s5 = D_008106E0;
                    if (s5 != 0) {
                        func_00183C40(s5, (void *) 0x700038A0);
                        locked = 1;
                    }
                }
            } else {
                s5 = D_008106E0;
                if (s5 != 0) {
                    func_00183C40(s5, (void *) 0x700038A0);
                    locked = 1;
                }
            }
        }
    }

    if (!locked) {
        func_00103230((void *) 0x700038A0, s6 + 0xC0, 4.5f);
        func_001028B8((void *) 0x700038A0, (void *) 0x700038A0, s6 + 0xA0);
        *(int *) 0x700038AC = 0x3F800000;
        func_001028D0((void *) 0x700038D0, (void *) 0x700038A0, s6 + 0xA0);
        *(int *) 0x700038DC = 0x3F800000;
    } else {
        func_001028D0((void *) 0x700038D0, (void *) 0x700038A0, s6 + 0xA0);
        *(int *) 0x700038DC = 0x3F800000;
        func_00102760((void *) 0x700038E0, (void *) 0x700038D0);
        func_00102900((void *) 0x700038E0, (void *) 0x700038E0, 5.0f);
        func_001028B8((void *) 0x700038A0, (void *) 0x700038A0, (void *) 0x700038E0);
        *(int *) 0x700038AC = 0x3F800000;
    }

    func_00102948((void *) 0x700038F0, (void *) 0x700038A0);
    *(int *) 0x700031E8 = 0;
    s1 = func_0019A570(s6 + 0xA0, (void *) 0x700038A0, 7, 0x20);

    if (s1 == 0) {
        func_001860A0((int) (s6 + 0xA0), (void *) 0x700038F0);
        return 0;
    }

    func_001031E0((void *) 0x700038A0, (void *) 0x700031B0);
    *(int *) 0x700038AC = 0x3F800000;

    hit = *(void **) 0x700031D0;
    s4 = *(void **) 0x700031D4;
    *(float *) 0x700038E0 = *(float *) ((char *) hit + 0x24);
    *(float *) 0x700038E4 = *(float *) ((char *) hit + 0x28);
    *(float *) 0x700038E8 = *(float *) ((char *) hit + 0x2C);
    *(int *) 0x700038EC = 0x3F800000;
    s2 = *(unsigned char *) ((char *) hit + 0x1A);
    s3 = *(int *) ((char *) hit + 0x1C);

    if (func_001860A0((int) (s6 + 0xA0), (void *) 0x700038A0) != 0) {
        *(int *) 0x700031E8 = -1;
    }

    if (s1 == 1) {
        if (s4 != 0 && *(unsigned char *) s4 == 1 && (*(unsigned char *) ((char *) s4 + 2) & ~0xE0) == 2) {
            if (func_001B41F0(s4, (void *) 0x700038A0, s6 + 0xC0, s3) != 0) {
                *(int *) 0x700031E8 = -1;
            }
        } else {
            s0 = 0x100;
        }
    } else {
        if (s5 != 0 && *(unsigned char *) s5 != 0 && (*(unsigned char *) ((char *) s5 + 2) & ~0xE0) == 2) {
            float mag0, mag1;
            func_00183C40(s5, (void *) 0x700038C0);
            func_001028D0((void *) 0x700038B0, (void *) 0x700038C0, s6 + 0xB0);
            *(int *) 0x700038BC = 0x3F800000;
            func_00102718((void *) 0x700038B0, (void *) 0x700038D0, (void *) 0x700038B0);
            mag0 = func_0011E748(func_00102738((void *) 0x700038D0, (void *) 0x700038D0));
            *(float *) 0x70003A20 = mag0;
            mag1 = func_0011E748(func_00102738((void *) 0x700038B0, (void *) 0x700038B0));
            *(float *) 0x70003A24 = mag1;
            if ((mag1 / mag0) < 4.5f) {
                if (*(unsigned char *) s5 == 1 && func_001B41F0(s5, (void *) 0x700038C0, s6 + 0xC0, s3) != 0) {
                    *(int *) 0x700031E8 = -1;
                }
                s1 = 1;
                func_001031E0((void *) 0x700038A0, (void *) 0x700038C0);
                *(int *) 0x700038AC = 0x3F800000;
            }
        }

        if (s1 != 1) {
            if (s1 == 2 && s4 != 0 && *(unsigned char *) s4 != 0) {
                /* actor secondary-hit reaction path (uses func_001839A0's
                   decoded state to pick the hit-reaction code). s0 stays at
                   the decoded reaction value unless overridden below; the
                   0x36 stamp is an unconditional side effect, unrelated to
                   the s0/reaction value. */
                s0 = (short) (func_001839A0(s4) & 0xFFFF);
                if (s0 == 0x101) {
                    if ((unsigned int) (s2 - 2) < 3U) {
                        s0 = 0x201;
                    } else if (s2 == 5) {
                        func_001EFD90(0x8000002C, (void *) 0x700038A0, (void *) 0x700038E0);
                        *(int *) 0x700031E8 = -1;
                    } else if (s2 == 8) {
                        func_001EFD90(0x80000067, (void *) 0x700038A0, (void *) 0x700038E0);
                        *(int *) 0x700031E8 = -1;
                    }
                    /* else: s0 stays 0x101 */
                } else if (s0 == 0x300) {
                    func_001EFD90(0x80000067, (void *) 0x700038A0, (void *) 0x700038E0);
                    *(int *) 0x700031E8 = -1;
                    /* s0 stays 0x300 */
                }
                /* else: s0 stays whatever func_001839A0 returned */
                *(short *) ((char *) s4 + 0x36) = 5;
                func_00102948((char *) s4 + 0x70, s6 + 0xC0);
            } else if (s4 == 0) {
                /* no secondary-hit record at all: pick the hit-reaction code
                   purely off the surface-type byte s2. */
                if ((unsigned int) (s2 - 2) < 3U) {
                    s0 = 0x201;
                } else if (s2 == 5) {
                    func_001EFD90(0x8000002C, (void *) 0x700038A0, (void *) 0x700038E0);
                    *(int *) 0x700031E8 = -1;
                } else if (s2 == 8) {
                    func_001EFD90(0x80000067, (void *) 0x700038A0, (void *) 0x700038E0);
                    *(int *) 0x700031E8 = -1;
                } else {
                    s0 = 0x101;
                }
            } else {
                /* secondary-hit record present but its flags byte (+0) is 0:
                   same surface-type dispatch as the no-record case. */
                if ((unsigned int) (s2 - 2) < 3U) {
                    s0 = 0x201;
                } else if (s2 == 5) {
                    func_001EFD90(0x8000002C, (void *) 0x700038A0, (void *) 0x700038E0);
                    *(int *) 0x700031E8 = -1;
                } else if (s2 == 8) {
                    func_001EFD90(0x80000067, (void *) 0x700038A0, (void *) 0x700038E0);
                    *(int *) 0x700031E8 = -1;
                } else {
                    s0 = 0x101;
                }
            }
        }
    }

    slot = func_001AFA90(1);
    if (slot == 0) {
        return 0;
    }
    if (*(int *) 0x700031E8 == -1) {
        *(unsigned char *) ((char *) slot + 0xD) = 0;
    } else {
        *(unsigned char *) ((char *) slot + 0xD) = 1;
        *(short *) ((char *) slot + 0x2E) = (short) s0;
    }
    *(unsigned char *) ((char *) slot + 3) = 3;
    *(float *) ((char *) slot + 0xB0) = *(float *) 0x700038A0;
    *(float *) ((char *) slot + 0xB4) = *(float *) 0x700038A4;
    *(float *) ((char *) slot + 0xB8) = *(float *) 0x700038A8;
    *(void (**)(void)) ((char *) slot + 0x10) = func_0018ABA0;
    if (s1 != 1) {
        func_00102948((char *) slot + 0xC0, (void *) 0x700038E0);
    } else {
        *(int *) ((char *) slot + 0xCC) = 0;
    }
    return 1;
}
