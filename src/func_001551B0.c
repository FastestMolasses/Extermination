// NEARMISS func_001551B0  (vram 0x001551B0, 0x1464 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.92% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Enemy "leap / pounce" AI state machine, dispatched on the state byte at arg0[4]
// (source case order 0, 4, 1, 2, 3/default -> mwcc tests them in reverse: 3,2,1,4,0).
// arg0 is the enemy actor; arg0[3] is its type id (6 / 0x1C / 0x1E / 0x1F / 0x50 are the
// leaping types), arg0[0x4C] is the per-frame virtual hook called from every tail,
// arg0+0xD0 is the world matrix, arg0+0xB0/B4/B8 the position, arg0+0x100.. the render pos,
// arg0+0x1F0 a scratch sub-object (+0x40 rotation matrix, +0x48/+0x4C stagger counters,
// +0xC8/+0xCC/+0xD0 euler deltas -> arg0+0x2B8/2BC/2C0).
// 0x700038A0/0x700038B0 are the shared scratch vectors used for the world collision probe
// func_0019AB20(self, from, to, mask); 0x700031D4 is its "hit is solid" result flag.
//
// state 0  (spawn): func_001B0FD0 gate, then seeds state 4, resets the flags, snapshots the
//          matrix into +0x1F0, and precomputes the eight probe corner offsets
//          (arg0+0x2D0..0x2EC) from a per-type radius (4.5961943 for 6/0x1E, 2.1213202 for
//          0x1C/0x50/0x1F) rotated by the actor matrix. Then walks the spawn-trigger list for
//          this room (D_0024D820[D_00810700] indexed via the D_0024A850 base table, 0x2C-byte
//          records terminated by -1); if any entry is still un-triggered it arms +0x238/+0x23C,
//          otherwise it clears bit 0 of the flags at +0xE. Finally a downward probe sets the
//          "grounded" word at +0x52.
// state 4  (idle/stagger): on +0x36 it flips the whole actor list (D_00275BC0 chain, +0x1C =
//          next) of leaping types into "alerted" (+0xA = 1) and drops to state 2; a pending
//          +0xA promotes to state 1 with a 6-frame timer. While bit 0 of +0xE is set it plays a
//          canned stagger from the D_002468B0/B4/B8 table triple (0x30-byte rows, 0xC-byte
//          columns) driven by the +0x238 counter, re-rolling the counter (and the row) from
//          func_00122BB8 when it runs out.
// state 1  (leap): sub-state arg0[5]. Sub-state 0 fires four corner probes; each solid hit
//          bumps a counter and sets one of the four quadrant flags. 3+ hits, or two opposite
//          hits, aborts back to state 4. A center probe does the same. Otherwise it builds the
//          leap rotation (pi/60 tilt toward the free quadrant, or a small random euler when no
//          side is blocked), sets the 0x1E-frame arc + the flight timer (180, or 60*y/12 for
//          type 6) and advances to sub-state 1. Sub-state 1 integrates the arc for its first
//          frames, then switches to ballistic motion (velocity at +0x2CC/2C8/2C4, gravity
//          0.052/frame) and probes ahead each frame; a wall hit or an expired timer lands the
//          actor back in state 2.
// state 2  (land/recover): sub-state arg0[7]. Sub-state 0 spawns the room's trigger objects
//          (func_001AFA90 per record, copying type/flags/pos/rot from the 0x2C-byte record),
//          plays the per-type landing sound/effect pair, and for types 6/0x1E does a random
//          quarter-turn re-orient (func_00122BB8 -> 0..3 * pi/2) plus a model swap
//          (func_001C6120(D_0028A56C, 0x22 / 0x29)) before rebuilding the bones; otherwise it
//          drops straight to state 3. Sub-state 1 re-probes the four corners while the recover
//          timer runs and returns to state 3 once fewer than three corners are blocked.
// state 3 / default: death/despawn - type 0x50 notifies its owner (func_001B1190) first, then
//          func_001AFC10 frees the actor.

/* the 0x1F0 scratch sub-object: +0x40 is its rotation matrix. */
typedef struct { char pad[0x40]; char mtx[0x40]; } SubObj;

extern int func_001B0FD0();
extern void func_001B1B70(void *self);
extern void func_001C6380(void *self);
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *src, void *out);
extern int func_001B11E0(int id);
extern int func_0019AB20(void *self, void *a, void *b, int mode);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102A60(void *dst, void *src, float ang);
extern void func_00102B08(void *dst, void *src, float ang);
extern void func_00102BB0(void *dst, void *src, float ang);
extern void func_00102C58(void *dst, void *src, void *v);
extern void func_001026D0(void *dst, void *a, void *b);
extern int func_00122BB8(void);
extern void func_001FBD50(void *self, int a, int b, float f);
extern int float_to_int(float f);
extern void func_001FC580(void *self, int id);
extern void func_001EFD90(int code, void *pos, void *rot);
extern char *func_001AFA90(int kind);
extern void func_001AFC10(void *self);
extern void func_001B1190();
extern int func_001C6120(int bank, int idx);
extern void func_001CA6E0(void *self, int model);
extern void bone_init_default_1(void *self);

extern int D_00275B40;
extern char *D_00275BC0;

extern short D_0024A850[];
extern int D_0024D820[];
extern char D_002468B0[];
extern char D_002468B4[];
extern char D_002468B8[];
extern int D_0028A56C[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];

extern int D_700036E0[];
extern int D_700038A0[];
extern int D_700038B0[];

void func_001551B0(char *arg0, int arg1, int arg2)
{
    unsigned char st = *(unsigned char *)(arg0 + 4);

    switch (st) {
    case 0: {
        int n;
        short idx;
        unsigned char kind;

        if (func_001B0FD0(arg0, st) != 0) {
            break;
        }
        *(unsigned char *)(arg0 + 4) = 4;
        *(unsigned char *)(arg0 + 0xA) = 0;
        *(char *)(arg0 + 0) = 1;
        *(short *)(arg0 + 0x34) = 1;
        func_001B1B70(arg0);
        func_001C6380(arg0);
        copy_qw4(arg0 + 0x1F0, arg0 + 0xD0);
        *(int *)(arg0 + 0x2C8) = 0;
        kind = *(unsigned char *)(arg0 + 3);
        switch (kind) {
        case 6:
        case 0x1E:
            *(int *)0x700038A8 = 0x40931406;
            *(int *)0x700038A0 = 0x40931406;
            break;
        case 0x1C:
        case 0x50:
        case 0x1F:
            *(int *)0x700038A8 = 0x4007C3B6;
            *(int *)0x700038A0 = 0x4007C3B6;
            break;
        default:
            *(int *)0x700038A8 = 0x40931406;
            *(int *)0x700038A0 = 0x40931406;
            break;
        }
        *(int *)0x700038AC = 0;
        *(int *)0x700038A4 = 0;
        func_001026A0(D_700038A0, arg0 + 0xD0, D_700038A0);
        *(float *)(arg0 + 0x2EC) = *(float *)(arg0 + 0xB0) + *(float *)0x700038A0;
        *(float *)(arg0 + 0x2DC) = *(float *)(arg0 + 0xB8) + *(float *)0x700038A8;
        *(float *)(arg0 + 0x2E8) = *(float *)(arg0 + 0xB0) - *(float *)0x700038A8;
        *(float *)(arg0 + 0x2D8) = *(float *)(arg0 + 0xB8) + *(float *)0x700038A0;
        *(float *)(arg0 + 0x2E4) = *(float *)(arg0 + 0xB0) - *(float *)0x700038A0;
        *(float *)(arg0 + 0x2D4) = *(float *)(arg0 + 0xB8) - *(float *)0x700038A8;
        *(float *)(arg0 + 0x2E0) = *(float *)(arg0 + 0xB0) + *(float *)0x700038A8;
        *(float *)(arg0 + 0x2D0) = *(float *)(arg0 + 0xB8) - *(float *)0x700038A0;
        if (*(unsigned short *)(arg0 + 0xE) & 1) {
            idx = *(short *)(arg0 + 0x56);
            if (idx >= 0) {
                char *p;
                int t = D_0024A850[D_00810700[0]];
                if (t == 0) {
                    t = t + 1;
                }
                t = t + idx;
                p = *(char **)(D_0024D820[D_00810700[0]] + t * 4);
                n = 0;
                while (*(short *)p != -1) {
                    if (func_001B11E0(*(unsigned char *)(p + 2)) == 0) {
                        n += 1;
                    }
                    p += 0x2C;
                }
            }
            if (n != 0) {
                *(int *)(arg0 + 0x238) = -1;
                *(int *)(arg0 + 0x23C) = 0;
            } else {
                *(unsigned short *)(arg0 + 0xE) = *(unsigned short *)(arg0 + 0xE) & 0xFFFE;
            }
        }
        *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
        *(float *)0x700038A4 = *(float *)(arg0 + 0xB4) - 2.0f;
        *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
        *(int *)0x700038B4 = 0xC0400000;
        if (func_0019AB20(arg0, D_700038A0, D_700038B0, 7) == 4) {
            *(unsigned short *)(arg0 + 0x52) = 0;
        } else {
            *(unsigned short *)(arg0 + 0x52) = 1;
        }
        break;
    }
    case 4: {
        if (*(short *)(arg0 + 0x36) != 0) {
            char *p;
            *(char *)(arg0 + 0) = 2;
            *(unsigned char *)(arg0 + 4) = 2;
            p = D_00275BC0;
            if (p != 0) {
                do {
                    if (*(unsigned short *)(p + 0x52) != 0) {
                        unsigned char k = *(unsigned char *)(p + 3);
                        if (k == 6 || k == 0x1C || k == 0x1E || k == 0x1F || k == 0x50) {
                            *(char *)(p + 0xA) = 1;
                        }
                    }
                    p = *(char **)(p + 0x1C);
                } while (p != 0);
            }
            *(unsigned char *)(arg0 + 0xA) = 0;
        }
        if (*(unsigned char *)(arg0 + 0xA) != 0) {
            *(unsigned char *)(arg0 + 0xA) = 0;
            *(unsigned char *)(arg0 + 4) = 1;
            *(short *)(arg0 + 0x2A) = 6;
        }
        if (*(unsigned short *)(arg0 + 0xE) & 1) {
            int d = *(int *)(arg0 + 0x238);
            char *sub = arg0 + 0x1F0;
            int *pn = (int *)(sub + 0x48);
            if (d >= 0) {
                if (d < 9 && (d & 1)) {
                    int *pm = (int *)(sub + 0x4C);
                    int c = *(int *)(arg0 + 0x23C);
                    func_00102BB0(arg0 + 0xD0, sub,
                                  *(float *)(D_002468B0 + c * 0x30 + ((d - 1) >> 1) * 0xC));
                    *(float *)(arg0 + 0x100) = *(float *)(arg0 + 0x220) +
                        *(float *)(D_002468B4 + *pm * 0x30 + ((*pn - 1) >> 1) * 0xC);
                    *(float *)(arg0 + 0x108) = *(float *)(arg0 + 0x228) +
                        *(float *)(D_002468B8 + *pm * 0x30 + ((*pn - 1) >> 1) * 0xC);
                }
                if (*pn == 0) {
                    copy_qw4(arg0 + 0xD0, ((SubObj *)(arg0 + 0x1F0))->pad);
                }
                *pn = *pn - 1;
            } else {
                func_001FBD50(arg0, 0x19C, 0, 300.0f);
                *pn = ((func_00122BB8() >> 16) << 8) >> 15;
                { int x = func_00122BB8() >> 16; x *= 7; x >>= 15;
                  *(int *)(arg0 + 0x23C) = x; }
            }
        }
        copy_qw4((char *)*(int *)D_00275B40 + 0x90, arg0 + 0xD0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        func_001B1B70(arg0);
        return;
    }
    case 1: {
        switch (*(unsigned char *)(arg0 + 5)) {
        case 0: {
            unsigned int cnt = 0;
            int c4 = 0, c3 = 0, c2 = 0, c1 = 0;
            char *mtx;
            float *pD, *pC, *pB;

            *(short *)(arg0 + 0x2A) = *(short *)(arg0 + 0x2A) - 1;
            func_00102948(D_700038A0, arg0 + 0xB0);
            {
                float a4 = *(float *)0x700038A4;
                *(int *)0x700038B4 = 0xC0000000;
                *(float *)0x700038A4 = a4 - 1.0f;
            }
            *(float *)0x700038A0 = *(float *)(arg0 + 0x2EC);
            *(float *)0x700038A8 = *(float *)(arg0 + 0x2DC);
            if (func_0019AB20(arg0, D_700038A0, D_700038B0, 7) == 2 && *(int *)0x700031D4 != 0) {
                cnt += 1;
                c1 = 1;
            }
            *(float *)0x700038A0 = *(float *)(arg0 + 0x2E8);
            *(float *)0x700038A8 = *(float *)(arg0 + 0x2D8);
            if (func_0019AB20(arg0, D_700038A0, D_700038B0, 7) == 2 && *(int *)0x700031D4 != 0) {
                cnt += 1;
                c2 = 1;
            }
            *(float *)0x700038A0 = *(float *)(arg0 + 0x2E4);
            *(float *)0x700038A8 = *(float *)(arg0 + 0x2D4);
            if (func_0019AB20(arg0, D_700038A0, D_700038B0, 7) == 2 && *(int *)0x700031D4 != 0) {
                cnt += 1;
                c3 = 1;
            }
            *(float *)0x700038A0 = *(float *)(arg0 + 0x2E0);
            *(float *)0x700038A8 = *(float *)(arg0 + 0x2D0);
            if (func_0019AB20(arg0, D_700038A0, D_700038B0, 7) == 2 && *(int *)0x700031D4 != 0) {
                cnt += 1;
                c4 = 1;
            }
            if (cnt >= 3U || (c1 && c3) || (c2 && c4)) {
                if (*(short *)(arg0 + 0x2A) == 0) {
                    *(unsigned char *)(arg0 + 4) = 4;
                }
                func_001B1B70(arg0);
                break;
            }
            func_00102948(D_700038A0, arg0 + 0xB0);
            {
                float a4b = *(float *)0x700038A4;
                *(int *)0x700038B4 = 0xC0400000;
                *(float *)0x700038A4 = a4b - 2.0f;
            }
            if (func_0019AB20(arg0, D_700038A0, D_700038B0, 7) == 2) {
                if (*(int *)0x700031D4 != 0) {
                    if (*(short *)(arg0 + 0x2A) == 0) {
                        *(unsigned char *)(arg0 + 4) = 4;
                    }
                    func_001B1B70(arg0);
                    break;
                }
            }
            mtx = ((SubObj *)(arg0 + 0x1F0))->mtx;
            func_001029C0(mtx);
            *(short *)(arg0 + 0x28) = 0x1E;
            {
                char *subB = arg0 + 0x1F0;
                pC = (float *)(subB + 0xCC);
                *(int *)(arg0 + 0x2B8) = 0;
                *(int *)(arg0 + 0x2BC) = 0;
                pB = (float *)(subB + 0xC8);
                pD = (float *)(subB + 0xD0);
                *(int *)(arg0 + 0x2C0) = 0;
            }
            if (cnt == 2) {
                if (c1 && c2) {
                    func_00102B08(mtx, mtx, -0.052359875f);
                    *(int *)pD = 0xBD56774F;
                } else if (c2 && c3) {
                    func_00102A60(mtx, mtx, -0.052359875f);
                    *(int *)pB = 0xBD56774F;
                } else if (c3 && c4) {
                    func_00102B08(mtx, mtx, 0.052359875f);
                    *(int *)pD = 0x3D56774F;
                } else if (c4 && c1) {
                    func_00102A60(mtx, mtx, 0.052359875f);
                    *(int *)pB = 0x3D56774F;
                }
            } else if (cnt == 1) {
                if (c1) {
                    func_00102B08(mtx, mtx, -0.052359875f);
                    *(int *)pD = 0xBD56774F;
                } else if (c2) {
                    func_00102A60(mtx, mtx, -0.052359875f);
                    *(int *)pB = 0xBD56774F;
                } else if (c3) {
                    func_00102B08(mtx, mtx, 0.052359875f);
                    *(int *)pD = 0x3D56774F;
                } else if (c4) {
                    func_00102A60(mtx, mtx, 0.052359875f);
                    *(int *)pB = 0x3D56774F;
                }
            } else {
                *(short *)(arg0 + 0x28) = 0;
                *pD += (((float)func_00122BB8() / 2147483648.0f) - 0.5f) / 60.0f;
                *pC += (((float)func_00122BB8() / 2147483648.0f) - 0.5f) / 50.0f;
                *pB += (((float)func_00122BB8() / 2147483648.0f) - 0.5f) / 60.0f;
                *(float *)0x700038A0 = *pD;
                *(float *)0x700038A4 = *pC;
                *(float *)0x700038A8 = *pB;
                func_00102C58(mtx, mtx, D_700038A0);
            }
            if (*(unsigned char *)(arg0 + 3) == 6) {
                *(short *)(arg0 + 0x2A) = float_to_int((60.0f * *(float *)(arg0 + 0xB4)) / 12.0f);
            } else {
                *(short *)(arg0 + 0x2A) = 0xB4;
            }
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        }
            /* fallthrough */
        case 1: {
            *(short *)(arg0 + 0x2A) = *(short *)(arg0 + 0x2A) - 1;
            if (*(short *)(arg0 + 0x28) < 2) goto motion;
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x2C8) + *(float *)(arg0 + 0xB4);
            if (*(short *)(arg0 + 0x28) < 2) {
                char *m2 = ((SubObj *)(arg0 + 0x1F0))->mtx;
                func_001029C0(m2);
                *(float *)(arg0 + 0x2CC) = 11.0f * -*(float *)(arg0 + 0x2B8);
                *(float *)(arg0 + 0x2C4) = 11.0f * *(float *)(arg0 + 0x2C0);
                *(float *)(arg0 + 0x2C0) = *(float *)(arg0 + 0x2C0) / 1.4f;
                *(float *)(arg0 + 0x2B8) = *(float *)(arg0 + 0x2B8) / 1.4f;
                *(float *)0x700038A0 = *(float *)(arg0 + 0x2C0);
                *(float *)0x700038A4 = *(float *)(arg0 + 0x2BC);
                *(float *)0x700038A8 = *(float *)(arg0 + 0x2B8);
                func_00102C58(m2, m2, D_700038A0);
            }
            goto ljoin;
        motion:
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x2CC) + *(float *)(arg0 + 0xB0);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x2C8) + *(float *)(arg0 + 0xB4);
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x2C4) + *(float *)(arg0 + 0xB8);
            *(float *)0x700038B4 = *(float *)(arg0 + 0x2C8);
            *(float *)(arg0 + 0x2C8) = *(float *)(arg0 + 0x2C8) - 0.052f;
            func_00102948(D_700038A0, arg0 + 0xB0);
            if (*(short *)(arg0 + 0x28) != 0) {
                if (*(unsigned char *)(arg0 + 3) == 6) {
                    *(float *)0x700038A4 = *(float *)0x700038A4 - 9.0f;
                } else {
                    *(float *)0x700038A4 = *(float *)0x700038A4 - 4.5f;
                }
            }
            if (func_0019AB20(arg0, D_700038A0, D_700038B0, 7) == 4 ||
                *(short *)(arg0 + 0x2A) < 0) {
                *(short *)(arg0 + 0x36) = 0;
                *(short *)(arg0 + 0x2A) = 0;
                *(unsigned char *)(arg0 + 4) = 2;
            }
        ljoin:
            {
                func_001026D0(arg0 + 0xD0, arg0 + 0xD0,
                              ((SubObj *)(arg0 + 0x1F0))->mtx);
            }
            *(float *)(arg0 + 0x100) = *(float *)(arg0 + 0xB0);
            *(float *)(arg0 + 0x104) = *(float *)(arg0 + 0xB4);
            *(float *)(arg0 + 0x108) = *(float *)(arg0 + 0xB8);
            break;
        }
        default:
            break;
        }
        copy_qw4((char *)*(int *)D_00275B40 + 0x90, arg0 + 0xD0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    }
    case 2: {
        unsigned char sub = *(unsigned char *)(arg0 + 7);
        switch (sub) {
        case 0: {
            short idx2;
            *(unsigned char *)(arg0 + 7) = sub + 1;
            *(int *)(arg0 + 0xC4) = 0;
            *(int *)(arg0 + 0xC0) = 0;
            *(short *)(arg0 + 0x2A) = 6;
            idx2 = *(short *)(arg0 + 0x56);
            if (idx2 >= 0) {
                char *q;
                int t2 = D_0024A850[D_00810700[0]];
                if (t2 == 0) {
                    t2 = t2 + 1;
                }
                t2 = t2 + idx2;
                q = *(char **)(D_0024D820[D_00810700[0]] + t2 * 4);
                while (*(short *)q != -1) {
                    if (func_001B11E0(*(unsigned char *)(q + 2)) == 0) {
                        char *e = func_001AFA90(*(unsigned char *)(q + 4));
                        if (e != 0) {
                            *(unsigned char *)(e + 0x9A) = *(unsigned char *)(q + 2);
                            *(unsigned char *)(e + 3) = *(unsigned char *)(q + 6);
                            *(short *)(e + 0x2E) = (*(short *)(q + 6) >> 8) & 0xFF;
                            *(unsigned char *)(e + 0xD) = *(unsigned char *)(q + 8);
                            if ((*(short *)(q + 4) & ~0xE0) == 2) {
                                *(unsigned char *)(e + 0x9D) = D_00810701[0];
                                *(unsigned char *)(e + 0x9E) = *(unsigned char *)(q + 0xA);
                            } else {
                                *(unsigned short *)(e + 0xE) = *(unsigned short *)(q + 0xA);
                            }
                            *(short *)(e + 0x54) = *(short *)(q + 0xC);
                            *(short *)(e + 0x56) = *(short *)(q + 0xE);
                            *(float *)(e + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)(q + 0x10);
                            *(float *)(e + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)(q + 0x14);
                            *(float *)(e + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)(q + 0x18);
                            *(float *)(e + 0xC0) = *(float *)(q + 0x1C);
                            *(float *)(e + 0xC4) = *(float *)(q + 0x20);
                            *(float *)(e + 0xC8) = *(float *)(q + 0x24);
                            *(int *)(e + 0x10) = *(int *)(q + 0x28);
                        }
                    }
                    q += 0x2C;
                }
            }
            {
                unsigned char kk = *(unsigned char *)(arg0 + 3);
                if (kk == 6) {
                    func_001FC580(arg0, 0x19D);
                    func_001EFD90(0x8000000A, arg0 + 0xB0, arg0 + 0xC0);
                    func_001EFD90(0x80000015, arg0 + 0xB0, arg0 + 0xC0);
                } else if (kk == 0x1C || kk == 0x50) {
                    func_001FC580(arg0, 0x19E);
                    func_001EFD90(0x8000000B, arg0 + 0xB0, arg0 + 0xC0);
                    func_001EFD90(0x80000014, arg0 + 0xB0, arg0 + 0xC0);
                } else if (kk == 0x1E) {
                    func_001FC580(arg0, 0x19D);
                    func_001EFD90(0x80000031, arg0 + 0xB0, arg0 + 0xC0);
                    func_001EFD90(0x80000015, arg0 + 0xB0, arg0 + 0xC0);
                } else if (kk == 0x1F) {
                    func_001FC580(arg0, 0x19E);
                    func_001EFD90(0x80000032, arg0 + 0xB0, arg0 + 0xC0);
                    func_001EFD90(0x80000014, arg0 + 0xB0, arg0 + 0xC0);
                }
            }
            {
                int k2;
                if (*(short *)(arg0 + 0x36) == 0 ||
                    ((k2 = *(unsigned char *)(arg0 + 3)) != 6 && k2 != 0x1E)) {
                    *(unsigned char *)(arg0 + 4) = 3;
                } else {
                    int r;
                    func_001029C0(D_700036E0);
                    r = func_00122BB8() >> 16; r <<= 2; r >>= 15;
                    switch (r) {
                    case 1:
                        func_00102BB0(D_700036E0, D_700036E0, 1.5707964f);
                        break;
                    case 2:
                        func_00102BB0(D_700036E0, D_700036E0, 3.1415927f);
                        break;
                    case 3:
                        func_00102BB0(D_700036E0, D_700036E0, 4.712389f);
                        break;
                    }
                    func_00102948(D_700038A0, arg0 + 0x100);
                    *(int *)(arg0 + 0x108) = 0;
                    *(int *)(arg0 + 0x104) = 0;
                    *(int *)(arg0 + 0x100) = 0;
                    func_001026D0(arg0 + 0xD0, arg0 + 0xD0, D_700036E0);
                    func_00102948(arg0 + 0x100, D_700038A0);
                    if (*(unsigned char *)(arg0 + 3) == 6) {
                        func_001CA6E0(arg0, func_001C6120(D_0028A56C[0], 0x22));
                    } else {
                        func_001CA6E0(arg0, func_001C6120(D_0028A56C[0], 0x29));
                    }
                    bone_init_default_1(arg0);
                }
            }
            break;
        }
        case 1: {
            short t3 = *(short *)(arg0 + 0x2A);
            if ((t3 != 0 || *(unsigned char *)(arg0 + 0xA) != 0) &&
                *(unsigned short *)(arg0 + 0x52) != 0) {
                int cnt2 = 0;
                if (t3 != 0) {
                    *(short *)(arg0 + 0x2A) = *(volatile short *)(arg0 + 0x2A) - 1;
                }
                func_00102948(D_700038A0, arg0 + 0xB0);
                {
                    float a4c = *(float *)0x700038A4;
                    *(int *)0x700038B4 = 0xC0000000;
                    *(float *)0x700038A4 = a4c - 1.0f;
                }
                *(float *)0x700038A0 = *(float *)(arg0 + 0x2EC);
                *(float *)0x700038A8 = *(float *)(arg0 + 0x2DC);
                if (func_0019AB20(arg0, D_700038A0, D_700038B0, 6) != 0) {
                    cnt2 += 1;
                }
                *(float *)0x700038A0 = *(float *)(arg0 + 0x2E8);
                *(float *)0x700038A8 = *(float *)(arg0 + 0x2D8);
                if (func_0019AB20(arg0, D_700038A0, D_700038B0, 6) != 0) {
                    cnt2 += 1;
                }
                *(float *)0x700038A0 = *(float *)(arg0 + 0x2E4);
                *(float *)0x700038A8 = *(float *)(arg0 + 0x2D4);
                if (func_0019AB20(arg0, D_700038A0, D_700038B0, 6) != 0) {
                    cnt2 += 1;
                }
                *(float *)0x700038A0 = *(float *)(arg0 + 0x2E0);
                *(float *)0x700038A8 = *(float *)(arg0 + 0x2D0);
                if (func_0019AB20(arg0, D_700038A0, D_700038B0, 6) != 0) {
                    cnt2 += 1;
                }
                if (cnt2 >= 3) {
                    if (*(short *)(arg0 + 0x2A) == 0 && *(unsigned char *)(arg0 + 0xA) != 0) {
                        *(short *)(arg0 + 0x2A) = 6;
                    }
                } else {
                    *(unsigned char *)(arg0 + 4) = 3;
                }
            }
            break;
        }
        default:
            break;
        }
        copy_qw4((char *)*(int *)D_00275B40 + 0x90, arg0 + 0xD0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    }
    case 3:
    default: {
        if (*(unsigned char *)(arg0 + 3) == 0x50) {
            unsigned char h = *(unsigned char *)(arg0 + 0x9A);
            if (h != 0) {
                func_001B1190(h);
            }
        }
        func_001AFC10(arg0);
        break;
    }
    }
}
