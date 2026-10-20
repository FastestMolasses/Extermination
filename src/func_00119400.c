// COMPILER: eegcc
// CFLAGS: -O2
//
// Increments the sequence counter D_0027F740[0x12] and queues
// func_001157F0(a0, seq << 8 | (a1 >> 16 & 0xFF), a1 << 16 | (a2 >> 8 &
// 0xFFFF), a2 << 24 | (a3 & 0xFFFFFF)).
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern volatile int D_0027F740[];
extern int func_001157F0(int a0, int a1, int a2, int a3);

void func_00119400(int a0, unsigned int a1, unsigned int a2, unsigned int a3) {
    unsigned int hi, mid, lo;

    D_0027F740[0x12] = D_0027F740[0x12] + 1;
    lo = (a2 << 24) | (a3 & 0xFFFFFF);
    mid = (a1 << 16) | ((a2 >> 8) & 0xFFFF);
    hi = (D_0027F740[0x12] << 8) | ((a1 >> 16) & 0xFF);
    func_001157F0(a0, hi, mid, lo);
}
