// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Two-stage gate + state dispatcher on an object (arg0). First runs
// func_001FE9A0(1, &arg0[0x50], &arg0[0x54]); if it returns 0, bail returning 0.
// Otherwise dispatches on the object's state byte (+0x3):
//   state 0 -> call func_00228530(arg0)
//   state 1 -> call sub_BASCUS_97112_DS00_00_EX_DATA_00(arg0)
//   state 2 -> call sub_BASCUS_97112_DS00_00_EX_DATA_00_2(arg0)
//   state 3 -> return 1 (already-complete fast path)
//   else    -> (handlers above fall through)
// Finally: if global D_00275C58 is set, return 2; else return 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. The switch
// value MUST be read into `unsigned char` so 2.3.3 emits the exact CW case-test
// ordering (==2, ==1, beqz) with plain `beq` rather than branch-likely; 991202
// mis-lowers the early-return tail. Verified objdiff 100.0% vs
// build/expected/func_00228A90.o.
extern int func_001FE9A0(int, char *, char *);
extern void func_00228530(char *);
extern void sub_BASCUS_97112_DS00_00_EX_DATA_00(char *);
extern void sub_BASCUS_97112_DS00_00_EX_DATA_00_2(char *);
extern int D_00275C58;

int func_00228A90(char *arg0) {
    unsigned char v1;

    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    v1 = *(unsigned char *)(arg0 + 3);
    switch (v1) {
    case 0:
        func_00228530(arg0);
        break;
    case 1:
        sub_BASCUS_97112_DS00_00_EX_DATA_00(arg0);
        break;
    case 2:
        sub_BASCUS_97112_DS00_00_EX_DATA_00_2(arg0);
        break;
    case 3:
        return 1;
    }
    return D_00275C58 ? 2 : 0;
}
