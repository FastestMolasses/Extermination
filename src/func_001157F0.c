// COMPILER: eegcc
// CFLAGS: -O2
//
// Appends a 16-byte command (a0..a3) to the queue at D_0027F7C0 + bank *
// 0x1000 (bank D_0027F740[15], count D_0027F740[16]); returns the new count,
// or -1 when the queue holds 255 entries.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern volatile int D_0027F740[];
extern char D_0027F7C0[];

int func_001157F0(int a0, int a1, int a2, int a3) {
    int *e = (int *)(D_0027F7C0 + (D_0027F740[15] << 12) + (D_0027F740[16] << 4));
    if (D_0027F740[16] >= 255) {
        return -1;
    }
    e[0] = a0;
    e[1] = a1;
    e[2] = a2;
    e[3] = a3;
    D_0027F740[16]++;
    return D_0027F740[16];
}
