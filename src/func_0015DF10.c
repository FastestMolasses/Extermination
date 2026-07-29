// NEARMISS func_0015DF10  (vram 0x0015DF10, 0xD38 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.26% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring + list-scheduling permutation (permuter class, NOT the clean-store delay-slot nop). Residual on mwcc 2.3.3 is ~50 of 865 instructions in four recurring spots, all with correct operands/opcodes but a different register assignment or issue order: (1) the two ledge-distance bloc...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Ledge-grab / vault probe. Called with the entity `p`, `mode` (0 = running
// vault, non-zero = standing grab) and `ang` = the entity's desired facing.
// Sweeps the collision capsule at two heights (0x700038A0 workspace matrix),
// picks whichever hit is nearer, checks the approach angle is within 30 deg,
// then walks the ledge list (D_70003170 flags / D_700030F0 heights /
// D_00282250 slopes, count in 0x700031E0) from the top down looking for a
// ledge the entity can actually mount. On success it sets the anim/state
// triple (p+5 / p+6 / p+0x1F0), stores the grab target into p+0x2E0/0x2E8 and
// returns 1; every rejection path returns 0.

extern unsigned char D_008106BE;
extern char D_827880[];
extern char D_828700[];
extern char D_002489F0[];
extern float D_00282250[];
extern char D_70003070[];
extern float D_700030F0[];
extern unsigned short D_70003170[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];
extern float D_002488A0;
extern float D_002488B0;
extern float D_002488B4;

extern void build_trs_matrix(void *a, void *b, void *c, void *d);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern float func_0011DF78(float a);
extern float func_0011E748(float a);
extern int func_0015DEC0(void);
extern int func_001760C0(char *p, void *a, int n, float d);
extern int func_00177460(char *p, int n);
extern void func_00177510(void);
extern int func_001775E0(char *p, int n, float y);
extern int func_001776E0(char *p, float y);
extern int func_00177CF0(char *p, float y);
extern int func_00177F40(char *p, float y);
extern long long func_0019A180(int a, int idx);
extern int func_0019AD00(char *p, void *m, int n);
extern int func_0019AFE0(char *p, void *a, void *b, int n);
extern void func_0019BC40(void *m);
extern float func_001B1470(float a);
extern int func_00219550();

int func_0015DF10(char *p, int mode, float ang)
{
    float v[4];
    char *q;
    int w;
    int r;
    float d0;
    float e0;
    float e1;
    float lastY;
    float h0;
    float h1;
    float mul;
    float d1;
    float dx;
    float dz;
    int n;
    int i;
    int found;
    unsigned short *flags;
    float *ys;
    float *as;
    float y;
    float dy;
    char *e;
    float t;
    int r2;

    if ((D_008106BE & 1) && !(D_008106BE & 0x80)) {
        return 0;
    }
    q = *(char **)(p + 0x308);
    if (q != 0) {
        w = *(int *)(q + 0x10);
        if ((w == (int)D_828700 || w == (int)D_827880) && !(*(float *)(p + 0xB4) < 60.0f)) {
            return 0;
        }
    }
    if (mode == 0) {
        unsigned char st;
        *(unsigned char *)(p + 0x2F2) = 1;
        st = *(unsigned char *)(p + 0x25C);
        if ((int)st < 2) {
            mul = 2.0f;
        } else if (st == 2) {
            mul = 4.0f;
        } else {
            mul = 6.0f;
        }
        func_00102948(D_700038A0, D_002489F0);
        *(float *)0x700038A8 = *(float *)0x700038A8 * (0.75f * mul);
    } else {
        *(unsigned char *)(p + 0x2F2) = 0;
        func_00102948(D_700038A0, D_002489F0);
        *(float *)0x700038A8 = 5.0f;
    }
    func_001026A0(v, p + 0xD0, D_700038A0);
    r = func_0019AD00(p, v, 7);
    if (r & 1) {
        return 0;
    }
    if (r != 0) {
        if (func_0015DEC0() == 0) {
            return 0;
        }
        func_00177510();
        h0 = *(float *)0x700031E4;
        t = func_0011DF78(func_001B1470(h0 - ang));
        *(float *)0x70003A20 = *(float *)0x700031B0 - *(float *)(p + 0xB0);
        e0 = t;
        dz = *(float *)0x700031B8 - *(float *)(p + 0xB8);
        *(float *)0x70003A24 = dz;
        d0 = func_0011E748(*(float *)0x70003A20 * *(float *)0x70003A20 + dz * dz);
    } else {
        d0 = 50.0f;
    }
    v[1] -= 13.99f;
    r = func_0019AD00(p, v, 7);
    if (r & 1) {
        return 0;
    }
    if (r != 0) {
        if (func_0015DEC0() == 0) {
            return 0;
        }
        func_00177510();
        h1 = *(float *)0x700031E4;
        t = func_0011DF78(func_001B1470(h1 - ang));
        *(float *)0x70003A20 = *(float *)0x700031B0 - *(float *)(p + 0xB0);
        e1 = t;
        dz = *(float *)0x700031B8 - *(float *)(p + 0xB8);
        *(float *)0x70003A24 = dz;
        d1 = func_0011E748(*(float *)0x70003A20 * *(float *)0x70003A20 + dz * dz);
    } else {
        d1 = 50.0f;
    }
    if (d0 == 50.0f && d1 == 50.0f) {
        return 0;
    }
    if (!(d0 < d1)) {
        if (!(e1 <= 0.5235988f)) {
            return 0;
        }
        *(float *)(p + 0xC4) = h1;
        build_trs_matrix(p + 0xD0, p + 0xB0, p + 0xC0, p + 0x60);
        func_001026A0(D_700038B0, p + 0xD0, D_700038A0);
        *(float *)0x700038B4 = 4.01f + *(float *)(p + 0xB4);
    } else {
        if (!(e0 <= 0.5235988f)) {
            return 0;
        }
        *(float *)(p + 0xC4) = h0;
        build_trs_matrix(p + 0xD0, p + 0xB0, p + 0xC0, p + 0x60);
        func_001026A0(D_700038B0, p + 0xD0, D_700038A0);
        *(float *)0x700038B4 = 18.0f + *(float *)(p + 0xB4);
    }
    r = func_0019AD00(p, D_700038B0, 7);
    if (r & 1) {
        return 0;
    }
    if (r != 0) {
        if (func_0015DEC0() == 0) {
            return 0;
        }
        func_00177510();
        t = func_0011DF78(func_001B1470(*(float *)0x700031E4 - ang));
        *(float *)0x70003A20 = t;
        if (!(t <= 0.5235988f)) {
            return 0;
        }
    }
    {
        float fx;
        float fy;
        fx = *(float *)0x70003050 - (0.5f * *(float *)0x70003060);
        fy = *(float *)0x70003054;
        *(float *)0x700038A0 = fx;
        *(float *)0x700038A4 = fy;
        *(float *)0x700038A8 = *(float *)0x70003058 - (0.5f * *(float *)0x70003068);
        *(int *)0x700038AC = 0x3F800000;
    }
    func_0019BC40(D_700038A0);
    n = *(int *)0x700031E0;
    if (n != 0) {
        i = n - 1;
        found = 0;
        if (i >= 0) {
        flags = D_70003170 + i;
        ys = D_700030F0 + i;
        as = D_00282250 + i;
        do {
            if (!(*flags & 1)) {
                lastY = *ys;
                found = 1;
                goto next;
            }
            if (((short)func_0019A180(0, i) & 0xFF) == 0x46) {
                goto next;
            }
            y = *ys;
            dy = y - *(float *)(p + 0xB4);
            if (dy <= D_002488A0) {
                goto next;
            }
            if (!(dy <= D_002488B4)) {
                goto next;
            }
            if (!(*as < 0.62831855f)) {
                goto next;
            }
            if (func_001775E0(p, 1, y) != 0) {
                goto next;
            }
            if (dy <= D_002488B0) {
                if (found != 0) {
                    if ((lastY - *ys) < 14.0f) {
                        goto next;
                    }
                }
                *(float *)0x700038A0 = *(float *)0x70003050;
                *(float *)0x700038A4 = *ys;
                *(float *)0x700038A8 = *(float *)0x70003058;
                *(int *)0x700038AC = 0x3F800000;
                *(int *)0x700038B0 = 0;
                *(float *)0x700038B4 = 4.01f;
                *(float *)0x700038B8 = 4.5f;
                *(int *)0x700038BC = 0;
                func_001026A0(D_700038C0, D_70003070, D_700038B0);
                func_001028B8(D_700038B0, D_700038A0, D_700038C0);
                *(int *)0x700038BC = 0x3F800000;
                if (func_0019AFE0(p, D_700038A0, D_700038B0, 7) != 0) {
                    e = *(char **)0x700031D4;
                    if (e == 0) {
                        goto next;
                    }
                    if ((*(unsigned char *)(e + 2) & ~0xE0) != 4) {
                        goto next;
                    }
                    if (*(int *)(e + 0x10) != (int)func_00219550) {
                        goto next;
                    }
                }
                func_00102948(D_700038A0, D_700038B0);
                *(float *)0x700038B0 = 4.0f;
                *(float *)0x700038B4 = 0.0f;
                *(float *)0x700038B8 = -0.5f;
                *(int *)0x700038BC = 0;
                func_001026A0(D_700038D0, D_70003070, D_700038B0);
                func_001028B8(D_700038B0, D_700038A0, D_700038D0);
                *(int *)0x700038BC = 0x3F800000;
                *(float *)0x700038C0 = -4.0f;
                *(float *)0x700038C4 = 0.0f;
                *(float *)0x700038C8 = -0.5f;
                *(int *)0x700038CC = 0;
                func_001026A0(D_700038D0, D_70003070, D_700038C0);
                func_001028B8(D_700038C0, D_700038A0, D_700038D0);
                *(int *)0x700038CC = 0x3F800000;
                if (func_0019AFE0(p, D_700038B0, D_700038C0, 7) != 0) {
                    goto next;
                }
                if (func_0019AFE0(p, D_700038C0, D_700038B0, 7) != 0) {
                    goto next;
                }
                if (func_00177F40(p, *ys) == 0) {
                    goto next;
                }
                *(float *)0x700038A0 = *(float *)(p + 0xB0);
                *(float *)0x700038A8 = *(float *)(p + 0xB8);
                *(float *)0x700038A4 = *ys - 4.0f;
                *(int *)0x700038AC = 0;
                if (func_001760C0(p, D_700038A0, 1, 18.0f) != 0) {
                    goto next;
                }
                *(float *)0x700038B0 = 4.5f;
                *(float *)0x700038B4 = 0.0f;
                *(float *)0x700038B8 = 0.0f;
                *(int *)0x700038BC = 0x3F800000;
                func_001026A0(D_700038A0, p + 0xD0, D_700038B0);
                *(float *)0x700038A4 = *ys - 4.0f;
                if (func_001760C0(p, D_700038A0, 1, 18.0f) != 0) {
                    goto next;
                }
                *(float *)0x700038B0 = -4.5f;
                *(float *)0x700038B4 = 0.0f;
                *(float *)0x700038B8 = 0.0f;
                *(int *)0x700038BC = 0x3F800000;
                func_001026A0(D_700038A0, p + 0xD0, D_700038B0);
                *(float *)0x700038A4 = *ys - 4.0f;
                if (func_001760C0(p, D_700038A0, 1, 18.0f) != 0) {
                    goto next;
                }
                *(float *)(p + 0x2E0) = *(float *)0x70003050 + (4.5f * *(float *)0x70003060);
                *(float *)(p + 0x2E8) = *(float *)0x70003058 + (4.5f * *(float *)0x70003068);
                *(float *)(p + 0x254) = dy;
                t = *(float *)0x700031E4;
                *(float *)(p + 0xC4) = t;
                *(float *)(p + 0x218) = t;
                *(char *)(p + 0x1F1) = 0;
                *(char *)(p + 0x25F) = 1;
                if (*(unsigned char *)(p + 0x2F2) != 0 && func_00177460(p, 0) != 0) {
                    *(char *)(p + 5) = 3;
                    *(char *)(p + 6) = 0;
                    *(char *)(p + 0x1F0) = 9;
                    *(float *)(p + 0x2E0) = *(float *)0x70003050;
                    *(float *)(p + 0x2E8) = *(float *)0x70003058;
                    *(float *)(p + 0x290) = *(float *)0x70003060;
                    *(float *)(p + 0x298) = *(float *)0x70003068;
                } else {
                    *(char *)(p + 5) = 2;
                    *(char *)(p + 6) = 0;
                    *(char *)(p + 0x1F0) = 8;
                }
                return 1;
            }
            if (mode != 0) {
                goto next;
            }
            r2 = func_001776E0(p, *ys);
            if (r2 == 1 || r2 == 2) {
                goto next;
            }
            if (r2 != 0) {
                goto next;
            }
            if (func_00177CF0(p, *ys - 1.0f) != 0) {
                goto next;
            }
            t = 2.0f + (*ys - *(float *)(p + 0xB4));
            *(float *)0x70003A20 = t;
            if (func_001760C0(p, p + 0xB0, 1, t) != 0) {
                goto next;
            }
            *(float *)0x700038A0 = *(float *)0x70003050 + *(float *)0x70003060;
            *(float *)0x700038A4 = *(float *)(p + 0xB4);
            *(float *)0x700038A8 = *(float *)0x70003058 + *(float *)0x70003068;
            *(int *)0x700038AC = 0x3F800000;
            if (func_001760C0(p, D_700038A0, 1, *(float *)0x70003A20) != 0) {
                goto next;
            }
            *(float *)(p + 0x2E0) = *(float *)0x70003050 + (1.5f * *(float *)0x70003060);
            *(float *)(p + 0x2E8) = *(float *)0x70003058 + (1.5f * *(float *)0x70003068);
            *(float *)(p + 0x254) = dy;
            t = *(float *)0x700031E4;
            *(float *)(p + 0xC4) = t;
            *(float *)(p + 0x218) = t;
            *(char *)(p + 0x1F1) = 1;
            *(char *)(p + 0x25F) = 1;
            if (*(unsigned char *)(p + 0x2F2) != 0 && func_00177460(p, 1) != 0) {
                *(char *)(p + 5) = 3;
                *(char *)(p + 6) = 0;
                *(char *)(p + 0x1F0) = 9;
                *(float *)(p + 0x290) = *(float *)0x70003060;
                *(float *)(p + 0x298) = *(float *)0x70003068;
            } else {
                *(char *)(p + 5) = 2;
                *(char *)(p + 6) = 0;
                *(char *)(p + 0x1F0) = 8;
            }
            return 1;
        next:
            i -= 1;
            flags -= 1;
            ys -= 1;
            as -= 1;
            } while (i >= 0);
        }
    }
    return 0;
}
