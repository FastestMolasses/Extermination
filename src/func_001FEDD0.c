// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// State-machine step (state 0xB). Reads the shared mode word D_00275C5C:
//   - state 0: arm this step (set state to 0xB), kick off func_00114448(); if
//     it reports busy (nonzero) raise the busy flag D_00275C58, then return -1.
//   - state 0xB: when the result word D_00275C64 is zero, clear the state and
//     return 0; otherwise raise the busy flag and return -1.
//   - any other state: raise the busy flag, return -1.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build fills the
// clean-store beqz delay slot (safe li), leaving a delay-slot-nop residual; 2.3.3
// leaves it empty and matches byte-identical. objdiff 100.0 vs expected; 991202 94.1%.
extern int func_00114448(void);
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C64;

int func_001FEDD0(void) {
    int ret = -1;

    switch (D_00275C5C) {
    case 0:
        D_00275C5C = 0xB;
        if (func_00114448() != 0) {
            D_00275C58 = 1;
        }
        break;
    case 0xB:
        if (D_00275C64 != 0) {
            D_00275C58 = 1;
        } else {
            D_00275C5C = 0;
            ret = 0;
        }
        break;
    default:
        D_00275C58 = 1;
        break;
    }
    return ret;
}
