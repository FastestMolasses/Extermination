// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Footing update, run at the end of the player's per-frame states (FINDINGS
// "Where the surface attr comes from"). self is the actor; probe_around
// enables the ring of side probes.
//
// 1. Unless +0x1F0 is 0x30, a surface code +0x23B other than 0x35 (the
//    uphill surface of func_00178B90) is cleared together with the slope
//    +0x9C.
// 2. A collision probe at the position +0xB0 (func_0019AB20, mask 6). On a
//    hit, the position is kept as the contact point and func_00175CF0(self,
//    hit, 0, position) handles it. Otherwise, when probe_around is set, up
//    to eight points two units from the position are probed, one per yaw
//    offset in D_00248950 (added to the heading +0xC4 and wrapped): the
//    local point (0, 0, 2, 1) is turned by that yaw and moved to the
//    position in the scratch matrix D_700036A0, giving D_700038A0. The first
//    hit becomes the contact point and goes to func_00175CF0(self, hit,
//    index + 1, point).
// 3. The floor probe func_0019B6C0 from 18 units above the position down to
//    it. On a hit: +0x0B = 1, the surface attr +0x23A is the result record's
//    byte +0x1A (*0x700031D0), the floor height +0x250 is 0x700031B4, and
//    the first contact with attr 0x5A / 0x5B / 0x5C (latches +0x23D /
//    +0x23C / +0x23E) runs func_00187DC0 / func_00187DE0 / func_00187EA0;
//    for 0x5B a probe 4.01 below the floor sets +0x23C = 1 (it hits) or 2.
//    On a miss the three latches are cleared and +0x250 = the height +0xB4.
// 4. While +0x0A is set and +0x0B is clear: the probe func_0019B8C0 (mask
//    7) at the contact point supplies +0x23A, else +0x23A = 0 and, when bit
//    0x80 of +0x0A is set and the object record +0x214 has type byte & ~0xE0
//    == 4, the subtypes 2, 0xA, 0xC, 0x18, 0x2A and 0x28 give +0x23A = 4.
//    Then +0x25F = 0 and, unless +0x05 is 0x1C, the X rotation +0xC0 is zeroed.
// Returns +0x0A.
extern float func_001B1470(float);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, char *);
extern void func_00102948(void *, void *);
extern void func_001026A0(void *, void *, float *);
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

unsigned char func_00175900(char *self, int probe_around) {
    float contact[4];
    void *position;
    float local_point[4];
    int i;
    float *yaw_offset;
    int hit;
    unsigned char attr;

    if (*(unsigned char *)(self + 0x1F0) != 0x30) {
        if (*(unsigned char *)(self + 0x23B) != 0x35) {
            *(unsigned char *)(self + 0x23B) = 0;
            *(int *)(self + 0x9C) = 0;
        }
    }
    position = self + 0xB0;

    hit = func_0019AB20(self, position, self + 0x280, 6);
    if (hit != 0) {
        func_00102948(contact, self + 0xB0);
        func_00175CF0(self, hit, 0, self + 0xB0);
    } else if (probe_around != 0) {
        local_point[0] = 0.0f;
        local_point[1] = 0.0f;
        local_point[2] = 2.0f;
        local_point[3] = 1.0f;
        yaw_offset = &D_00248950;
        i = 0;
        do {
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, func_001B1470(*(float *)(self + 0xC4) + *yaw_offset));
            func_00102918(&D_700036A0, &D_700036A0, self + 0xB0);
            func_001026A0(&D_700038A0, &D_700036A0, local_point);
            hit = func_0019AB20(self, &D_700038A0, self + 0x280, 6);
            if (hit != 0) {
                func_00102948(contact, &D_700038A0);
                func_00175CF0(self, hit, i + 1, &D_700038A0);
                break;
            }
            i += 1;
            yaw_offset += 1;
        } while (i < 8);
    }

    func_001031E0(&D_700038A0, self + 0xB0);
    *(float *)0x700038A4 += 18.0f;
    if (func_0019B6C0(&D_700038A0, self + 0xB0) != 0) {
        *(unsigned char *)(self + 0xB) = 1;
        *(unsigned char *)(self + 0x23A) = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
        *(float *)(self + 0x250) = *(float *)0x700031B4;
        attr = *(unsigned char *)(self + 0x23A);
        switch (attr) {
        case 0x5A:
            if (*(unsigned char *)(self + 0x23D) == 0) {
                *(unsigned char *)(self + 0x23D) = 1;
                func_00187DC0(self);
            }
            break;
        case 0x5B:
            if (*(unsigned char *)(self + 0x23C) == 0) {
                func_001031E0(&D_700038A0, self + 0xB0);
                *(float *)0x700038A4 = *(float *)(self + 0x250) - 4.01f;
                if (func_0019AB20(self, &D_700038A0, self + 0x280, 6) != 0) {
                    *(unsigned char *)(self + 0x23C) = 1;
                } else {
                    *(unsigned char *)(self + 0x23C) = 2;
                }
                func_00187DE0(self);
            }
            break;
        case 0x5C:
            if (*(unsigned char *)(self + 0x23E) == 0) {
                *(unsigned char *)(self + 0x23E) = 1;
                func_00187EA0(self);
            }
            break;
        }
    } else {
        *(unsigned char *)(self + 0x23C) = 0;
        *(unsigned char *)(self + 0x23D) = 0;
        *(unsigned char *)(self + 0x23E) = 0;
        *(float *)(self + 0x250) = *(float *)(self + 0xB4);
    }

    if (*(unsigned char *)(self + 0xA) != 0) {
        if (*(unsigned char *)(self + 0xB) == 0) {
            if (func_0019B8C0(self, contact, self + 0x280, 7) != 0) {
                *(unsigned char *)(self + 0x23A) = *(unsigned char *)(*(int *)0x700031D0 + 0x1A);
            } else {
                *(unsigned char *)(self + 0x23A) = 0;
                if (*(unsigned char *)(self + 0xA) & 0x80) {
                    char *object = *(char **)(self + 0x214);
                    if ((*(unsigned char *)(object + 2) & ~0xE0) == 4) {
                        unsigned char subtype = *(unsigned char *)(object + 3);
                        if (subtype == 2 || subtype == 10 || subtype == 12 || subtype == 24 || subtype == 42 || subtype == 40) {
                            *(unsigned char *)(self + 0x23A) = 4;
                        }
                    }
                }
            }
        }
        *(char *)(self + 0x25F) = 0;
        if (*(unsigned char *)(self + 5) != 0x1C) {
            *(int *)(self + 0xC0) = 0;
        }
    }

    return *(unsigned char *)(self + 0xA);
}
