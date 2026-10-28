// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x70003A20 0x70003A24 0x70003050 0x70003054 0x70003058 0x70003060 0x70003068 0x700031B0 0x700031B8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.95% NEARMISS).
// Fixes read from the original instructions:
//  - the scratch vector at 0x700038A0 is one relocated struct (x, y, z, w), so its
//    four stores keep their source order;
//  - the impact point / normal words (0x70003050..68, 0x700031B0/B8) and the
//    planar delta pair (0x70003A20/24) are relocated externs (idiom-32), which lets
//    their loads be scheduled ahead of the scratch stores;
//  - the planar distance squares a reread of the stored z delta (no early z local).
//
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
// a synthesized capsule: offset (0, 4.01, 9.5) rotated by D_70003070, w cleared
// then set to 1.0f) instead of the simple distance/angle gates, and a different
// flags byte (+0x1F1=0) on success.

extern void func_001026A0(void *dst, void *a, void *b);
extern int func_001028B8(void *dst, void *a, void *b);
extern float func_0011DF78(float x);
extern float func_0011E748(float x);
extern void func_00177510(void);
extern int func_001775E0(void *a0, int a1, float a2);
extern int func_001776E0(void *a0, float a1);
extern int func_00177CF0(void *a0, float a1);
extern short func_0019A180(int a0, int a1);
extern int func_0019AD00(void *a0, void *a1, int a2);
extern int func_0019AFE0(void *a0, void *a1, void *a2, int a3);
extern void func_0019BC40(void *a0);
extern float D_00282250[];
extern unsigned char D_008106F1;
extern float D_70003070;
extern unsigned short D_70003170[];
extern float D_700030F0[];
typedef struct { float x, y, z; int w; } Vec4;
extern Vec4 D_700038A0;
extern float D_700038B0;
extern float D_700038C0;
extern float D_70003A20;
extern float D_70003050;
extern float D_70003054;
extern float D_70003058;
extern float D_70003060;
extern float D_70003068;
extern float D_700031B0;
extern float D_700031B8;
extern float D_70003A24;

int func_0017C860(char *arg0, float fparg0) {
    int i;
    unsigned short *flags;
    float *heading;
    float *pos;
    short id;
    short cls;
    float dz;
    float vx, vy, vz;
    float dist0, dist1;
    unsigned short *flags2;
    float *heading2;
    float *pos2;
    int j;

    if (*(float *)(arg0 + 0x220) <= 0.0f ||
        (!(*(float *)(arg0 + 0x228) < 100.0f) && D_008106F1 != 0)) {
        return 0;
    }

    D_700038A0.x = 0.0f;
    D_700038A0.y = 18.0f;
    D_700038A0.z = 5.5f;
    D_700038A0.w = 0x3F800000;
    func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);

    if (func_0019AD00(arg0, &D_700038B0, 7) != 0) {
        cls = *(short *)(*(char **)0x700031D0 + 0x1A);
        if ((cls & 0xFF) != 0x46 && (cls & 0xFF00) == 0x2000) {
            func_00177510();
            D_70003A20 = D_70003050 - *(float *)(arg0 + 0xB0);
            D_70003A24 = D_70003058 - *(float *)(arg0 + 0xB8);
            dz = D_70003A24;
            dist0 = func_0011E748((D_70003A20 * D_70003A20) + (dz * dz));

            D_700038A0.x = 0.0f;
            D_700038A0.y = 4.01f;
            D_700038A0.z = 5.5f;
            D_700038A0.w = 0x3F800000;
            func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);
            if (func_0019AD00(arg0, &D_700038B0, 7) != 0) {
                D_70003A20 = D_700031B0 - *(float *)(arg0 + 0xB0);
                D_70003A24 = D_700031B8 - *(float *)(arg0 + 0xB8);
                dz = D_70003A24;
                dist1 = func_0011E748((D_70003A20 * D_70003A20) + (dz * dz));
                if ((0.5f + dist1) < dist0) {
                    return 0;
                }
            }

            vx = D_70003050 - D_70003060;
            vy = D_70003054;
            vz = D_70003058 - D_70003068;
            D_700038A0.x = vx;
            D_700038A0.y = vy;
            D_700038A0.z = vz;
            D_700038A0.w = 0x3F800000;
            func_0019BC40(&D_700038A0);

            if (*(int *)0x700031E0 != 0) {
                i = *(int *)0x700031E0 - 1;
                if (i >= 0) {
                    flags = &D_70003170[i];
                    heading = &D_00282250[i];
                    pos = &D_700030F0[i];
                    do {
                        if (*flags & 1) {
                            id = func_0019A180(0, i);
                            if ((id & 0xFF) != 0x46 && *heading < 0.62831855f &&
                                func_001775E0(arg0, 0, *pos) == 0) {
                                D_70003A20 = 20.5f + *(float *)(arg0 + 0xB4);
                                if (!((D_70003A20 - fparg0) < *pos) &&
                                    !(*pos < D_70003A20) &&
                                    func_001776E0(arg0, *pos) == 0 &&
                                    func_00177CF0(arg0, *pos - 1.0f) == 0) {
                                    *(float *)(arg0 + 0x2E0) = D_70003050 + (1.5f * D_70003060);
                                    *(float *)(arg0 + 0x2E8) = D_70003058 + (1.5f * D_70003068);
                                    *(float *)(arg0 + 0x2E4) = D_700030F0[i] - 20.5f;
                                    *(float *)(arg0 + 0x254) = D_700030F0[i];
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
                        i--;
                        flags--;
                        heading--;
                        pos--;
                    } while (i >= 0);
                }
            }
        }
    } else {
        D_700038A0.x = 0.0f;
        D_700038A0.y = 4.01f;
        D_700038A0.z = 5.5f;
        D_700038A0.w = 0x3F800000;
        func_001026A0(&D_700038B0, arg0 + 0xD0, &D_700038A0);
        if (func_0019AD00(arg0, &D_700038B0, 7) != 0) {
            cls = *(short *)(*(char **)0x700031D0 + 0x1A);
            if ((cls & 0xFF) != 0x46 && (cls & 0xFF00) == 0x2000) {
                func_00177510();
                vx = D_70003050 - D_70003060;
                vy = D_70003054;
                vz = D_70003058 - D_70003068;
                D_700038A0.x = vx;
                D_700038A0.y = vy;
                D_700038A0.z = vz;
                D_700038A0.w = 0x3F800000;
                func_0019BC40(&D_700038A0);

                if (*(int *)0x700031E0 != 0) {
                    j = *(int *)0x700031E0 - 1;
                    if (j >= 0) {
                        flags2 = &D_70003170[j];
                        heading2 = &D_00282250[j];
                        pos2 = &D_700030F0[j];
                        do {
                            if (*flags2 & 1) {
                                id = func_0019A180(0, j);
                                if ((id & 0xFF) != 0x46 && *heading2 < 0.62831855f &&
                                    func_001775E0(arg0, 1, *pos2) == 0) {
                                    dist1 = *pos2;
                                    dist1 = func_0011DF78(dist1 - *(float *)(arg0 + 0xB4));
                                    D_70003A20 = dist1;
                                    if (dist1 <= 0.5f) {
                                        D_700038A0.x = *(float *)(arg0 + 0xB0);
                                        D_700038A0.y = *pos2;
                                        D_700038A0.z = *(float *)(arg0 + 0xB8);
                                        D_700038A0.w = 0x3F800000;
                                        (*(float *)0x700038B0) = 0.0f;
                                        (*(float *)0x700038B4) = 4.01f;
                                        (*(float *)0x700038B8) = 9.5f;
                                        (*(int *)0x700038BC) = 0;
                                        func_001026A0(&D_700038C0, &D_70003070, &D_700038B0);
                                        func_001028B8(&D_700038B0, &D_700038A0, &D_700038C0);
                                        (*(int *)0x700038BC) = 0x3F800000;
                                        if (func_0019AFE0(arg0, &D_700038A0, &D_700038B0, 7) == 0) {
                                            *(float *)(arg0 + 0x2E0) = D_70003050 + (1.5f * D_70003060);
                                            *(float *)(arg0 + 0x2E8) = D_70003058 + (1.5f * D_70003068);
                                            *(float *)(arg0 + 0x2E4) = D_700030F0[j];
                                            *(float *)(arg0 + 0x254) = D_700030F0[j];
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
                            j--;
                            flags2--;
                            heading2--;
                            pos2--;
                        } while (j >= 0);
                    }
                }
            }
        }
    }
    return 0;
}
