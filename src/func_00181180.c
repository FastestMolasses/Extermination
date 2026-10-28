// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Hit/launch check for an entity: if either of the floats at e+0x224 / e+0x22C
// is nonzero, or bit 1 of e[0xF] is set, the entity is put into state
// (mode 2, sub-state 5, step 0) in e[4]/e[5]/e[6], the argument is stored in
// e[0x302], and it returns 1; otherwise nothing is written and it returns 0.
// Matching note: the zero is a named local (`zero`); comparing against the
// 0.0f literal swaps the compare operands.
int func_00181180(unsigned char *e, int arg) {
    int r;
    float zero = 0.0f;
    if (*(float *)(e + 0x224) != zero || *(float *)(e + 0x22C) != zero || (e[0xF] & 2)) {
        e[4] = 2;
        e[5] = 5;
        e[6] = 0;
        e[0x302] = arg;
        r = 1;
    } else {
        r = 0;
    }
    return r;
}
