// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Async-stream state machine keyed on *arg0 (0=open, 1=read, 2=close):
//   - state 0: func_001FE9E0(arg1,arg2,arg3,0x203) opens; on success store the
//     handle in D_00275840 and advance to state 1.
//   - state 1: func_001FEB90(handle, arg4, arg5) reads; on success advance to
//     state 2 and record the result in D_00275C68.
//   - state 2: func_001FEA70(handle) closes; when it reports done, mark the
//     handle as freed (-1), then return 1 if the recorded D_00275C68 matches
//     arg5, else -1.
// After any step, if the shared busy flag D_00275C58 is set, tear everything
// down (state/handle = -1, D_00275C68 = 0) and return -1; otherwise return 0.
//
// gp-relative globals require -sdatathreshold 4. The trailing busy-flag block is
// written as an early `return -1;` so the flag load reuses $v0 and mwcc emits the
// dead var=0 join block the target has. objdiff 100.0 vs expected; 991202 = 90.1%.
extern int func_001FE9E0(int, int, int, int);
extern int func_001FEA70(int);
extern int func_001FEB90(int, int, int);
extern int D_00275840;
extern int D_00275C58;
extern int D_00275C68;

int func_001FE6B0(int *arg0, int arg1, int arg2, int arg3, int arg4, int arg5) {
    int r;

    switch (*arg0) {
    case 0:
        r = func_001FE9E0(arg1, arg2, arg3, 0x203);
        if (r >= 0) {
            D_00275840 = r;
            *arg0 = 1;
        }
        break;
    case 1:
        r = func_001FEB90(D_00275840, arg4, arg5);
        if (r >= 0) {
            *arg0 = 2;
            D_00275C68 = r;
        }
        break;
    case 2:
        if (func_001FEA70(D_00275840) == 0) {
            *arg0 = -1;
            D_00275840 = -1;
            if (D_00275C68 != arg5) {
                D_00275C68 = 0;
                return -1;
            }
            D_00275C68 = 0;
            return 1;
        }
        break;
    }
    if (D_00275C58 != 0) {
        *arg0 = -1;
        D_00275840 = -1;
        D_00275C68 = 0;
        return -1;
    }
    return 0;
}
