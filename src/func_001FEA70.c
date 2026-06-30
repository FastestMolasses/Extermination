// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Async-stream state-machine step (state 3), sibling of func_001FEC20/FEB00/FEB90.
// Reads the shared mode word D_00275C5C:
//   - state 0: arm this step (set state to 3), kick off func_00114480(); if it
//     reports busy (nonzero) raise the busy flag D_00275C58, then return -1.
//   - state 3: if the result word D_00275C64 is nonzero, raise the busy flag and
//     return -1; otherwise clear the state and return 0 (step complete).
//   - any other state: raise the busy flag, return -1.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build fills the
// clean-store beqz delay slot (safe li), leaving a delay-slot-nop residual; 2.3.3
// leaves it empty and matches byte-identical. objdiff 100.0 vs expected; 991202 94.1%.
extern int func_00114480(void);
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C64;

int func_001FEA70(void) {
    int ret = -1;

    switch (D_00275C5C) {
    case 0:
        D_00275C5C = 3;
        if (func_00114480() != 0) {
            D_00275C58 = 1;
        }
        break;
    case 3:
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
