// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (lane DMATCH 2026-10-02, was companion C at 92.35%): the vertical test is an
// empty then-branch with `else return 0;` followed by a separate `return 1;`, which gives
// the original's branch to a return-1 block and its dead join-head copy.
// Zone test: 0 when the scratchpad byte 0x70003B8D is nonzero; otherwise 1 when the
// horizontal distance sqrt(dx^2 + dz^2) (func_0011E748) between a+0xA0/+0xA8 and
// b+0xB0/+0xB8 is at most zone->0x18[0] and |a+0xA4 - b+0xB4| (func_0011DF78) is at
// most zone->0x18[1], else 0.
extern unsigned char D_70003B8D;
extern float func_0011E748(float x);
extern float func_0011DF78(float x);

int func_001BE5F0(char *a, char *b, char *zone) {
    float dx;
    float dz;
    if (D_70003B8D != 0) {
        return 0;
    }
    dx = *(float *)(a + 0xA0) - *(float *)(b + 0xB0);
    dz = *(float *)(a + 0xA8) - *(float *)(b + 0xB8);
    if (!(func_0011E748(dx * dx + dz * dz) <= *(float *)(*(char **)(zone + 0x18) + 0))) {
        return 0;
    }
    if (func_0011DF78(*(float *)(a + 0xA4) - *(float *)(b + 0xB4)) <= *(float *)(*(char **)(zone + 0x18) + 4)) {
    } else {
        return 0;
    }
    return 1;
}
