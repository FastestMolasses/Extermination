// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 62.91% NEARMISS).
// Fixes read from the original instructions:
//  - The scratchpad vectors passed by address (0x700031B0, 0x700038A0/B0/C0/
//    D0/E0/F0) are relocated externs (the original carries %hi/%lo on every
//    address argument); the word stores (+0xC w lanes, 0x700031E8, the hit
//    record reads, 0x70003A20/24) stay literal addresses, as in the original.
//  - The no-lock forward scale is 260.0, not 4.5 (4.5 is only the range-ratio
//    threshold).
//  - func_001B41F0 takes six arguments: both calls pass 0 and 5 after the aux
//    word (its definition in src/ has six parameters).
//  - The lock-class pick is a switch (default, case 1, case 2), read through a
//    local pointer to D_008102B0 taken at entry.
//  - The locked record (s5) is NOT initialized when the game-mode test fails:
//    the original leaves the register as the caller left it.
//  - Branch shapes were wrong: a direct actor hit with no secondary record keeps
//    reaction code 0; a locked-target hit (kind 2) whose secondary record has a
//    zero flags byte does nothing; any other hit kind always runs the
//    surface-type dispatch. The reaction code is a short.
//  - The miss path and the "no free slot" return share one return-0 tail after
//    the main body (if/else around the whole hit handling).
//  - Saved registers follow the declaration order s0..s5 with the shooter as
//    the parameter itself.
//
// Weapon-fire raycast/hit-reaction dispatcher. self is the shooter actor.
//
// Aim origin: when the game mode D_008104E0 is 0xC or 0x29 and the weapon-lock
// mode D_00810CA4 is 0 or 1, picks a lock record (D_008106E0, or by the
// player's target-class byte at D_008102B0+0x2F0: class 2 tries D_008106E8,
// class 1 tries D_008106E4, each falling back to D_008106E0); a non-NULL record
// has its position fetched into D_700038A0 by func_00183C40 and takes the
// locked path.
//
// Unlocked: the forward vector at self+0xC0 scaled by 260.0 plus the position
// at self+0xA0 becomes the aim point D_700038A0 (w=1), then rotated into
// D_700038D0. Locked: the locked position is rotated into D_700038D0,
// normalized into D_700038E0, scaled by 5.0 and added onto D_700038A0.
//
// Copies the aim point to D_700038F0, clears the result word 0x700031E8 and
// casts func_0019A570(self+0xA0, D_700038A0, 7, 0x20). On a miss it resolves
// the impact at D_700038F0 via func_001860A0 and returns 0.
//
// On a hit the impact point comes from D_700031B0, the surface normal from the
// primary hit record (*0x700031D0, +0x24..+0x2C) into D_700038E0, and its
// surface type (+0x1A) and aux word (+0x1C) are kept; func_001860A0 on the
// impact point sets the -1 sentinel in 0x700031E8 when it fails.
//  - hit kind 1 (actor): with a secondary record (*0x700031D4) whose flags
//    byte is 1 and class (+2 masked ~0xE0) is 2, applies the hit through
//    func_001B41F0 (sentinel on success); other records give code 0x100.
//  - otherwise, if the lock record is a live class-2 actor and the ratio of
//    the aim-delta length to the aim length is under 4.5, applies the hit to
//    it (when its flags byte is 1), moves the impact point to it and becomes
//    kind 1. Still not kind 1: kind 2 with a secondary record whose flags
//    byte is set takes func_001839A0's reaction code (0x101 is refined by the
//    surface type: 2..4 -> 0x201, 5 -> FX cue 0x8000002C, 8 -> 0x80000067;
//    0x300 -> FX cue 0x80000067; cues set the sentinel), stamps +0x36 = 5 and
//    copies the shooter's forward vector to +0x70; kind 2 without a record and
//    every other kind run the same surface dispatch with 0x101 as default.
//
// Finally allocates an FX slot via func_001AFA90(1) (none free -> return 0),
// stores +0xD (0 when the sentinel fired, else 1 plus the code at +0x2E),
// +3 = 3, the impact point at +0xB0..+0xB8 and func_0018ABA0 at +0x10; a
// non-kind-1 hit also copies D_700038E0 to +0xC0, kind 1 zeroes +0xCC.
// Returns 1.
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
extern int func_001860A0(void *a0, void *a1);
extern int func_0019A570(void *a0, void *a1, int a2, int a3);
extern void *func_001AFA90(int a0);
extern int func_001B41F0(void *e, void *a, void *b, int c, int d, int f);
extern void func_001EFD90(int code, void *a, void *b);

extern int D_008104E0;
extern unsigned char D_00810CA4;
extern void *D_008106E0;
extern void *D_008106E4;
extern void *D_008106E8;
extern char D_008102B0[];
extern char D_700031B0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];
extern char D_700038E0[];
extern char D_700038F0[];
extern void func_0018ABA0(void);

int func_001861C0(char *self) {
    short code;         /* hit-reaction code stored at slot+0x2E */
    int kind;           /* func_0019A570 result: 0 miss, 1 actor, 2 locked target */
    int surface;        /* primary hit record +0x1A */
    int aux;            /* primary hit record +0x1C */
    unsigned char *other; /* secondary hit record (*0x700031D4) */
    unsigned char *lock;  /* lock record; uninitialized unless the mode test passes */
    int locked;
    char *player = D_008102B0;
    char *hit;
    char *slot;

    code = 0;
    locked = 0;

    if ((D_008104E0 == 0xC || D_008104E0 == 0x29) && (D_00810CA4 == 1 || D_00810CA4 == 0)) {
        if (D_00810CA4 == 1) {
            lock = D_008106E0;
            if (lock != 0) {
                func_00183C40(lock, D_700038A0);
                locked = 1;
            }
        } else {
            switch (*(unsigned char *) (player + 0x2F0)) {
            default:
                lock = D_008106E0;
                if (lock != 0) {
                    func_00183C40(lock, D_700038A0);
                    locked = 1;
                }
                break;
            case 1:
                lock = D_008106E4;
                if (lock != 0) {
                    func_00183C40(lock, D_700038A0);
                    locked = 1;
                } else {
                    lock = D_008106E0;
                    if (lock != 0) {
                        func_00183C40(lock, D_700038A0);
                        locked = 1;
                    }
                }
                break;
            case 2:
                lock = D_008106E8;
                if (lock != 0) {
                    func_00183C40(lock, D_700038A0);
                    locked = 1;
                } else {
                    lock = D_008106E0;
                    if (lock != 0) {
                        func_00183C40(lock, D_700038A0);
                        locked = 1;
                    }
                }
                break;
            }
        }
    }

    if (!locked) {
        func_00103230(D_700038A0, self + 0xC0, 260.0f);
        func_001028B8(D_700038A0, D_700038A0, self + 0xA0);
        *(float *) 0x700038AC = 1.0f;
        func_001028D0(D_700038D0, D_700038A0, self + 0xA0);
        *(float *) 0x700038DC = 1.0f;
    } else {
        func_001028D0(D_700038D0, D_700038A0, self + 0xA0);
        *(float *) 0x700038DC = 1.0f;
        func_00102760(D_700038E0, D_700038D0);
        func_00102900(D_700038E0, D_700038E0, 5.0f);
        func_001028B8(D_700038A0, D_700038A0, D_700038E0);
        *(float *) 0x700038AC = 1.0f;
    }

    func_00102948(D_700038F0, D_700038A0);
    *(int *) 0x700031E8 = 0;
    if ((kind = func_0019A570(self + 0xA0, D_700038A0, 7, 0x20)) != 0) {

        func_001031E0(D_700038A0, D_700031B0);
        *(float *) 0x700038AC = 1.0f;
        hit = *(char **) 0x700031D0;
        other = *(unsigned char **) 0x700031D4;
        *(float *) 0x700038E0 = *(float *) (hit + 0x24);
        *(float *) 0x700038E4 = *(float *) (hit + 0x28);
        *(float *) 0x700038E8 = *(float *) (hit + 0x2C);
        *(float *) 0x700038EC = 1.0f;
        surface = *(unsigned char *) (hit + 0x1A);
        aux = *(int *) (hit + 0x1C);

        if (func_001860A0(self + 0xA0, D_700038A0) != 0) {
            *(int *) 0x700031E8 = -1;
        }

        if (kind == 1) {
            if (other != 0) {
                if (other[0] == 1 && (other[2] & ~0xE0) == 2) {
                    if (func_001B41F0(other, D_700038A0, self + 0xC0, aux, 0, 5) != 0) {
                        *(int *) 0x700031E8 = -1;
                    }
                } else {
                    code = 0x100;
                }
            }
        } else {
            if (lock != 0 && lock[0] != 0 && (lock[2] & ~0xE0) == 2) {
                func_00183C40(lock, D_700038C0);
                func_001028D0(D_700038B0, D_700038C0, self + 0xB0);
                *(float *) 0x700038BC = 1.0f;
                func_00102718(D_700038B0, D_700038D0, D_700038B0);
                *(float *) 0x70003A20 = func_0011E748(func_00102738(D_700038D0, D_700038D0));
                *(float *) 0x70003A24 = func_0011E748(func_00102738(D_700038B0, D_700038B0));
                if (*(float *) 0x70003A24 / *(float *) 0x70003A20 < 4.5f) {
                    if (lock[0] == 1) {
                        if (func_001B41F0(lock, D_700038C0, self + 0xC0, aux, 0, 5) != 0) {
                            *(int *) 0x700031E8 = -1;
                        }
                    }
                    kind = 1;
                    func_001031E0(D_700038A0, D_700038C0);
                    *(float *) 0x700038AC = 1.0f;
                }
            }

            if (kind != 1) {
                if (kind == 2) {
                    if (other != 0) {
                        if (other[0] != 0) {
                            code = func_001839A0(other);
                            if (code == 0x101) {
                                if ((unsigned int) (surface - 2) < 3) {
                                    code = 0x201;
                                } else if (surface == 5) {
                                    func_001EFD90(0x8000002C, D_700038A0, D_700038E0);
                                    *(int *) 0x700031E8 = -1;
                                } else if (surface == 8) {
                                    func_001EFD90(0x80000067, D_700038A0, D_700038E0);
                                    *(int *) 0x700031E8 = -1;
                                }
                            } else if (code == 0x300) {
                                func_001EFD90(0x80000067, D_700038A0, D_700038E0);
                                *(int *) 0x700031E8 = -1;
                            }
                            *(short *) (other + 0x36) = 5;
                            func_00102948(other + 0x70, self + 0xC0);
                        }
                    } else {
                        if ((unsigned int) (surface - 2) < 3) {
                            code = 0x201;
                        } else if (surface == 5) {
                            func_001EFD90(0x8000002C, D_700038A0, D_700038E0);
                            *(int *) 0x700031E8 = -1;
                        } else if (surface == 8) {
                            func_001EFD90(0x80000067, D_700038A0, D_700038E0);
                            *(int *) 0x700031E8 = -1;
                        } else {
                            code = 0x101;
                        }
                    }
                } else {
                    if ((unsigned int) (surface - 2) < 3) {
                        code = 0x201;
                    } else if (surface == 5) {
                        func_001EFD90(0x8000002C, D_700038A0, D_700038E0);
                        *(int *) 0x700031E8 = -1;
                    } else if (surface == 8) {
                        func_001EFD90(0x80000067, D_700038A0, D_700038E0);
                        *(int *) 0x700031E8 = -1;
                    } else {
                        code = 0x101;
                    }
                }
            }
        }

        slot = func_001AFA90(1);
        if (slot != 0) {
            if (*(int *) 0x700031E8 == -1) {
                slot[0xD] = 0;
            } else {
                slot[0xD] = 1;
                *(short *) (slot + 0x2E) = code;
            }
            slot[3] = 3;
            *(float *) (slot + 0xB0) = *(float *) 0x700038A0;
            *(float *) (slot + 0xB4) = *(float *) 0x700038A4;
            *(float *) (slot + 0xB8) = *(float *) 0x700038A8;
            *(void (**)(void)) (slot + 0x10) = func_0018ABA0;
            if (kind != 1) {
                func_00102948(slot + 0xC0, D_700038E0);
            } else {
                *(int *) (slot + 0xCC) = 0;
            }
            return 1;
        }
    } else {
        func_001860A0(self + 0xA0, D_700038F0);
    }
    return 0;
}
