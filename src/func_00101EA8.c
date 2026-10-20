// COMPILER: eegcc
// CFLAGS: -O2
//
// Copies the 20-byte record D_002410E8 to a0 and returns a0.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

typedef struct { int w[5]; } Rec20;
extern Rec20 D_002410E8;

char *func_00101EA8(char *a0) {
    *(Rec20 *)a0 = D_002410E8;
    return a0;
}
