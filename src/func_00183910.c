// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), decomp2 lane round 3 (was a 94.6% NEARMISS).
// Actor sub-state step keyed on the byte at +6:
//   0: if (&D_00275B00)[2] (gp-relative, -sdatathreshold 4) is nonzero, set +6
//      to 1 and clear the int at +0x2EC.
//   1: func_00179880(self, self + 0x2EC), then advance +6 when
//      func_00175900(self, 1) is nonzero.
//   2: nothing.
// How it matches: func_00179880 takes (actor, float *) (its own C, and
// func_0016EBA0 calls it so since decomp lane round 8); the old C passed only
// the pointer, and the "arg0 kept in $a0 across the dispatch" residual the old
// header recorded was the missing first argument.

extern void func_00179880(unsigned char *a0, float *a1);
extern int func_00175900(char *, int);
extern int D_00275B00;

void func_00183910(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        if ((&D_00275B00)[2] != 0) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)(arg0 + 0x2EC) = 0;
        }
        break;
    case 1:
        func_00179880((unsigned char *)arg0, (float *)(arg0 + 0x2EC));
        if (func_00175900(arg0, 1) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        }
        break;
    case 2:
        break;
    }
}
