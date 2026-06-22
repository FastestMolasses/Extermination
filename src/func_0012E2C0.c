// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-object state machine keyed on the phase byte arg0[7].
//   phase 0: kick off the move -- compute a target value via func_001B1240
//            (parent block arg0+0xB0 with two tuning floats), store it at
//            arg1+0xE8, advance the phase to 1, and arm a 0x78-tick timer at
//            arg1+0xD2. Returns 0 (not done).
//   phase 1: ease arg0+0xC4 toward arg1+0xE8 via func_001B12B0 (step
//            0x3D8EFA35 ~ 0.0698f), decrement the 16-bit timer, and report
//            done (1) when the timer expires or the value has reached target.
//   other phases: return 0.
// The single trailing `return ret;` (ret defaulting to 0) is what merges the
// case-0 / default / case-1-fallthrough exits into one epilogue and matches.
extern float func_001B1240(int, float, float);
extern float func_001B12B0(float, float, float);
extern float D_00810350;
extern float D_00810358;

int func_0012E2C0(unsigned char *arg0, unsigned char *arg1) {
    short v0;
    int ret;

    ret = 0;
    switch (arg0[7]) {
    case 0:
        *(float *)(arg1 + 0xE8) = func_001B1240((int)(arg0 + 0xB0), D_00810350, D_00810358);
        arg0[7] = arg0[7] + 1;
        *(short *)(arg1 + 0xD2) = 0x78;
        break;
    case 1:
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0xE8), *(float *)(arg0 + 0xC4), 0.06981316953897476f);
        v0 = *(short *)(arg1 + 0xD2) - 1;
        *(short *)(arg1 + 0xD2) = v0;
        if (v0 == 0) {
            return 1;
        }
        if (*(float *)(arg0 + 0xC4) == *(float *)(arg1 + 0xE8)) {
            return 1;
        }
        break;
    }
    return ret;
}
