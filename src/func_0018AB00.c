// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State-dispatch helper: reads a mode byte (D_00810CA4) and writes a
// derived value into D_008106C6. For modes 0/1/2 the mapping is fixed
// (5/4/3). Otherwise a secondary byte (D_00810CA7) is consulted: 8 -> 1,
// 9 -> 2, anything else -> 0.

extern unsigned char D_00810CA4;
extern unsigned char D_00810CA7;
extern unsigned char D_008106C6;

void func_0018AB00(void) {
    int v = D_00810CA4;
    if (v == 0) {
        D_008106C6 = 5;
    } else if (v == 1) {
        D_008106C6 = 4;
    } else if (v == 2) {
        D_008106C6 = 3;
    } else if (D_00810CA7 == 8) {
        D_008106C6 = 1;
    } else if (D_00810CA7 == 9) {
        D_008106C6 = 2;
    } else {
        D_008106C6 = 0;
    }
}
