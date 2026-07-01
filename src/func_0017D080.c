// NEARMISS func_0017D080  (vram 0x0017D080, 0x780 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring / parameter-register-choice permutation (a0-vs-a1 for a loop bound) plus instruction-scheduling permutation across 3 near-identical repeated cell-position blocks (0x70003050.. math with different scale constants 0.5/1.5/3.0), stacked with the same D_00275B40/D_70003070 gp-rel-at...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Aim/reposition solver: given the actor's cover-transition timers (e+0x220,
// e+0x228), builds a probe vector toward the enemy (via func_0019AD00 casting
// a short ray) and checks the hit surface's material class (0x2000-masked type
// byte). If the hit is a valid ledge/step (func_0017D040) or a specific facing
// class (0x46), it computes the delta angle to the enemy's aim field (e+0x31E4
// via a hardware-scratch snapshot) and rejects if the required turn exceeds a
// threshold. It then samples up to 6 evenly spaced candidate stand points
// (D_00286340-style table indexed via D_00275B40 lists, D_70003170 bit-flags,
// D_00282250 angle-band and D_700030F0 y-height arrays), rejecting occupied
// nodes, out-of-arc nodes, and nodes too close to a current LOS obstruction
// (func_001776E0/func_00177CF0/func_00177B80), and commits the first valid
// node: sets e+0xB0/e+0xB8 to the node's ground position, sets e+0xC4 to the
// facing angle back toward the enemy, and stashes the walk-target pose at
// e+0x290/e+0x294/e+0x298. Returns 1 on success, 0 otherwise.
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
extern long long func_0019A180(int a, int idx);
extern int func_0019A570(void *dst, void *src, int n, int m);
extern int func_0019AD00(void *e, void *m, int n);
extern void func_0019BC40(void *m);
extern float func_001B1470(float a);

extern float D_00282250[];
extern unsigned char D_008106F1;
extern unsigned char D_00810700;
extern float D_70003070;
extern float D_700030F0[];
extern unsigned short D_70003170[];
extern int D_700038A0;
extern int D_700038B0;

int func_0017D080(unsigned char *e) {
    int found;
    int haveFacing;
    int i;
    int n;
    int idx;
    unsigned short *flags;
    float *band;
    float *ypos;

    found = 0;
    haveFacing = 0;
    if (*(float *)(e + 0x220) <= 0.0f ||
        (!(*(float *)(e + 0x228) < 100.0f) && D_008106F1 != 0)) {
        return 0;
    }

    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038A8 = -8.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038B0, e + 0xD0, &D_700038A0);
    if (func_0019AD00(e, &D_700038B0, 7) & 6) {
        if ((*(short *)(*(int *)0x700031D0 + 0x1A) & 0xFF00) != 0x2000) {
            return 0;
        }
        if (*(int *)0x700031D4 != 0 && func_0017D040(*(int *)0x700031D4) != 0) {
            haveFacing = 1;
        }
        if ((unsigned char)*(short *)(*(int *)0x700031D0 + 0x1A) == 0x46 && haveFacing == 0) {
            return 0;
        }
        func_00177510();
        *(float *)0x70003A20 = func_0011DF78(func_001B1470(*(float *)0x700031E4 - *(float *)(e + 0xC4)));
        if (func_001000C0(func_00128350(*(float *)0x70003A20), 1.8849556f)) {
            return 0;
        }
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0xC1000000;
        *(int *)0x700038AC = 0;
        func_001026A0(&D_700038B0, &D_70003070, &D_700038A0);
        func_001028B8(&D_700038B0, &D_700038B0, e + 0xB0);
        if (func_0019AD00(e, &D_700038B0, 7) & 6) {
            *(float *)0x70003A20 = *(float *)0x700031E4;
            func_00177510();
            *(float *)0x70003A24 = func_001B1470(*(float *)0x70003A20 - *(float *)0x700031E4);
            if (!(func_0011DF78(*(float *)0x70003A24) <= 0.34906587f)) {
                return 0;
            }
        }
        *(float *)0x700038A0 = *(float *)0x70003050 + 0.5f * *(float *)0x70003060;
        *(float *)0x700038A4 = *(float *)0x70003054;
        *(float *)0x700038A8 = *(float *)0x70003058 + 0.5f * *(float *)0x70003068;
        *(int *)0x700038AC = 0x3F800000;
        func_0019BC40(&D_700038A0);
        n = *(int *)0x700031E0;
        if (n != 0) {
            ypos = &D_700030F0[0];
            i = 0;
            while (i < n) {
                if (!(*ypos < *(float *)(e + 0xB4) - 20.5f)) {
                    break;
                }
                found = 1;
                ypos += 1;
                i += 1;
            }
            if (found == 0) {
                return 0;
            }
        }
    }

    idx = 0;
    if (D_00810700 == 0x11) {
        *(float *)0x70003A20 = *(float *)(e + 0xB0) - 340.0f;
        *(float *)0x70003A28 = *(float *)(e + 0xB8) - 270.0f;
        if (func_0011E748(*(float *)0x70003A28 * *(float *)0x70003A28 + *(float *)0x70003A20 * *(float *)0x70003A20) <= 115.0f) {
            idx = 1;
        }
    }
    if (idx != 0) {
        *(float *)0x700038A0 = *(float *)0x70003050 - 3.0f * *(float *)0x70003060;
        *(float *)0x700038A4 = *(float *)0x70003054;
        *(float *)0x700038A8 = *(float *)0x70003058 - 3.0f * *(float *)0x70003068;
    } else {
        *(float *)0x700038A0 = *(float *)0x70003050 - 0.5f * *(float *)0x70003060;
        *(float *)0x700038A4 = *(float *)0x70003054;
        *(float *)0x700038A8 = *(float *)0x70003058 - 0.5f * *(float *)0x70003068;
    }
    *(int *)0x700038AC = 0x3F800000;
    func_0019BC40(&D_700038A0);
    n = *(int *)0x700031E0;
    if (n == 0) {
        return 0;
    }
    i = n - 1;
    if (i < 0) {
        return 0;
    }
    flags = &D_70003170[i];
    band = &D_00282250[i];
    ypos = &D_700030F0[i];
    for (;;) {
        if (*flags & 1) {
            if (haveFacing == 0) {
                if ((int)(func_0019A180(0, i) & 0xFF) != 0x46) {
                    goto next;
                }
                goto out;
            }
        } else {
            goto next;
        }
        if (*band < 0.62831855f && func_0011DF78(*ypos - *(float *)(e + 0xB4)) <= 2.8f
            && func_001776E0(e, *ypos) == 0) {
            if (func_00177CF0(e, *(float *)0x70003054) != 0) {
                return 0;
            }
            if (func_00177B80(e, *ypos) != 0) {
                return 0;
            }
            *(float *)0x700038A0 = *(float *)0x70003050 + 1.5f * *(float *)0x70003060;
            *(float *)0x700038A8 = *(float *)0x70003058 + 1.5f * *(float *)0x70003068;
            *(float *)0x700038A4 = *ypos;
            func_00102948(&D_700038B0, &D_700038A0);
            *(float *)0x700038B4 = *(float *)0x700038B4 - 20.5f;
            if (func_0019A570(&D_700038A0, &D_700038B0, 6, 0) != 0) {
                return 0;
            }
            *(float *)(e + 0xB0) = *(float *)0x70003050;
            *(float *)(e + 0xB8) = *(float *)0x70003058;
            *(float *)(e + 0xC4) = func_001B1470(*(float *)0x700031E4 - 3.1415927f);
            *(float *)(e + 0x290) = *(float *)0x70003050 + 1.5f * *(float *)0x70003060;
            *(float *)(e + 0x298) = *(float *)0x70003058 + 1.5f * *(float *)0x70003068;
            *(float *)(e + 0x294) = *ypos;
            return 1;
        }
        goto next;
    out:
    next:
        i -= 1;
        flags -= 1;
        band -= 1;
        ypos -= 1;
        if (i < 0) {
            return 0;
        }
    }
}
