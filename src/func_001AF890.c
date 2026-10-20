// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Zeroes 13 quadwords at blk, pushes blk on the free stack D_00275BD0 and
// increments the count D_00275BCC.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

typedef unsigned int u128 __attribute__((mode(TI)));
extern char **D_00275BD0;
extern short D_00275BCC;

void func_001AF890(char *blk) {
    u128 *p = (u128 *)blk;
    int n = 13;
    do {
        *p = 0;
        n--;
        p++;
    } while (n != 0);
    *--D_00275BD0 = blk;
    D_00275BCC++;
}
