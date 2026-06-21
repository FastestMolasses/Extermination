// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Clean-store beqz delay-slot-nop wall: target+2.3.3 leave the beqz delay slot
// as a nop; the pinned 991202 build fills it (93.75%). Matched on mwcc 2.3.3.
// Original is void-returning (no return value reaches v0), which frees the
// scratch value to land in v1 like the target; an int return forced v0 and
// produced a register mismatch throughout.
void func_0017C540(unsigned char *a0) {
    if (a0[0x25C]) {
        a0[0x5] = 1;
        a0[0x6] = 0;
        a0[0x1F0] = 1;
        a0[0x1F1] = 0;
    } else {
        a0[0x5] = 0;
        a0[0x6] = 0;
        a0[0x1F0] = 0;
        *(int *)(a0 + 0x38) = 0;
    }
    a0[0x4] = 1;
}
