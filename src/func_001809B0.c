// NEARMISS func_001809B0  (vram 0x001809B0, 0x760 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Permuter-class register-coloring + FP-arg scheduling on a fully-recovered body. Residual clusters: (1) func_00180600(11.5f,10.0f,-1.5f) three-immediate FP arg order -- target materializes f13(10.0) then f14(-1.5) then f12(11.5); both mwcc builds emit f12,f13,f14 and the assign-in-arg idiom cannot...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Enemy melee/contact-attack resolver: arg0 = the attacker entity, arg1 = a 0/1 facing/side flag. Gated by
// func_00180600 (a global cooldown/can-attack check). Picks a vertical reach 'ydist' (12/10/9 by the entity type byte
// at arg0+0xD and the +0xB4 distance) and a forward const 'xc' (16 or 19 by the weapon byte at arg0+5), builds two
// oriented hit-volumes at the global blocks D_700038C0/D_700038D0 from a box described at 0x700038A0/0x700038B0 (via
// func_001026A0 transform-by-matrix at arg0+0xD0), and sweeps them with func_0019AFE0 (overlap test, mask &6). The
// victim type byte lives at *(0x700031D0)+0x1A. First sweep: type 0x3D + func_00178390 -> result 3. Second sweep
// (a longer 12.0/-2.0 lunge box, via func_0019A570): on type 0x34 it commits a dash -- snaps the entity back to its
// saved B0/B8, computes the lunge distance dist = sqrt-ish func_0011E748(dx*dx+dy*dy) where dx/dy = box-vs-entity at
// 0x70003A20/0x70003A28, quantizes it to 4.5-unit steps (step = 4.5*(0.5 + (float)(int)(dist/4.5 -/+1))), and writes
// the dash target arg0+0x290/0x298 = box origin -/+ step*(sin/cos of heading arg0+0xC4) with sign by arg1, plus a
// timer arg0+0x294 = *0x700031B4 - 20.5 -> result 7. Third sweep (a 9.0/-5.0 box): dispatches on the victim type --
// 0x32 -> grapple, set arg0+0xD=0, result 1; 0x3B -> grapple, set arg0+0xD=1, result 1; 0x3D -> no-op; default ->
// func_00178080 hit, and on the special global pair (D_00810700==8 && D_00810701==3) a positional bonus check on
// arg0+0x2E0/0x2E4 (120<a<130 && 250<b<260) yields result 5 (store 0x431C6666 at +0x2E8) else result 2; otherwise
// snapshots +0x2E0..+0x2E8 to the stack and func_0017E250 -> result 2 on fail.
//
// WALL: permuter-class register-coloring + FP-arg scheduling. Residuals: func_00180600's three-immediate FP arg
// order (f13,f14,f12 vs f12,f13,f14; not flippable by the assign-in-arg idiom on three lui/ori immediates);
// constant-materialization v0-vs-v1 coloring rippling through every 0x700038A0.. store group; and the early
// return-value lands in s0 (via the unified return var) where the target keeps v0. Not the clean-store nop, not
// idiom-fixable -> parked.
extern int float_to_int(float value);
extern int func_00180600(float a, float b, float c);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001031E0(void *a0, void *a1);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern float func_0011E748(float x);
extern void func_00177030(char *p, int n);
extern int func_00178080(char *p);
extern int func_001782A0(char *p);
extern int func_00178390(char *p);
extern int func_0017E250(char *p, float *q);
extern void func_00199DB0(void *p);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_0019AFE0(char *p, void *a, void *b, int n);
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;

int func_001809B0(char *arg0, int arg1) {
    int r = 0;
    float ydist;
    float xc;

    if (func_00180600(11.5f, 10.0f, -1.5f) != 0) {
        goto end;
    }
    if (*(unsigned char *)(arg0 + 0xD) == 2) {
        if (*(float *)(arg0 + 0xB4) < 560.0f) {
            ydist = 12.0f;
        } else {
            ydist = 10.0f;
        }
    } else {
        ydist = 9.0f;
    }
    if (*(unsigned char *)(arg0 + 5) == 0xC) {
        xc = 16.0f;
    } else {
        xc = 19.0f;
    }
    if (arg1 == 0) {
        *(float *)0x700038A0 = -ydist;
        *(float *)0x700038B0 = -ydist;
        *(float *)0x700038A4 = xc;
        *(float *)0x700038B4 = xc;
        *(float *)0x700038A8 = -6.0f;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B8 = 0x40800000;
    } else {
        *(float *)0x700038A0 = ydist;
        *(float *)0x700038A4 = xc;
        *(float *)0x700038B0 = ydist;
        *(float *)0x700038B4 = xc;
        *(float *)0x700038A8 = -6.0f;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B8 = 0x40800000;
    }
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(D_700038C0, arg0 + 0xD0, D_700038A0);
    func_001026A0(D_700038D0, arg0 + 0xD0, D_700038B0);
    if ((func_0019AFE0(arg0, D_700038C0, D_700038D0, 7) & 6) && *(unsigned char *)(*(int *)0x700031D0 + 0x1A) == 0x3D && func_00178390(arg0) != 0) {
        r = 3;
        goto end;
    }
    if (arg1 == 0) {
        *(float *)0x700038A0 = 2.0f * -ydist;
        *(int *)0x700038A4 = 0x41400000;
        *(float *)0x700038A8 = -2.0f;
    } else {
        *(float *)0x700038A0 = 2.0f * ydist;
        *(int *)0x700038A4 = 0x41400000;
        *(float *)0x700038A8 = -2.0f;
    }
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
    func_001031E0(D_700038C0, D_700038B0);
    *(float *)0x700038C4 = *(float *)0x700038C4 + 10.0f;
    if (func_0019A570(D_700038B0, D_700038C0, 4, 0) != 0 && *(unsigned char *)(*(int *)0x700031D0 + 0x1A) == 0x34) {
        float sx = *(float *)(arg0 + 0xB0);
        float sy = *(float *)(arg0 + 0xB8);
        float dist, step;
        func_00177030(arg0, 2);
        func_00199DB0(D_700038A0);
        *(float *)0x70003A20 = *(float *)0x700038A0 - *(float *)(arg0 + 0xB0);
        *(float *)0x70003A28 = *(float *)0x700038A8 - *(float *)(arg0 + 0xB8);
        dist = func_0011E748(*(float *)0x70003A20 * *(float *)0x70003A20 + *(float *)0x70003A28 * *(float *)0x70003A28);
        *(float *)0x70003A2C = dist;
        if (arg1 == 1) {
            step = 4.5f * (0.5f + (float)(float_to_int(*(float *)0x70003A2C / 4.5f) - 1));
            *(float *)(arg0 + 0x290) = *(float *)0x700038A0 + step * func_0011E2A8(*(float *)(arg0 + 0xC4));
            *(float *)(arg0 + 0x298) = *(float *)0x700038A8 + step * func_0011DE90(*(float *)(arg0 + 0xC4));
        } else {
            step = 4.5f * (0.5f + (float)(float_to_int(*(float *)0x70003A2C / 4.5f) + 1));
            *(float *)(arg0 + 0x290) = *(float *)0x700038A0 - step * func_0011E2A8(*(float *)(arg0 + 0xC4));
            *(float *)(arg0 + 0x298) = *(float *)0x700038A8 - step * func_0011DE90(*(float *)(arg0 + 0xC4));
        }
        *(float *)(arg0 + 0xB0) = sx;
        *(float *)(arg0 + 0xB8) = sy;
        *(float *)(arg0 + 0x294) = *(float *)0x700031B4 - 20.5f;
        r = 7;
        goto end;
    }
    if (arg1 == 0) {
        *(float *)0x700038A0 = -ydist;
        *(float *)0x700038B0 = -ydist;
        *(int *)0x700038A4 = 0x41100000;
        *(int *)0x700038B4 = 0x41100000;
        *(float *)0x700038A8 = -5.0f;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B8 = 0x40A00000;
    } else {
        *(float *)0x700038A0 = ydist;
        *(float *)0x700038B0 = ydist;
        *(int *)0x700038A4 = 0x41100000;
        *(int *)0x700038B4 = 0x41100000;
        *(float *)0x700038A8 = -5.0f;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B8 = 0x40A00000;
    }
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(D_700038C0, arg0 + 0xD0, D_700038A0);
    func_001026A0(D_700038D0, arg0 + 0xD0, D_700038B0);
    if (func_0019AFE0(arg0, D_700038C0, D_700038D0, 7) & 6) {
        switch (*(unsigned char *)(*(int *)0x700031D0 + 0x1A)) {
        case 0x3D:
            break;
        case 0x32:
            if (func_001782A0(arg0) != 0) {
                r = 1;
                *(unsigned char *)(arg0 + 0xD) = 0;
            }
            break;
        case 0x3B:
            if (func_001782A0(arg0) != 0) {
                r = 1;
                *(unsigned char *)(arg0 + 0xD) = 1;
            }
            break;
        default:
            if (func_00178080(arg0) != 0) {
                if (D_00810700 == 8 && D_00810701 == 3) {
                    float a = *(float *)(arg0 + 0x2E0);
                    r = 2;
                    if (a > 120.0f) {
                        if (a < 130.0f) {
                            float b = *(float *)(arg0 + 0x2E4);
                            if (b > 250.0f && b < 260.0f) {
                                r = 5;
                                *(int *)(arg0 + 0x2E8) = 0x431C6666;
                            } else {
                                r = 2;
                            }
                        } else {
                            r = 2;
                        }
                    }
                } else {
                    float sp50 = *(float *)(arg0 + 0x2E0);
                    float sp54 = *(float *)(arg0 + 0x2E4);
                    int sp58 = *(int *)(arg0 + 0x2E8);
                    int sp5C = 0x3F800000;
                    (void)sp54; (void)sp58; (void)sp5C;
                    if (func_0017E250(arg0, &sp50) == 0) {
                        r = 2;
                    }
                }
            }
            break;
        }
    }
end:
    return r;
}
