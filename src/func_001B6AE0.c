// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame step driven by a small state byte at arg1[4] (0=init, 1=count, 2=poll):
//   - state 0: run func_001FD4C0(arg2[6]); if the busy word arg2[2] is clear,
//     advance the state, fire func_00119828(0,..) and (1,..), and reset the timer
//     float at arg2[0x10] to 0; otherwise force the state to 2.
//   - state 1: advance the timer float by 1.0; once it exceeds 60.0 bump the state.
//   - state 2: report done (return 1) once the global flag D_008106F4 == 1.
//
// Built with mwcc 2.3.3: the float-constant 1.0 must land in $f2 (matching the
// target add.s $f1,$f2,$f1); hoisting it to a named local `one` forces that FP
// register assignment. objdiff 100.0 vs expected; pinned 991202 = 98.5%.
extern int func_00119828(int, int, int);
extern void func_001FD4C0(int);
extern unsigned char D_008106F4;

int func_001B6AE0(int arg0, unsigned char *arg1, int *arg2) {
    float one, *fp;

    switch (arg1[4]) {
    case 0:
        func_001FD4C0(arg2[6]);
        if (arg2[2] == 0) {
            arg1[4] = arg1[4] + 1;
            func_00119828(0, 0, 0);
            func_00119828(1, 0, 0);
            *(float *)(arg2 + 4) = 0.0f;
        } else {
            arg1[4] = 2;
        }
        break;
    case 1:
        one = 1.0f;
        fp = (float *)(arg2 + 4);
        *fp = *fp + one;
        if (!(*fp <= 60.0f)) {
            arg1[4] = arg1[4] + 1;
        }
        break;
    case 2:
        if (D_008106F4 == 1) {
            return 1;
        }
        break;
    }
    return 0;
}
