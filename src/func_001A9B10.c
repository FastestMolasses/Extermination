// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003B88

//
// Double-loop interaction pass over two global object lists. Outer list: base D_00275B90, count
// D_00275B98, walked via a global short countdown at 0x70003B86 (seeded from D_00275B98). Inner
// list: base D_00275B80, count D_00275B88, walked via a global short countdown at 0x70003B88
// (seeded from D_00275B88). Runs only if both counts are nonzero. For each outer entry e with
// e[3]==0 whose type field (*(int*)(e+0x2D4) >> 8) is 1, 2, or 3, it walks the inner list: for
// each inner entry f with f[3]==7 and a nonzero float at f+0x38, it calls func_001A99E0(e). Each
// counter is decremented and re-tested per iteration so a nested handler can shorten the walk
// (func_001A99E0 itself zeroes 0x70003B88).
//
// MATCH NOTE (m2-matching lane, 87.30% -> 100.0%): (1) both countdowns are
// relocated, volatile scratchpad externs (idiom-22/32), over-declared as arrays
// for -sdatathreshold 8; 0x70003B88 is opted in by `// SPAD: 0x70003B88`. This
// stops mwcc hoisting `lui at,0x7000` into the loop slots. (2) The f[3] == 7
// test is a one-case `switch`, which lowers to the target's `beq 7 / b next`.
// (3) Declaring p, e, q in that order gives the target's s0/s1/s2 coloring.
extern void func_001A99E0(unsigned char *);
extern unsigned char **D_00275B80;
extern short D_00275B88;
extern unsigned char **D_00275B90;
extern short D_00275B98;
extern volatile short D_70003B86[8];
extern volatile short D_70003B88[8];

void func_001A9B10(void) {
    int n;
    unsigned char **p;
    unsigned char *e;
    unsigned char **q;
    unsigned char *f;
    int t;

    n = D_00275B98;
    D_70003B86[0] = n;
    if (n == 0) return;
    if (D_00275B88 == 0) return;
    q = D_00275B90;
    while (D_70003B86[0] != 0) {
        e = *q++;
        D_70003B86[0] = D_70003B86[0] - 1;
        if (e[3] != 0) continue;
        t = *(int *)(e + 0x2D4) >> 8;
        switch (t) {
        case 1:
        case 2:
        case 3:
            D_70003B88[0] = D_00275B88;
            p = D_00275B80;
            while (D_70003B88[0] != 0) {
                f = *p++;
                D_70003B88[0] = D_70003B88[0] - 1;
                switch (f[3]) {
                case 7:
                    if (*(float *)(f + 0x38) != 0.0f) {
                        func_001A99E0(e);
                    }
                }
            }
            break;
        }
    }
}
