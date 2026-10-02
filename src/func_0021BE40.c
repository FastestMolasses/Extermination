// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (lane DMATCH 2026-10-02, was companion C at 90.83%): one combined condition
// with a shared `return 1;` and a final `return 0;` (the func_0021BD60 lever).
// Returns 0 when every test passes, else 1: the scratchpad byte 0x70003B8D is 0,
// self+0x220 is not <= 0.0, self+0 == 1, self+4 == 1, func_0021BB00(self) returns 0 and
// the halfword self+0x20E is 0.
extern unsigned char D_70003B8D;
extern int func_0021BB00(char *p);

int func_0021BE40(char *p) {
    if (D_70003B8D != 0 || *(float *)(p + 0x220) <= 0.0f || *(unsigned char *)(p + 0) != 1 ||
        *(unsigned char *)(p + 4) != 1 || func_0021BB00(p) != 0 || *(short *)(p + 0x20E) != 0) {
        return 1;
    }
    return 0;
}
