// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Returns D_00275B40[a0/13] + ((a0 % 13) << 4): row base from the gp-rel table plus a
// 16-byte-strided column offset. 2.3.3 schedules sra right after div with no nop, which
// the 991202 build cannot reproduce.
extern int *D_00275B40;

int func_0022BB70(int a0) {
    int v0 = D_00275B40[a0 / 13];
    return v0 + (a0 % 13 << 4);
}
