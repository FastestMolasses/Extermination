// NEARMISS func_0017C860  (vram 0x0017C860, 0x7DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.66% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// VU0/FPU register-coloring and instruction-scheduling spread across many mula.s/madd.s sum-of-squares sequences and two large near-duplicate ring-buffer scan loops -- same class/ceiling as the matched sibling func_0017AF70 (87.23%). No jr-table, no unrecoverable control flow; not further reducible...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Enemy melee/ranged "can I hit target" scan. arg0 is the actor, fparg0 is a
// caller-supplied cone/reach parameter. Bails out early if the actor's
// "alert" timer (+0x220) has expired, or its "cooldown" (+0x228) is still
// above 100 and the global retreat/ambush flag D_008106F1 is set.
//
// Builds a small offset vector (0,18,5.5) [then (0,4.01,5.5) later] into the
// scratch 0x700038A0..AC quad, rotates it by the actor's world matrix
// (func_001026A0), and asks func_0019AD00 to find what's hit along that ray.
// If the hit record's flags (lo byte) aren't the "wall" sentinel 0x46 and the
// hi byte marks it a "player" class (0x2000), computes the planar (XZ)
// distance from the actor to the stored impact point (0x70003050/58) via
// func_00177510 + a mula.s/madd.s sum-of-squares + sqrt (func_0011E748), then
// repeats with a (0, 4.01, 5.5) offset: if that second distance-plus-0.5f
// check fails (too far), bails to the shared "no target" return.
//
// Otherwise it derives an aim point from the hit normal (0x70003060/68) and
// scans the "nearby actor" ring buffers (D_70003170 flags, D_00282250
// headings, D_700030F0 positions) from the current count down to 0, picking
// the first live entry (flag bit 0) that isn't itself a wall (func_0019A180
// class != 0x46), whose heading is within ~0.126*PI of the impact heading,
// and that passes func_001775E0/001776E0/00177CF0 line-of-sight/range gates
// relative to fparg0. On a hit it stashes an aim/lunge target into the actor
// record (+0x2E0/0x2E4/0x2E8 position, +0x254/+0x218 heading/aux), flags
// +0x25F/+0x1F0/+0x1F1, sets state (+5=4,+6=0), and returns 1.
//
// The second half (post func_0019BC40 raycast-again) repeats the same ring
// scan for a variant target using func_0019AFE0 (a second collision test with
// a synthesized capsule at D_70003070/(4.01,17.5,0)) instead of the simple
// distance/angle gates, and a different flags byte (+0x1F1=0) on success.
extern void func_001026A0(void *dst, void *a, void *b);
extern int func_001028B8(void *dst, void *a, void *b);
extern float func_0011DF78(float x);
extern float func_0011E748(float x);
extern void func_00177510(int id);
extern int func_001775E0(void *a0, int a1, float a2);
extern int func_001776E0(void *a0, float a1);
extern int func_00177CF0(void *a0, float a1);
extern short func_0019A180(int a0, int a1);
extern int func_0019AD00(void *a0, void *a1, int a2);
extern int func_0019AFE0(void *a0, void *a1, void *a2, int a3);
extern void func_0019BC40(void *a0);

extern short D_00282250[];
extern unsigned char D_008106F1;
extern int D_700031E0;
extern float D_70003070;
extern unsigned short D_70003170[];
extern float D_700030F0[];
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;

int func_0017C860(char *arg0, float fparg0) {
    int i;
    unsigned short *flags;
    short *heading;
    float *pos;
    short id;
    short cls;
    float dx, dz;
    float dist0, dist1;

    if (*(float *)(arg0 + 0x220) <= 0.0f) {
        return 0;
    }
    if (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1 != 0) {
        return 0;
    }

    (*(float *)0x700038A0) = 0.0f;
    (*(float *)0x700038A4) = 18.0f;
    (*(float *)0x700038A8) = 5.5f;
    (*(int *)0x700038AC) = 0x3F800000;
    func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);

    if (func_0019AD00(arg0, &D_700038B0, 7) != 0) {
        cls = *(short *)(*(char **)0x700031D0 + 0x1A);
        if ((cls & 0xFF) == 0x46) {
            return 0;
        }
        if ((cls & 0xFF00) == 0x2000) {
            func_00177510(cls);
            (*(float *)0x70003A20) = (*(float *)0x70003050) - *(float *)(arg0 + 0xB0);
            (*(float *)0x70003A24) = (*(float *)0x70003058) - *(float *)(arg0 + 0xB8);
            dist0 = func_0011E748(((*(float *)0x70003A20) * (*(float *)0x70003A20)) + ((*(float *)0x70003A24) * (*(float *)0x70003A24)));

            (*(float *)0x700038A0) = 0.0f;
            (*(float *)0x700038A4) = 4.01f;
            (*(float *)0x700038A8) = 5.5f;
            (*(int *)0x700038AC) = 0x3F800000;
            func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);
            if (func_0019AD00(arg0, &D_700038B0, 7) != 0) {
                (*(float *)0x70003A20) = (*(float *)0x700031B0) - *(float *)(arg0 + 0xB0);
                (*(float *)0x70003A24) = (*(float *)0x700031B8) - *(float *)(arg0 + 0xB8);
                dist1 = func_0011E748(((*(float *)0x70003A20) * (*(float *)0x70003A20)) + ((*(float *)0x70003A24) * (*(float *)0x70003A24)));
                if ((0.5f + dist1) < dist0) {
                    return 0;
                }
            }

            dx = (*(float *)0x70003050) - (*(float *)0x70003060);
            dz = (*(float *)0x70003058) - (*(float *)0x70003068);
            (*(float *)0x700038A0) = dx;
            (*(float *)0x700038A4) = (*(float *)0x70003054);
            (*(float *)0x700038A8) = dz;
            (*(int *)0x700038AC) = 0x3F800000;
            func_0019BC40(&D_700038A0);

            if (D_700031E0 != 0) {
                i = D_700031E0 - 1;
                if (i >= 0) {
                    flags = &D_70003170[i];
                    heading = &D_00282250[i * 2];
                    pos = &D_700030F0[i * 4];
                    for (; i >= 0; i--, flags--, heading -= 2, pos -= 4) {
                        if (!(*flags & 1)) {
                            continue;
                        }
                        id = (short) (func_0019A180(0, i) & 0xFF);
                        if (id == 0x46) {
                            continue;
                        }
                        if (!(*heading < 0.62831855f)) {
                            continue;
                        }
                        if (func_001775E0(arg0, 0, *pos) != 0) {
                            continue;
                        }
                        if ((20.5f + *(float *)(arg0 + 0xB4) - fparg0) < *pos) {
                            continue;
                        }
                        if (*pos < (20.5f + *(float *)(arg0 + 0xB4))) {
                            continue;
                        }
                        if (func_001776E0(arg0, *pos) != 0) {
                            continue;
                        }
                        if (func_00177CF0(arg0, *pos - 1.0f) != 0) {
                            continue;
                        }

                        *(float *)(arg0 + 0x2E0) = (*(float *)0x70003050) + (1.5f * (*(float *)0x70003060));
                        *(float *)(arg0 + 0x2E8) = (*(float *)0x70003058) + (1.5f * (*(float *)0x70003068));
                        *(float *)(arg0 + 0x2E4) = pos[0] - 20.5f;
                        *(float *)(arg0 + 0x254) = pos[0];
                        *(float *)(arg0 + 0x218) = (*(float *)0x700031E4);
                        *(unsigned char *)(arg0 + 0x25F) = 1;
                        *(unsigned char *)(arg0 + 5) = 4;
                        *(unsigned char *)(arg0 + 6) = 0;
                        *(unsigned char *)(arg0 + 0x1F0) = 9;
                        *(unsigned char *)(arg0 + 0x1F1) = 1;
                        return 1;
                    }
                }
            }
            return 0;
        }
        return 0;
    }

    (*(float *)0x700038A0) = 0.0f;
    (*(float *)0x700038A4) = 4.01f;
    (*(float *)0x700038A8) = 5.5f;
    (*(int *)0x700038AC) = 0x3F800000;
    func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);
    if (func_0019AD00(arg0, &D_700038B0, 7) != 0) {
        cls = *(short *)(*(char **)0x700031D0 + 0x1A);
        if ((cls & 0xFF) != 0x46 && (cls & 0xFF00) == 0x2000) {
            func_00177510(cls);
            dx = (*(float *)0x70003050) - (*(float *)0x70003060);
            dz = (*(float *)0x70003058) - (*(float *)0x70003068);
            (*(float *)0x700038A0) = dx;
            (*(float *)0x700038A4) = (*(float *)0x70003054);
            (*(float *)0x700038A8) = dz;
            (*(int *)0x700038AC) = 0x3F800000;
            func_0019BC40(&D_700038A0);

            if (D_700031E0 != 0) {
                i = D_700031E0 - 1;
                if (i >= 0) {
                    flags = &D_70003170[i];
                    heading = &D_00282250[i * 2];
                    pos = &D_700030F0[i * 4];
                    for (; i >= 0; i--, flags--, heading -= 2, pos -= 4) {
                        if (!(*flags & 1)) {
                            continue;
                        }
                        id = (short) (func_0019A180(0, i) & 0xFF);
                        if (id == 0x46) {
                            continue;
                        }
                        if (!(*heading < 0.62831855f)) {
                            continue;
                        }
                        if (func_001775E0(arg0, 1, *pos) != 0) {
                            continue;
                        }
                        dist1 = func_0011DF78(*pos - *(float *)(arg0 + 0xB4));
                        (*(float *)0x70003A20) = dist1;
                        if (!(dist1 <= 0.5f)) {
                            continue;
                        }

                        (*(float *)0x700038A0) = *(float *)(arg0 + 0xB0);
                        (*(float *)0x700038A4) = *pos;
                        (*(float *)0x700038A8) = *(float *)(arg0 + 0xB8);
                        (*(int *)0x700038AC) = 0x3F800000;
                        (*(float *)0x700038B0) = 0.0f;
                        (*(float *)0x700038B4) = 4.01f;
                        (*(float *)0x700038B8) = 9.5f;
                        (*(int *)0x700038BC) = 0x3F800000;
                        func_001026A0(&D_700038C0, &D_70003070, &D_700038B0);
                        func_001028B8(&D_700038B0, &D_700038A0, &D_700038C0);
                        (*(int *)0x700038BC) = 0x3F800000;
                        if (func_0019AFE0(arg0, &D_700038A0, &D_700038B0, 7) == 0) {
                            *(float *)(arg0 + 0x2E0) = (*(float *)0x70003050) + (1.5f * (*(float *)0x70003060));
                            *(float *)(arg0 + 0x2E8) = (*(float *)0x70003058) + (1.5f * (*(float *)0x70003068));
                            *(float *)(arg0 + 0x2E4) = pos[0];
                            *(float *)(arg0 + 0x254) = pos[0];
                            *(float *)(arg0 + 0x218) = (*(float *)0x700031E4);
                            *(unsigned char *)(arg0 + 0x25F) = 1;
                            *(unsigned char *)(arg0 + 5) = 4;
                            *(unsigned char *)(arg0 + 6) = 0;
                            *(unsigned char *)(arg0 + 0x1F0) = 9;
                            *(unsigned char *)(arg0 + 0x1F1) = 0;
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
