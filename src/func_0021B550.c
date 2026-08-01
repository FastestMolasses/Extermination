// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Loading-screen particle-veil state machine, driven off the global struct
// pointed to by D_00275888: state byte at +0, sub-state at +1, second-level
// sub-state at +2, and three intensity floats at +8 / +0xC / +0x10.
//
//   state 0 (fade-in warm-up): sub-state 0 and 1 stop particle group 2, start
//     group 3 and bump the sub-state; any other sub-state zeroes the three
//     intensities and advances to state 1.
//   state 1 (ramp up): keeps group 3 alive and ramps the intensities by
//     0.01 / 0.008 / 0.006 per frame, clamped to 1.0, then drives the particle
//     system via func_0021B1B0 + func_0021B500.
//   state 2 (fade-out): sub-state 0 decays the intensities by 0.9 / 0.92 /
//     0.94, keeps group 3 alive, updates the system, and once all three drop
//     below 0.01 moves to sub-state 1. Sub-state 1 stops group 2 and runs a
//     three-frame tail on the +2 counter before advancing to state 3.
//   state 3 / default: stops group 2 and returns 1 (effect finished); every
//     other state returns 0.
//
// Matching keys: (1) the state-2 sub-state test must be a `switch (st2)` with
// ascending `case 0` / `case 1` - mwcc then emits the target's descending
// `beql st2,1` (with func_001D2830's literal-2 first argument reused from the
// outer switch in the delay slot) followed by `beqz st2` + explicit `b` to the
// state-2 exit. Written as `if (st2 != 1) { if (st2 != 0) break; ... }` mwcc
// collapses the double negative into one `bnez` and loses two instructions.
// (2) the three decays must be compound assignments (`*= 0.9f`), which puts
// the loaded field first: `mul.s $f0,$f0,$f1`. Spelled `x = x * 0.9f` mwcc
// emits `mul.s $f0,$f1,$f0`.
// (3) mwcc 2.3.3 is required: the pinned 991202 build caps at 83.66% and
// 2.4.1 at 99.03%.
extern void func_001D2830(int group, int enable);
extern void func_0021B1B0(char *particleState);
extern void func_0021B500(char *particleState);
extern char *D_00275888;

int func_0021B550(void) {
    char *s = D_00275888;
    int ret = 0;
    unsigned char st = *(unsigned char *)(s + 0);
    unsigned char st2;
    unsigned char st3;
    float f;

    switch (st) {
    case 0:
        st2 = *(unsigned char *)(s + 1);
        switch (st2) {
        case 0:
        case 1:
            func_001D2830(2, 0);
            func_001D2830(3, 1);
            *(unsigned char *)(s + 1) = *(unsigned char *)(s + 1) + 1;
            break;
        default:
            *(int *)(s + 0x10) = 0;
            *(int *)(s + 0xC) = 0;
            *(int *)(s + 8) = 0;
            *(unsigned char *)(s + 0) = 1;
            break;
        }
        break;
    case 1:
        func_001D2830(3, 1);
        f = *(float *)(s + 8) + 0.01f;
        f = (f < 1.0f) ? f : 1.0f;
        *(float *)(s + 8) = f;
        f = *(float *)(s + 0xC) + 0.008f;
        f = (f < 1.0f) ? f : 1.0f;
        *(float *)(s + 0xC) = f;
        f = *(float *)(s + 0x10) + 0.006f;
        f = (f < 1.0f) ? f : 1.0f;
        *(float *)(s + 0x10) = f;
        func_0021B1B0(s);
        func_0021B500(s);
        break;
    case 2:
        st2 = *(unsigned char *)(s + 1);
        switch (st2) {
        case 0:
            *(float *)(s + 8) *= 0.9f;
            *(float *)(s + 0xC) *= 0.92f;
            *(float *)(s + 0x10) *= 0.94f;
            func_001D2830(3, 1);
            func_0021B1B0(s);
            func_0021B500(s);
            if (*(float *)(s + 8) < 0.01f && *(float *)(s + 0xC) < 0.01f && *(float *)(s + 0x10) < 0.01f) {
                *(unsigned char *)(s + 1) = 1;
            }
            break;
        case 1:
            func_001D2830(2, 0);
            st3 = *(unsigned char *)(s + 2);
            switch (st3) {
            case 0:
            case 1:
            case 2:
                func_001D2830(3, 1);
                *(unsigned char *)(s + 2) = *(unsigned char *)(s + 2) + 1;
                break;
            default:
                *(char *)(s + 0) = 3;
                break;
            }
            break;
        }
        break;
    default:
    case 3:
        func_001D2830(2, 0);
        ret = 1;
        break;
    }
    return ret;
}
