// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Weapon/state dispatcher on the actor's state byte (+0x5). Three handled
// states plus an implicit default; returns 0 except in state 2.
//   state 0 -> call func_00229960() (init/reset arm), return 0
//   state 1 -> set state-result byte (+0x0) = 3, return 0
//   state 2 -> tail to func_0022A020() and return its result
//   else    -> return 0
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. Under
// 991202 the switch compare-order / branch lowering left a 1-instruction
// residual; 2.3.3 emits the exact CW case-test ordering (==2, ==1, beqz),
// giving a byte-identical match. Verified objdiff 100.0% vs
// build/expected/func_00229A00.o.
extern void func_00229960(void);
extern int func_0022A020(void);

int func_00229A00(char *arg0) {
    unsigned char v1;

    v1 = *(unsigned char *)(arg0 + 5);
    switch (v1) {
    case 0:
        func_00229960();
        break;
    case 1:
        *(char *)(arg0 + 0) = 3;
        break;
    case 2:
        return func_0022A020();
    }
    return 0;
}
