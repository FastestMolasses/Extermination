// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sets active-input index D_008104EF = arg0, caches its turn-rate D_008104F0
// from table D_00248870[arg0], advances heading accumulator D_00810374 by
// func_001B1470(D_00810374 + fparg0), then if the input is non-zero and the
// state byte at D_008102B0[5] is clear, latches it (D_008102B0[5]=1) and
// clears D_008102B0[6]. D_00810374/D_008104F0 are marked volatile to pin the
// store of D_008104F0 BEFORE the heading load/call (mwcc otherwise defers the
// swc1 across the jal); that ordering is what reaches 100.0 on mwcc 2.3.3.

extern float D_00248870[];
extern unsigned char D_008102B0[];
extern volatile float D_00810374;
extern unsigned char D_008104EF;
extern volatile float D_008104F0;

extern float func_001B1470(float);

void func_00183160(int arg0, float fparg0) {
    unsigned char *p;

    p = D_008102B0;
    D_008104EF = (unsigned char)arg0;
    D_008104F0 = D_00248870[arg0];
    D_00810374 = func_001B1470(D_00810374 + fparg0);
    if (D_008104EF != 0) {
        if (p[5] == 0) {
            p[5] = 1;
            p[6] = 0;
        }
    }
}
