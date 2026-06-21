// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Far field at a0[0x14001] (byte offset 0x50004). Clamp a1 to min(a1, field)
// then subtract it back into the field, returning the clamped amount.
// int* indexing -> single lw with 0x50004 disp (assembler $at-expands, as in
// target). Ternary forces slt+movz; compound -= re-reads (target's 2nd load).
int func_00203AE0(int *a0, int a1) {
    a1 = (a1 < a0[0x14001]) ? a1 : a0[0x14001];
    a0[0x14001] -= a1;
    return a1;
}
