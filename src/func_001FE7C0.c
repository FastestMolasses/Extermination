// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Twin of func_001FE6B0: the same open/read/close async state machine on *arg0,
// but for a second stream slot. Differences: open uses mode arg 1 (not 0x203),
// the read step calls func_001FEB00, and the handle/result live in D_00275844
// and D_00275C6C (D_00275C58 is the shared busy flag).
//   - state 0: func_001FE9E0(arg1,arg2,arg3,1); on success D_00275844 = handle,
//     advance to state 1.
//   - state 1: func_001FEB00(handle, arg4, arg5); on success advance to state 2,
//     D_00275C6C = result.
//   - state 2: func_001FEA70(handle); when done, handle = -1, return 1 if
//     D_00275C6C == arg5 else -1.
// After any step, busy flag set -> tear down and return -1, else return 0.
//
// gp-relative globals require -sdatathreshold 4; early-return busy block matches
// the target $v0 reuse + dead join block. objdiff 100.0 vs expected; 991202 = 90.1%.
extern int func_001FE9E0(int, int, int, int);
extern int func_001FEA70(int);
extern int func_001FEB00(int, int, int);
extern int D_00275844;
extern int D_00275C58;
extern int D_00275C6C;

int func_001FE7C0(int *arg0, int arg1, int arg2, int arg3, int arg4, int arg5) {
    int r;

    switch (*arg0) {
    case 0:
        r = func_001FE9E0(arg1, arg2, arg3, 1);
        if (r >= 0) {
            D_00275844 = r;
            *arg0 = 1;
        }
        break;
    case 1:
        r = func_001FEB00(D_00275844, arg4, arg5);
        if (r >= 0) {
            *arg0 = 2;
            D_00275C6C = r;
        }
        break;
    case 2:
        if (func_001FEA70(D_00275844) == 0) {
            *arg0 = -1;
            D_00275844 = -1;
            if (D_00275C6C != arg5) {
                D_00275C6C = 0;
                return -1;
            }
            D_00275C6C = 0;
            return 1;
        }
        break;
    }
    if (D_00275C58 != 0) {
        *arg0 = -1;
        D_00275844 = -1;
        D_00275C6C = 0;
        return -1;
    }
    return 0;
}
