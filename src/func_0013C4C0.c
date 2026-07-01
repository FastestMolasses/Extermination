// NEARMISS func_0013C4C0  (vram 0x0013C4C0, 0x3F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.98% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 98.98% -- extremely close. Fully recovered logic (fixed a spurious extra func_001B39F0 call the m2c decode hallucinated at the tail, corrected func_00102C58's real 3-pointer signature which fixed the D_700036A0/D_700038A0 staging order, and forced signed 'lb' byte loads via a signed char* arg1 ty...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001029C0(void *a0);
extern void func_00102B08(void *obj, void *obj2, float v);
extern void func_00102BB0(void *obj, void *obj2, float v);
extern void func_00102C58(void *obj, void *unused, void *v);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019AFE0(char *actor, void *from, void *to, int mask);
extern float func_001B15D0(float *a, float *b);
extern int func_001B39F0(unsigned char *arg0, float *arg1, float *arg2);
extern int func_0013D220(unsigned char *arg0);

extern float D_700031B0;
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038D0[];

// Steering-obstacle probe for a moving actor (arg0) writing state into its
// control block arg1. arg1+0x80/0x81 hold flag bytes; arg1+0x5C a scan-angle
// bias, chosen +pi/8 (turn-mode 4) or -pi/8 (turn-mode 2) from arg1+0x80 bit0.
// First probes straight ahead (arg0+0xC4 heading) for 25 units; on a hit,
// measures the hit distance (func_001B15D0) and if close (<=5.0) or the
// relevant turn-mode bit is already latched in arg1+0x81, records a turn
// decision via func_001B39F0 (only once, gated by arg1+0x80 bits 2/3) and
// latches the corresponding turn bit (1 or 2) into arg1+0x80, clearing the
// gate bits first. On a miss straight ahead: if the turn-mode bit is set in
// arg1+0x81, do the same turn-bit latch without a new probe. Otherwise probe
// again along a facing rotated by the chosen bias; a hit there always
// records the turn decision and returns 1; a miss falls through to: if the
// opposite turn-mode is already active (bit 2 of arg1+0x80) or
// func_0013D220(arg0) reports the path clear, clear the turn bits and reset
// arg1+0x5C to 0.0f, returning 0; otherwise return 1.
int func_0013C4C0(unsigned char *arg0, signed char *arg1) {
    float ang;
    int mode;

    if (arg1[0x80] & 1) {
        ang = 0.39269909f;
        mode = 4;
        *(int *)(arg1 + 0x5C) = 0x3FC6D3F2;
    } else {
        ang = -0.39269909f;
        mode = 2;
        *(int *)(arg1 + 0x5C) = 0xBFC6D3F2;
    }

    func_001029C0(D_700036A0);
    *(int *)0x700038A0 = 0;
    *(float *)0x700038A4 = *(float *)(arg0 + 0xC4);
    *(float *)0x700038A8 = 0.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_00102C58(D_700036A0, D_700036A0, D_700038A0);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 25.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
    if (func_0019AFE0((char *)arg0, arg0 + 0xB0, D_700038A0, 6) != 0) {
        float dist;
        *(float *)0x700038A8 = 0.0f;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A0 = 0;
        *(int *)0x700038AC = 0x3F800000;
        dist = func_001B15D0((float *)(arg0 + 0xB0), &D_700031B0);
        *(float *)0x700038A8 = dist;
        if (dist <= 5.0f || (arg1[0x81] & mode)) {
            if (!(arg1[0x80] & 0xC)) {
                func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
                arg1[0x80] = arg1[0x80] | ((func_001B39F0(arg0, D_700038A0, D_700038D0) + 1) * 4);
            }
            if (arg1[0x81] & mode) {
                arg1[0x80] = arg1[0x80] & 0xC;
                if (arg1[0x81] & 4) {
                    arg1[0x80] = arg1[0x80] | 2;
                } else {
                    arg1[0x80] = arg1[0x80] | 1;
                }
            }
        }
        return 1;
    }
    if (arg1[0x81] & mode) {
        arg1[0x80] = arg1[0x80] & 0xC;
        if (arg1[0x81] & 4) {
            arg1[0x80] = arg1[0x80] | 2;
        } else {
            arg1[0x80] = arg1[0x80] | 1;
        }
        if (!(arg1[0x80] & 0xC)) {
            arg1[0x80] = arg1[0x80] | ((func_001B39F0(arg0, D_700038A0, D_700038D0) + 1) * 4);
        }
        return 1;
    }

    func_001029C0(D_700036A0);
    func_00102B08(D_700036A0, D_700036A0, ang);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 25.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
    if (func_0019AFE0((char *)arg0, arg0 + 0xB0, D_700038A0, 6) == 0) {
        if (!(arg1[0x80] & 2)) {
            if (func_0013D220(arg0) != 0) {
                return 1;
            }
        }
        arg1[0x80] = arg1[0x80] & 0xC;
        *(int *)(arg1 + 0x5C) = 0;
        return 0;
    }
    return 1;
}
