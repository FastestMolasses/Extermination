// NEARMISS func_00169250  (vram 0x00169250, 0x450 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.12% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FPU register-coloring/scheduling wall in the two func_0011E748 (dx*dx+dz*dz distance) MAC blocks and the even/odd interpolation-step math; also a residual GP dispatch-register choice (a0 vs a1 for the state byte) in the switch prologue. Body/structure fully recovered including the case-0->case-1 ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-state animation-turn/interp driver dispatched on the state byte at
// arg0+6, sibling of func_0016B8A0 (same shape, different constants/states).
// State 0: bump state, clear arg0+7; if the two globals D_00810700/
// D_00810701 read {0x13,0} call func_00194240(old-state); falls through
// into state 1's body (case 0 has no separate work of its own -- the whole
// interpolation setup below is state 1's, run once immediately after the
// state-0 bump). State 1: step arg0[0xC4] toward arg0[0x218] with
// func_001B12B0(goal,cur,0.3927f); once reached, bump state, kick clip 0x70
// at 1.0f, set arg0[0x28] (s16) = 8 (frame count), then build an
// interpolation step: two scratch anim vectors are fetched via
// func_0019F680(&D_700038A0/&D_700038B0, arg0[0x30C], 0/1), their planar
// delta length (func_0011E748) plus 0.1 divided by 4.5 gives the sub-step
// count n; even n lerps the (0x290,0x298) target toward the current pos by
// n*4.5*speed; odd n also removes a half-step (2.25f) via the msub
// identity. State 2 (0xBA): gated on flag 0x1000 in arg0+0x200 -- bump
// state, kick clip 0xBA at 1.0f, arg0[0x25F]=3. State 3 (0x123/300): once
// arg0[0x28]==0, snapshot pos into arg0+0xB4/0xB0/0xB8 and kick clip 0x123
// at 300.0f; else accumulate arg0+0x2E4/0x2E0/0x2E8 into 0xB4/0xB0/0xB8 and
// decrement arg0[0x28]. State 4: gated on flag 0x1000 in arg0+0x200 --
// reset arg0[5]/[6]/[0x1F0]/[0x1F1]/[0x2F1] and kick
// func_001749A0(arg0, func_001885B0(arg0), 0, 1.0f).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// fills the case-0 beql delay slot differently (idiom-13/20 clean-store).
extern void func_001749A0(char *, int, int, float);
extern int func_001885B0(char *);
extern float func_001B12B0(float, float, float);
extern void func_0019F680(void *, int, int);
extern float func_0011E748(float);
extern int float_to_int(float);
extern void func_001FBD50(void *, int, int, float);
extern void func_00194240(int);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038A8;
extern float D_700038B8;

void func_00169250(char *arg0) {
    unsigned char st = *(unsigned char *)(arg0 + 6);
    float d;
    float dx, dz, dist1, dist2;
    int n;

    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg0 + 7) = 0;
        if (D_00810700 == 0x13 && D_00810701 == 0) {
            func_00194240(st);
        }
        /* fallthrough */
    case 1:
        d = func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), 0.39269909f);
        *(float *)(arg0 + 0xC4) = d;
        if (d == *(float *)(arg0 + 0x218)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001749A0(arg0, 0x70, 0, 1.0f);
            *(short *)(arg0 + 0x28) = 8;
            *(float *)(arg0 + 0x2E4) = (*(float *)(arg0 + 0x254) - *(float *)(arg0 + 0xB4)) / 8.0f;
            func_0019F680(&D_700038A0, *(int *)(arg0 + 0x30C), 0);
            func_0019F680(&D_700038B0, *(int *)(arg0 + 0x30C), 1);
            *(float *)0x70003A20 = D_700038A0 - D_700038B0;
            *(float *)0x70003A28 = D_700038A8 - D_700038B8;
            dx = *(float *)0x70003A20;
            dz = *(float *)0x70003A28;
            dist1 = func_0011E748(dx * dx + dz * dz) + 0.1f;
            *(float *)0x70003A24 = dist1;
            n = float_to_int(dist1 / 4.5f);

            dx = *(float *)(arg0 + 0x290) - *(float *)(arg0 + 0xB0);
            *(float *)0x70003A20 = dx;
            dz = *(float *)(arg0 + 0x298) - *(float *)(arg0 + 0xB8);
            *(float *)0x70003A28 = dz;
            dist2 = func_0011E748(dx * dx + dz * dz);

            if (!(n & 1)) {
                float t = (float)float_to_int(dist2 / 4.5f);
                *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0x290) - (4.5f * *(float *)(arg0 + 0x2E0)) * t;
                *(float *)(arg0 + 0x2F8) = *(float *)(arg0 + 0x298) - (4.5f * *(float *)(arg0 + 0x2E8)) * t;
            } else {
                float t = (float)float_to_int((dist2 - 2.25f) / 4.5f);
                *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0x290) - (4.5f * *(float *)(arg0 + 0x2E0)) * t - 0.5f * *(float *)(arg0 + 0x2E0);
                *(float *)(arg0 + 0x2F8) = *(float *)(arg0 + 0x298) - (4.5f * *(float *)(arg0 + 0x2E8)) * t - 0.5f * *(float *)(arg0 + 0x2E8);
            }
            *(float *)(arg0 + 0x2E0) = (*(float *)(arg0 + 0x2F4) - *(float *)(arg0 + 0xB0)) / 8.0f;
            *(float *)(arg0 + 0x2E8) = (*(float *)(arg0 + 0x2F8) - *(float *)(arg0 + 0xB8)) / 8.0f;
        }
        return;
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001749A0(arg0, 0xBA, 0, 1.0f);
            *(char *)(arg0 + 0x25F) = 3;
        }
        return;
    case 3:
        if (*(short *)(arg0 + 0x28) == 0) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x254);
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x2F4);
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x2F8);
            func_001FBD50(arg0, 0x123, 0, 300.0f);
            return;
        }
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x2E4) + *(float *)(arg0 + 0xB4);
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x2E0) + *(float *)(arg0 + 0xB0);
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x2E8) + *(float *)(arg0 + 0xB8);
        *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
        return;
    case 4:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 5) = 0x10;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x21;
            *(char *)(arg0 + 0x1F1) = 0;
            *(char *)(arg0 + 0x2F1) = 0;
            func_001749A0(arg0, func_001885B0(arg0), 0, 1.0f);
        }
        return;
    }
}
