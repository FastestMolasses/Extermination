// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Three-state advance gate keyed on the s8 flag D_00282150 (range 0/1/other).
// State 0: bump the flag to 1 and fall through to state 1's action.
// State 1: tail-call func_001FB3E0() and return its result.
// Any other state: reset both D_00282150 and the adjacent s8 D_00282151 to 0
// and return 0. The 991202 build lowers this irregular switch with a
// branch-merge that differs from CodeWarrior's layout (84.2%); mwcc 2.3.3
// (mwcps2-2.3.3-000906) reproduces it byte-identically. Verified objdiff
// 100% vs build/expected/func_001FB370.o.
extern int func_001FB3E0(void);
extern signed char D_00282150;
extern signed char D_00282151;

int func_001FB370(void) {
    switch (D_00282150) {
    case 0:
        D_00282150 += 1;
        /* fallthrough */
    case 1:
        return func_001FB3E0();
    default:
        D_00282150 = 0;
        D_00282151 = 0;
        return 0;
    }
}
