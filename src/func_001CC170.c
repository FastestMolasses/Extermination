// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// TU_STATIC: func_001CBE10
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a CodeWarrior asm body with a
// readable companion; same-TU callee via // TU_STATIC:). tools/decomp/tu_static.py removes the
// static copy of func_001CBE10 after checking it equals the original; the call links to that
// function's own unit.
//
// Pixel width of a message string: the sum of func_001CBE10(c) (the glyph
// advance) over the bytes c >= 0x20 of the first strlen(s) bytes
// (func_001232E0 is strlen); control bytes add nothing.
// The original keeps the count and the running sum in caller-saved registers
// across the call (intra-TU register analysis of the leaf func_001CBE10), so
// the callee is compiled here as a static of the same TU.
static int func_001CBE10(int id);  /* same-TU callee, see TU_STATIC */
#include "src/func_001CBE10.c"
extern unsigned int func_001232E0(unsigned char *s);

int func_001CC170(unsigned char *s) {
    unsigned int n = func_001232E0(s);
    int width = 0;

    while (n != 0) {
        if (*s >= 0x20U) {
            width += func_001CBE10(*s);
        }
        n--;
        s++;
    }
    return width;
}
