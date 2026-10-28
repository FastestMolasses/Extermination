// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.74% NEARMISS).
// Fixes read from the original instructions:
// - The function takes two parameters and passes them straight through as
//   the first two arguments of func_00114988; only the last three arguments
//   are zero. The old (void) signature with five literal zeros was the whole
//   "register-allocation" residual: with a0/a1 live, the state value lands in
//   a2 and only three argument registers are zeroed before the call.
// - func_00114988 returns int (its definition in src/).
//
// State machine on the gp-rel global D_00275C5C:
// clamp the state to -1 when it is non-negative, then dispatch:
//   state == -1 -> advance to state -2 and call func_00114988 (returns -1)
//   state == -2 -> reset D_00275C58 and D_00275C5C to 0, return 0
//   otherwise   -> return -1
// The default/-1 result (-1) lives in a callee-saved register across the call.
extern int D_00275C58;
extern int D_00275C5C;
extern int func_00114988(int a0, int a1, int a2, int a3, int t0);

int func_001FE920(int a0, int a1)
{
    int s0;

    s0 = -1;
    if (D_00275C5C >= 0) {
        D_00275C5C = s0;
    }
    switch (D_00275C5C) {
    case -1:
        D_00275C5C = -2;
        func_00114988(a0, a1, 0, 0, 0);
        break;
    case -2:
        D_00275C58 = 0;
        D_00275C5C = 0;
        s0 = 0;
        break;
    }
    return s0;
}
