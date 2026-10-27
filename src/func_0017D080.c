// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003050 0x70003054 0x70003058 0x70003060 0x70003068 0x700031D0 0x700031D4 0x700031E0 0x700031E4 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B4 0x70003A20 0x70003A24 0x70003A28 0x70003A2C

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.85% NEARMISS).
// Fixes read from the original instructions:
//  - Wrong body: a failed first ray probe (func_0019AD00 & 6 == 0) returns 0;
//    the old C fell through to the node search. Everything after the probe is
//    nested inside that if, so every failure path shares the final return 0.
//  - Wrong body: in the node search a hit class 0x46 SKIPS the node when the
//    facing flag is clear; the old C skipped every other node instead.
//  - The func_001000C0 threshold is (double)1.8849556f (low word 0x40000000).
//  - Every scratchpad word is its own relocated extern (idiom-32, // SPAD:),
//    so no scratchpad upper half is copied into a branch delay slot; the
//    output vector words 0x700038A0..AC are volatile (their stores keep source
//    order while the 0x70003050.. loads hoist above them).
//  - The first node scan indexes D_700030F0[i] against a re-read count; the
//    second loop indexes all three tables by i (mwcc strength-reduces them
//    after the i >= 0 guard) and re-reads D_700030F0[i] after the calls.
//  - 0x700038AC = 1.0 is stored in each arm of the 3.0/0.5 offset choice; the
//    candidate height is held in a local for the height-difference test;
//    func_0019A180 returns short (its definition in src/); the sqrt result is
//    kept in 0x70003A2C before the 115.0 test.
//
// Aim/reposition solver. Gates on the actor's cover timers (e+0x220 > 0, and
// e+0x228 < 100 or D_008106F1 clear), then casts a short probe from the
// actor's matrix (func_001026A0 + func_0019AD00). If nothing is hit it fails.
// The hit surface must be of material class 0x20xx; a class-0x46 surface also
// needs the facing flag (func_0017D040 on the hit object). The turn toward the
// hit's angle (0x700031E4 vs e+0xC4) must exceed about 108 degrees; a second
// probe toward the target, if it hits, must be within 20 degrees. A cell
// lookup half a cell ahead (func_0019BC40) must find a node lower than the
// actor by more than 20.5. Then (3 cells back on AREA 0x11 inside a 115-unit
// circle around (340, 270), else half a cell back) the candidate nodes are
// walked from the last: flagged (bit 0), not a 0x46 class unless facing,
// angle band < 36 degrees, height within 2.8 of the actor and clear of the
// LOS checks (func_001776E0/func_00177CF0/func_00177B80/func_0019A570). The
// first valid node commits: e+0xB0/e+0xB8 = cell origin, e+0xC4 = facing back
// toward the enemy, e+0x290..0x298 = walk-target pose. Returns 1, else 0.
extern void func_001026A0(void *dst, void *src, void *xf);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern float func_0011DF78(float a);
extern float func_0011E748(float a);
extern int func_00128350(float a);
extern int func_001000C0(int a, double d);
extern int func_00177510(void);
extern int func_001776E0(void *e, float ang);
extern int func_00177B80(void *e, float y);
extern int func_00177CF0(void *e, float y);
extern int func_0017D040(int a);
extern short func_0019A180(int a, int idx);
extern int func_0019A570(void *dst, void *src, int n, int m);
extern int func_0019AD00(void *e, void *m, int n);
extern void func_0019BC40(void *m);
extern float func_001B1470(float a);

extern volatile float D_700038A0;
extern unsigned char *D_700031D0;
extern int D_700031D4;
extern float D_70003050;
extern float D_70003054;
extern float D_70003058;
extern float D_70003060;
extern float D_70003068;
extern float D_700031E4;
extern volatile float D_700038A4;
extern volatile float D_700038A8;
extern volatile float D_700038AC;
extern float D_700038B4;
extern float D_70003A20;
extern float D_70003A24;
extern float D_70003A28;
extern float D_70003A2C;
extern int D_700031E0;
extern float D_00282250[];
extern unsigned char D_008106F1;
extern unsigned char D_00810700;
extern float D_70003070;
extern float D_700030F0[];
extern unsigned short D_70003170[];
extern int D_700038B0;

int func_0017D080(unsigned char *e) {
    int found;
    int haveFacing;
    float y;
    int i;
    int n;

    found = 0;
    haveFacing = 0;
    if (*(float *)(e + 0x220) <= 0.0f ||
        (!(*(float *)(e + 0x228) < 100.0f) && D_008106F1 != 0)) {
        return 0;
    }

    D_700038A0 = 0.0f;
    D_700038A4 = 0.0f;
    D_700038A8 = -8.0f;
    D_700038AC = 1.0f;
    func_001026A0(&D_700038B0, e + 0xD0, &D_700038A0);
    if (func_0019AD00(e, &D_700038B0, 7) & 6) {
        if ((*(short *)(D_700031D0 + 0x1A) & 0xFF00) != 0x2000) {
            return 0;
        }
        if (D_700031D4 != 0 && func_0017D040(D_700031D4) != 0) {
            haveFacing = 1;
        }
        if (D_700031D0[0x1A] == 0x46 && haveFacing == 0) {
            return 0;
        }
        func_00177510();
        D_70003A20 = func_0011DF78(func_001B1470(D_700031E4 - *(float *)(e + 0xC4)));
        if (func_001000C0(func_00128350(D_70003A20), 1.884955644607544) != 0) {
            return 0;
        }
        D_700038A0 = 0.0f;
        D_700038A4 = 0.0f;
        D_700038A8 = -8.0f;
        D_700038AC = 0.0f;
        func_001026A0(&D_700038B0, &D_70003070, &D_700038A0);
        func_001028B8(&D_700038B0, &D_700038B0, e + 0xB0);
        if (func_0019AD00(e, &D_700038B0, 7) & 6) {
            D_70003A20 = D_700031E4;
            func_00177510();
            D_70003A24 = func_001B1470(D_70003A20 - D_700031E4);
            if (!(func_0011DF78(D_70003A24) <= 0.34906587f)) {
                return 0;
            }
        }
        D_700038A0 = D_70003050 + 0.5f * D_70003060;
        D_700038A4 = D_70003054;
        D_700038A8 = D_70003058 + 0.5f * D_70003068;
        D_700038AC = 1.0f;
        func_0019BC40(&D_700038A0);
        if (D_700031E0 != 0) {
            for (i = 0; i < D_700031E0; i++) {
                if (!(D_700030F0[i] < *(float *)(e + 0xB4) - 20.5f)) {
                    break;
                }
                found = 1;
            }
            if (found == 0) {
                return 0;
            }
        }
        found = 0;
        if (D_00810700 == 0x11) {
            D_70003A20 = *(float *)(e + 0xB0) - 340.0f;
            D_70003A28 = *(float *)(e + 0xB8) - 270.0f;
            D_70003A2C = func_0011E748(D_70003A20 * D_70003A20 + D_70003A28 * D_70003A28);
            if (D_70003A2C <= 115.0f) {
                found = 1;
            }
        }
        if (found != 0) {
            D_700038A0 = D_70003050 - 3.0f * D_70003060;
            D_700038A4 = D_70003054;
            D_700038A8 = D_70003058 - 3.0f * D_70003068;
            D_700038AC = 1.0f;
        } else {
            D_700038A0 = D_70003050 - 0.5f * D_70003060;
            D_700038A4 = D_70003054;
            D_700038A8 = D_70003058 - 0.5f * D_70003068;
            D_700038AC = 1.0f;
        }
        func_0019BC40(&D_700038A0);
        n = D_700031E0;
        if (n != 0) {
            for (i = n - 1; i >= 0; i--) {
                if (!(D_70003170[i] & 1)) {
                    continue;
                }
                if (haveFacing == 0 && (func_0019A180(0, i) & 0xFF) == 0x46) {
                    continue;
                }
                if (!(D_00282250[i] < 0.62831855f)) {
                    continue;
                }
                y = D_700030F0[i];
                if (func_0011DF78(y - *(float *)(e + 0xB4)) <= 2.8f && func_001776E0(e, D_700030F0[i]) == 0) {
                    if (func_00177CF0(e, D_70003054) != 0) {
                        return 0;
                    }
                    if (func_00177B80(e, D_700030F0[i]) != 0) {
                        return 0;
                    }
                    D_700038A0 = D_70003050 + 1.5f * D_70003060;
                    D_700038A8 = D_70003058 + 1.5f * D_70003068;
                    D_700038A4 = D_700030F0[i];
                    func_00102948(&D_700038B0, &D_700038A0);
                    D_700038B4 = D_700038B4 - 20.5f;
                    if (func_0019A570(&D_700038A0, &D_700038B0, 6, 0) != 0) {
                        return 0;
                    }
                    *(float *)(e + 0xB0) = D_70003050;
                    *(float *)(e + 0xB8) = D_70003058;
                    *(float *)(e + 0xC4) = func_001B1470(D_700031E4 - 3.1415927f);
                    *(float *)(e + 0x290) = D_70003050 + 1.5f * D_70003060;
                    *(float *)(e + 0x298) = D_70003058 + 1.5f * D_70003068;
                    *(float *)(e + 0x294) = D_700030F0[i];
                    return 1;
                }
            }
        }
    }
    return 0;
}
