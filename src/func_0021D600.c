// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Returns 1 if byte field at a0+0x1F1 is 1, 3, or 4; else 0.
// Short-circuit OR (==1 || (v-3)<2) reproduces the CW branch lowering exactly
// (beq + sltiu/beqz, redundant delay-slot addiu, unreachable trailing paddub)
// instead of the movz that a plain if/return collapses to.
int func_0021D600(int a0) {
    unsigned char v;
    v = *(unsigned char *)(a0 + 0x1F1);
    if (v == 1 || (unsigned int)(v - 3) < 2) {
        return 1;
    }
    return 0;
}
