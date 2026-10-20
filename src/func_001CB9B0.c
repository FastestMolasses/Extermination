// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Returns D_00275674 + 0x6A0 + idx * 0x80 for idx 0..4 (as five cases), else
// 0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern int D_00275674;

int func_001CB9B0(int idx) {
    switch (idx) {
    case 0:
        return D_00275674 + 0x6A0;
    case 1:
        return D_00275674 + 0x720;
    case 2:
        return D_00275674 + 0x7A0;
    case 3:
        return D_00275674 + 0x820;
    case 4:
        return D_00275674 + 0x8A0;
    }
    return 0;
}
