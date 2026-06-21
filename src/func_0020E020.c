// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Zeroes 16 x 16-byte records of D_00821300 then clears gp-rel D_00275C90.
// The loop-counter init compiles to `paddub aN,zero,zero` only under mwcc 2.3.3
// (991202 emits a plain move and loses the idiom). Declaring i before p is
// load-bearing: it assigns the counter to $a0 and the pointer to $a1, matching
// the target's register allocation.
extern int D_00821300[];
extern int D_00275C90;

void func_0020E020(void) {
    int i;
    int *p;
    for (i = 0, p = D_00821300; i < 0x10; i++, p += 4) {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p[3] = 0;
    }
    D_00275C90 = 0;
}
