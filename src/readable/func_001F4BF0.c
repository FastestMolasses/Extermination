// NEARMISS func_001F4BF0  (vram 0x001F4BF0, 0xC8 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 99.70% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// One float register: the original materialises 3.0 in f13 and copies it to f12; mwcc 2.3.3 loads
// f12 and copies to f13 (literal, shared-local and cast spellings and mwcc 991202 / 2.4 measured).
// Colour packing and the random brightness match.
//
// The function links from the asm body in src/func_001F4BF0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_00122BB8(void);
extern int func_001CD520(int, int, void *, long long, int, float, float, float);
static inline long long shl64(long long x, int n) { return x << n; }

void func_001F4BF0(void *pos, unsigned int *rgba) {
    unsigned int a;
    unsigned int c;
    a = rgba[3];
    a += (a * ((func_00122BB8() >> 23) & 0xFF)) >> 8;
    a >>= 1;
    c = ((rgba[2] * a) >> 7) << 16;
    c |= ((rgba[1] * a) >> 7) << 8;
    c |= (rgba[0] * a) >> 7;
    func_001CD520(0, 2, pos, (shl64((long long)0x20045B05, 0x20) | shl64(0x9942, 0x10)) | 0x1EF0, c, 3.0f, 3.0f, 1.5f);
}
