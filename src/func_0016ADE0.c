// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// When the pad mask D_00810E70[0] meets either scratchpad mask 0x70003B7C /
// 0x70003B7E, sets self +5 = 0x13, +6 = 0, +0x1F0 = 0x25, D_00275B14 = 30 and
// returns 1; otherwise 0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern unsigned short D_00810E70[4];
extern int D_00275B14;

int func_0016ADE0(char *self) {
    int pad = D_00810E70[0];
    if ((pad & *(unsigned short *)0x70003B7C) != 0 || (pad & *(unsigned short *)0x70003B7E) != 0) {
        self[5] = 0x13;
        self[6] = 0;
        self[0x1F0] = 0x25;
        D_00275B14 = 0x1E;
        return 1;
    }
    return 0;
}
