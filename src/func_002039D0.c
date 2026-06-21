// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Far record at a0[0x14000..0x14002] (byte offsets 0x50000/0x50004/0x50008).
// Computes d = field(+8) - field(+4); when nonzero, writes base + field(+0)
// into *a1. Returns the difference (the caller ignores it, but returning it is
// what makes mwcc keep d in $v0 to match the target's register allocation).
// The far int* indexing forces CW's `lui at; addu at,a0,at; lw disp(at)`
// addressing macro per access; the clean store leaves the beqz delay slot as a
// nop under 2.3.3 (991202 fills it -> only 93.75). Verified objdiff 100%.
int func_002039D0(int *a0, int **a1) {
    int d = a0[0x14002] - a0[0x14001];
    if (d != 0) {
        *a1 = (int *)((char *)a0 + a0[0x14000]);
    }
    return d;
}
