// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Eases a global timer/fade value held at the fixed address 0x70003680.
// Calls func_001CD180 (returns a frame/tick count, also cached in D_00275C00);
// if nonzero, computes (count - arg2), clamps the normalized result
// (over arg3) into [0,1], and stores (1 - clamp) back to 0x70003680, which
// is also the return value. The over-1 clamp is written as a ternary so 2.3.3
// leaves the bc1t delay slot as a nop (matching CW) instead of hoisting the
// 1.0f-materializing lui into it (the if-statement form fills the slot, 98.6%).
// Float params arg2/arg3 land in $f14/$f15 -> two leading (unused) float args.
extern int func_001CD180(void);
extern int D_00275C00;

float func_001CD2B0(float a0, float a1, float arg2, float arg3) {
    float v;
    float t;
    *(float *)0x70003680 = 0.0f;
    D_00275C00 = func_001CD180();
    if (D_00275C00 != 0) {
        t = (float)D_00275C00 - arg2;
        *(float *)0x70003680 = t;
        if (t < 0.0f) { v = 0.0f; } else { v = t / arg3; }
        v = (v > 1.0f) ? 1.0f : v;
        *(float *)0x70003680 = 1.0f - v;
    }
    return *(float *)0x70003680;
}
