// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Range-reduce a float into (-inf, 2*PI] by repeatedly subtracting 2*PI
// (0x40C90FDB = 6.2831855f). CW emits a bc1tl entry guard + bc1f loop;
// mwcc 2.3.3 reproduces the exact branch-likely / mov.s scheduling byte-identically.
float func_001B1510(float x) {
    while (!(x <= 6.2831855f)) {
        x -= 6.2831855f;
    }
    return x;
}
