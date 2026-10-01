// NEARMISS func_001CC170  (vram 0x001CC170, 0x64 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 100.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// The original keeps the count and the running width in caller-saved registers across the call to
// the leaf func_001CBE10 (intra-TU register analysis); the match needs a static copy of the callee
// in this TU, whose extra .text the per-function link cannot take.
//
// The function links from the asm body in src/func_001CC170.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Pixel width of a message string: the sum of func_001CBE10(c) (the glyph
// advance) over the bytes c >= 0x20 of the first strlen(s) bytes
// (func_001232E0 is strlen); control bytes add nothing.
// The original keeps the count and the running sum in caller-saved registers
// across the call (intra-TU register analysis of the leaf func_001CBE10), so
// the callee is copied here as a static of the same TU.
static int func_001CBE10(int id) {
    switch (id) {
    case 0x20: return 5;
    case 0x21: return 4;
    case 0x27: return 4;
    case 0x28: return 8;
    case 0x29: return 8;
    case 0x2C: return 6;
    case 0x2E: return 5;
    case 0x2F: return 8;
    case 0x3A: return 7;
    case 0x3B: return 8;
    case 0x49: return 4;
    case 0x4A: return 7;
    case 0x4D: return 0xC;
    case 0x57: return 0xC;
    case 0x5B: return 8;
    case 0x5C: return 8;
    case 0x5D: return 8;
    case 0x60: return 5;
    case 0x66: return 8;
    case 0x69: return 4;
    case 0x6A: return 6;
    case 0x6C: return 4;
    case 0x6D: return 0xC;
    case 0x72: return 9;
    case 0x77: return 0xC;
    case 0x82: return 6;
    case 0x84: return 8;
    case 0x8B: return 9;
    case 0x91: return 6;
    case 0x92: return 6;
    case 0x93: return 8;
    case 0x94: return 8;
    case 0x9B: return 9;
    case 0xA1: return 4;
    case 0xA6: return 6;
    default: return 9;
    }
}
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
