// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Action-blocked test for an actor record e (returns 1 = blocked, 0 = free).
// Blocked when the scratchpad mode byte 0x70003B8D is non-zero, when health
// e+0x220 is <= 0, when e[0] or e[4] is not 1, when e[0x236] is set, when
// the current kind e+0x1F0 blocks actions (func_0021BB00 / func_0021BC40),
// when e[0xA] is 0, or while the halfword timer e+0x20E is still running.

extern int func_0021BB00(unsigned char *e);
extern int func_0021BC40(unsigned char *e);

int func_0021BED0(unsigned char *e) {
    if (*(unsigned char *)0x70003B8D != 0 || *(float *)(e + 0x220) <= 0.0f || e[0] != 1 || e[4] != 1
        || e[0x236] != 0 || func_0021BB00(e) != 0 || func_0021BC40(e) != 0 || e[0xA] == 0
        || *(short *)(e + 0x20E) != 0) {
        return 1;
    }
    return 0;
}
