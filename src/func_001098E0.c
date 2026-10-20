// COMPILER: eegcc
// CFLAGS: -O2
//
// Rounds a2 + 19 down to a multiple of 16 (signed division), stores a1 and the
// rounded size in D_002412F8 / D_00241400 and tail-calls func_0010A400(a1,
// size).
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern int D_002412F8;
extern int D_00241400;
extern int func_0010A400(int src, int count);

void func_001098E0(int a0, int a1, int a2) {
    a2 = ((a2 + 19) / 16) * 16;
    D_002412F8 = a1;
    D_00241400 = a2;
    func_0010A400(a1, a2);
}
