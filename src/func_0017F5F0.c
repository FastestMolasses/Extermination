// NEARMISS func_0017F5F0  (vram 0x0017F5F0, 0x3E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP odd/even register-coloring on two timer-expiry float add/sub ops (target picks a different-numbered FP reg than mwcc for the same live value, a function-wide register-pressure artifact) plus a cosmetic prologue saved-reg move-order swap; both confirmed genuine after exhausting operand-order/ma...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 99.752% (mwcc 2.3.3; pinned 991202 build 82.9%). Logic fully recovered.
//
// Entity turn/aim-blend state machine. arg0 is the entity, arg1 is a
// 'skip-anim-clip' flag. First block: state = arg0[0x24C] selects a turn
// blend (0=turn-toward via D_00248790 table +cos speed, 1=turn-away via
// (1-table)*cos, default=snap using func_0011E2A8 heading trig), each firing
// anim_clip via func_001749A0(e, clipId, 0, blendSeconds) unless arg1 skips
// it, then ramps arg0[0x38] over a 30-frame timer (arg0[0x2F8]/arg0[0x2E4])
// and clamps it to 1.5f. Second block: arg0[0x24C]==2/3 rotates the aim
// angle arg0[0xC4] toward a computed target using func_001B1470 (angle
// normalize) + func_001B12B0 (angle lerp, rate 0.06981317f), tracking a
// left/right turn flag at arg0[0x25D]; default case resets the turn flag and
// lerps back to the base heading arg0[0x218].
//
// WALL: body/structure/signature/all constants fully recovered (including
// fixing the true func_001749A0(e,clip,flags,blend) signature and the two
// blend-seconds float constants 8.0f/12.0f, misread initially as 1.0f/3.0f).
// Sole residual: FP odd/even register-coloring on two float add/sub ops
// inside the timer-expiry branches (target colors a constant load to a
// DIFFERENT numbered FP reg -- f1 vs f2 -- than mwcc, purely a function-wide
// register-pressure artifact; confirmed genuine by exhausting float-local,
// operand-order and materialization variants without effect) plus a cosmetic
// prologue saved-reg move-order swap (paddub s0,a1 / s1,a0 emitted in the
// opposite order; registers themselves already match). No further C-source
// lever found within the effort cap.
extern float func_0011E2A8(float a);
extern int func_001749A0(int e, short clip, int flags, float blend);
extern float func_001B1470(float a);
extern float func_001B12B0(float a, float b, float rate);
extern float D_00248790[];
extern float D_002487A0[];
extern void func_00174FD0(void);

void func_0017F5F0(int arg0, int arg1) {
    int state;
    int flag;
    flag = arg1;
    func_00174FD0();
    state = *(int *)(arg0 + 0x24C);

    if (state == 0) {
        if (flag == 0) {
            func_001749A0(arg0, 0x61, 0, 8.0f);
        }
        *(float *)0x70003A20 = 1.0f + D_00248790[*(unsigned char *)(arg0 + 0x23F)];
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) + *(float *)0x70003A20 * (0.01f * func_0011E2A8(*(float *)(arg0 + 0x9C)));
        if (*(unsigned char *)(arg0 + 0x1F1) != 1) {
            *(float *)(arg0 + 0x2F8) = 30.0f;
            *(float *)(arg0 + 0x2E4) = (*(float *)(arg0 + 0x38) * *(float *)0x70003A20) / *(float *)(arg0 + 0x2F8);
            *(unsigned char *)(arg0 + 0x1F1) = 1;
        } else {
            float zero = 0.0f;
            float t = *(float *)(arg0 + 0x2F8);
            *(float *)(arg0 + 0x2F8) = t - 1.0f;
            if (t == zero) {
                float d = *(float *)(arg0 + 0x2E4);
                *(float *)(arg0 + 0x38) = d + *(float *)(arg0 + 0x38);
            }
        }
    } else if (state == 1) {
        if (flag == 0) {
            func_001749A0(arg0, 0x62, 0, 8.0f);
        }
        *(float *)0x70003A20 = 1.0f - D_00248790[*(unsigned char *)(arg0 + 0x23F)];
        if (*(unsigned char *)(arg0 + 0x1F1) != 2) {
            *(float *)(arg0 + 0x2F8) = 30.0f;
            *(float *)(arg0 + 0x2E4) = (*(float *)(arg0 + 0x38) * *(float *)0x70003A20) / *(float *)(arg0 + 0x2F8);
            *(unsigned char *)(arg0 + 0x1F1) = 2;
        } else {
            float zero = 0.0f;
            float t = *(float *)(arg0 + 0x2F8);
            *(float *)(arg0 + 0x2F8) = t - 1.0f;
            if (t == zero) {
                float d = *(float *)(arg0 + 0x2E4);
                *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - d;
            }
        }
    } else {
        *(unsigned char *)(arg0 + 0x1F1) = 0;
        if (flag == 0) {
            func_001749A0(arg0, 0x5F, 0, 8.0f);
        }
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) + 0.01f * func_0011E2A8(*(float *)(arg0 + 0x9C));
    }

    if (!(*(float *)(arg0 + 0x38) <= 1.5f)) {
        *(float *)(arg0 + 0x38) = 1.5f;
    }

    state = *(int *)(arg0 + 0x24C);
    if (state == 2) {
        *(unsigned char *)(arg0 + 0x25D) = 1;
        if (flag == 0) {
            func_001749A0(arg0, 0x63, 0, 8.0f);
        }
        {
            int idx = *(unsigned char *)(arg0 + 0x23F);
            float ang = *(float *)(arg0 + 0x218) - (3.1415927f * D_002487A0[idx]) / 180.0f;
            *(float *)0x70003A24 = ang;
            *(float *)(arg0 + 0xC4) = func_001B12B0(func_001B1470(ang), *(float *)(arg0 + 0xC4), 0.06981317f);
        }
    } else if (state == 3) {
        *(unsigned char *)(arg0 + 0x25D) = 2;
        if (flag == 0) {
            func_001749A0(arg0, 0x64, 0, 8.0f);
        }
        {
            int idx = *(unsigned char *)(arg0 + 0x23F);
            float ang = *(float *)(arg0 + 0x218) + (3.1415927f * D_002487A0[idx]) / 180.0f;
            *(float *)0x70003A24 = ang;
            *(float *)(arg0 + 0xC4) = func_001B12B0(func_001B1470(ang), *(float *)(arg0 + 0xC4), 0.06981317f);
        }
    } else {
        unsigned char turn = *(unsigned char *)(arg0 + 0x25D);
        if (((turn == 1) || (turn == 2)) && (flag == 0)) {
            func_001749A0(arg0, 0x5F, 0, 12.0f);
        }
        *(unsigned char *)(arg0 + 0x25D) = 0;
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), 0.06981317f);
    }
}
