// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Builds the 23-entry (start, count) span table D_00289B40 from the counts
// D_0024A850; a zero count still advances the start by 1.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

typedef struct { short start; short count; } Span;
extern Span D_00289B40[23];
extern short D_0024A850[23];
void func_001B0BA0(void) {
    int i;
    int pos = 0;
    for (i = 0; i < 23; i++) {
        D_00289B40[i].start = pos;
        D_00289B40[i].count = D_0024A850[i];
        if (D_0024A850[i] == 0) {
            pos++;
        } else {
            pos += D_0024A850[i];
        }
    }
}
