// NEARMISS func_0013BE60  (vram 0x0013BE60, 0xC0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.58% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was an asm-word
// body, matching wave 2026-10-28). Body-correct: same calls, stores and conditions as the original.
// Remaining differences: block layout of the `e[4] == 1 && e[5] == 3` early return only. The
// original branches to the body when either test fails and reaches the return through an
// unconditional branch to the epilogue; mwcc 2.3.3 inverts the second test into a direct branch
// to the epilogue. Nested if, if/else, !=-or, switch (outer or inner), helper function, early
// return vs enclosing if, and mwcc 991202 / 2.4 / -O4,s all tried.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Integrates the actor motion, (e, st). Does nothing while st[0x82]
// is set or while the actor is in mode 1, sub-mode 3 (e[4] == 1, e[5] == 3).
// Otherwise: st+0x44 += st+0x48 (speed += accel); e+0xB4 (y) +=
// st+0x4C * sin(st+0x50); e+0xB0 (x) += st+0x44 * sin(heading e+0xC4);
// e+0xB8 (z) += st+0x44 * cos(heading). func_0011E2A8 / func_0011DE90 are
// the sin / cos helpers (as in func_00140C30).
extern float func_0011DE90(float);
extern float func_0011E2A8(float);

void func_0013BE60(char *e, char *st) {
    if (*(char *)(st + 0x82) != 0) {
        return;
    }
    if (*(unsigned char *)(e + 4) == 1 && *(unsigned char *)(e + 5) == 3) {
        return;
    }
    *(float *)(st + 0x44) += *(float *)(st + 0x48);
    *(float *)(e + 0xB4) += *(float *)(st + 0x4C) * func_0011E2A8(*(float *)(st + 0x50));
    *(float *)(e + 0xB0) += *(float *)(st + 0x44) * func_0011E2A8(*(float *)(e + 0xC4));
    *(float *)(e + 0xB8) += *(float *)(st + 0x44) * func_0011DE90(*(float *)(e + 0xC4));
}
