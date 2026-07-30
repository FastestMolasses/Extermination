// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS:
// Thin argument-selecting wrapper around func_0017B460(cmd, arg), dispatching on `cmd` (0..6).
// Only TWO distinct bodies exist; the 7-entry table maps cmds 0,2,3,4,5 -> body A and cmds 1,6 -> body B.
//
// On entry it calls func_001B0070() and keeps bit 2 of the result as `flag` (a global mode/config
// query — the same gate is consulted by both bodies).
//
// Both bodies apply the identical 3-part override gate:
//     obj[0x236] == 0  &&  (obj[0x235] & 1) == 0  &&  flag != 0
// When the gate passes, the second argument is REPLACED by a fixed "override" value; otherwise the
// caller-supplied value is used:
//     body A (cmds 0,2,3,4,5): gate -> func_0017B460(cmd, 4)          else func_0017B460(cmd, idx)
//     body B (cmds 1,6):       gate -> func_0017B460(cmd, &tbl[4])    else func_0017B460(cmd, &tbl[idx])
// So body A passes an int BY VALUE and body B passes a POINTER into `tbl` — the callee is
// prototype-less in the original (declared `int f();`), which is why both forms coexist.
// obj[0x236] reads like a "special/alternate mode active" byte and obj[0x235] bit 0 like a
// per-object "no override" opt-out.
//
// The 16-bit result of each call is sign-extended (dsll32/dsra32 16) into the return slot, i.e. the
// function returns a `short`.
//
// UNINITIALIZED DEFAULT, faithfully reproduced: for cmd >= 7 the switch has NO default arm and the
// function falls straight to the epilogue returning `ret` UNSET (the target likewise returns
// whatever $s0 happens to hold — $s0 is saved in the jal delay slot and never written on that path).
// This is original-source behaviour, not a decode error; callers are expected never to pass cmd >= 7.

extern int func_001B0070();
extern int func_0017B460();

short func_0017B490(char *obj, int cmd, int idx, int *tbl)
{
    short ret;
    int flag;

    flag = func_001B0070() & 4;
    switch (cmd) {
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
        if (*(unsigned char *)(obj + 0x236) == 0) {
            if ((*(unsigned char *)(obj + 0x235) & 1) == 0) {
                if (flag) {
                    ret = func_0017B460(cmd, 4);
                    break;
                }
            }
        }
        ret = func_0017B460(cmd, idx);
        break;
    case 1:
    case 6:
        if (*(unsigned char *)(obj + 0x236) == 0) {
            if ((*(unsigned char *)(obj + 0x235) & 1) == 0) {
                if (flag) {
                    ret = func_0017B460(cmd, tbl + 4);
                    break;
                }
            }
        }
        ret = func_0017B460(cmd, tbl + idx);
        break;
    }
    return ret;
}
