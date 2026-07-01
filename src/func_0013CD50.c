// NEARMISS func_0013CD50  (vram 0x0013CD50, 0x4C8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation. Both mwcc builds place the exact same four live values (three per-slot scan pointers D_700030F0/D_70003170/D_00282250, plus the loop index) in the same four callee-saved registers s0-s3 but in a different assignment ORDER than the target (target: s0=tpos,s1=...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019AFE0(void *p, void *a, void *b, int n);
extern void func_0019BC40(void *p);
extern float func_0011DF78(float a);
extern float func_001B1470(float a);
extern void func_001029C0(void *a);
extern void func_00102C58(void *a, void *b, void *c);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);

extern float D_00282250[];
extern float D_700030F0[];
extern unsigned short D_70003170[];
extern float D_700031B0;
extern float D_700036A0;
extern float D_700038B0;

int func_0013CD50(char *arg0, char *arg1) {
    float *tpos;
    unsigned short *flags;
    float *tsin;
    float lo;
    float hi;
    int i;
    int cnt;
    int mask;
    int r;

    r = func_0019AFE0(arg0, arg0 + 0xB0, arg1, 6);
    if (r == 0) {
        return 0;
    }
    if ((r & 2) && *(int *)0x700031D4 == 0) {
        return 3;
    }
    if (r & 1) {
        return 3;
    }
    func_0019BC40(arg1);
    if (*(int *)0x700031E0 == 0) {
        return 3;
    }

    lo = *(float *)(arg1 + 4) - 1000.0f;
    tpos = D_700030F0;
    flags = D_70003170;
    hi = 1000.0f + *(float *)(arg1 + 4);
    tsin = D_00282250;
    i = 0;

    for (; i < *(int *)0x700031E0; i++, tpos += 1, flags += 1, tsin += 1) {
        if (!(*(float *)(arg1 + 4) < *tpos)) {
            if (!(*flags & 1) && func_0011DF78(*tsin) <= 1.0471976f) {
                lo = *tpos;
            }
            continue;
        }
        if ((D_70003170[i] & 1) && func_0011DF78(D_00282250[i]) <= 1.0471976f) {
            hi = D_700030F0[i];
        }
        break;
    }

    *(int *)0x700038B0 = 0;
    *(float *)0x700038B4 = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
    *(int *)0x700038B8 = 0;
    *(int *)0x700038BC = 0x3F800000;
    func_001029C0(&D_700036A0);
    func_00102C58(&D_700036A0, &D_700036A0, &D_700038B0);
    *(int *)0x700038B0 = 0;
    *(float *)0x700038B4 = 0.0f;
    *(int *)0x700038B8 = 0x3DCCCCCD;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038B0, &D_700036A0, &D_700038B0);
    func_001028B8(&D_700038B0, &D_700038B0, &D_700031B0);
    func_0019BC40(&D_700038B0);

    mask = 0;
    cnt = *(int *)0x700031E0;
    if (cnt != 0) {
        tpos = D_700030F0;
        for (i = 0; i < cnt; i++, tpos += 1) {
            float d = *tpos;
            if (d <= *(float *)(arg1 + 4)) {
                if (!(d <= lo - 5.0f)) {
                    mask |= 1;
                }
            } else if (d < 5.0f + hi) {
                mask |= 2;
            }
        }
    }

    switch (mask) {
    case 0:
        if (func_0011DF78(hi - *(float *)(arg1 + 4)) <= 25.0f && func_0011DF78(*(float *)(arg1 + 4) - lo) <= 25.0f) {
            if (func_0011DF78(hi - *(float *)(arg1 + 4)) <= func_0011DF78(*(float *)(arg1 + 4) - lo)) {
                return 1;
            }
            return 2;
        }
        if (func_0011DF78(hi - *(float *)(arg1 + 4)) <= 25.0f) {
            return 1;
        }
        if (func_0011DF78(*(float *)(arg1 + 4) - lo) <= 25.0f) {
            return 2;
        }
        return 1;
    case 1:
        if (func_0011DF78(hi - *(float *)(arg1 + 4)) <= 25.0f) {
            return 1;
        }
        return 3;
    case 2:
        if (func_0011DF78(*(float *)(arg1 + 4) - lo) <= 25.0f) {
            return 2;
        }
        return 3;
    default:
        return 3;
    }
}
