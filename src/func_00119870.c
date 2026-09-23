// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 63.33% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern short D_0027F778;

void func_00119870(short a0) {
    D_0027F778 = a0;
}
