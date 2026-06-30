// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sets arg0[0x274]=1, then selects a short from one of two pointer tables
// indexed by arg1: table D_00248B70 when the state byte arg0[5] is 0x1D/0x1E,
// else table D_00248C50. Each table entry is a pointer to a short; the short
// is converted to float via func_001C61D0(arg0[0x40], v) and stored at
// arg0+0x2F4. If the current sub-state byte arg0[0x275] differs from arg1,
// it is updated, arg0[7] is cleared, and the function returns 1 (changed);
// otherwise returns 0.
extern int func_001C61D0(int a, short b);
extern char D_00248B70[];
extern char D_00248C50[];

int func_0017A8B0(char *arg0, int arg1) {
    short v;
    unsigned char st;

    *(char *)(arg0 + 0x274) = 1;
    st = *(unsigned char *)(arg0 + 5);
    if (st == 0x1D || st == 0x1E) {
        v = **(short **)(D_00248B70 + arg1 * 4);
    } else {
        v = **(short **)(D_00248C50 + arg1 * 4);
    }
    *(float *)(arg0 + 0x2F4) = (float) func_001C61D0(*(int *)(arg0 + 0x40), v);
    if (*(unsigned char *)(arg0 + 0x275) != arg1) {
        *(unsigned char *)(arg0 + 0x275) = arg1;
        *(char *)(arg0 + 7) = 0;
        return 1;
    }
    return 0;
}
