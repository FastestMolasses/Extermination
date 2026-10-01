// NEARMISS func_00225720  (vram 0x00225720, 0x2DC bytes) — readable companion C, NOT byte-identical.
//
// objdiff 99.92% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// One register: the original reads the second page flag (+0x15) into a2, mwcc 2.3.3 into a0
// (switch, cast, signedness and local spellings, mwcc 991202 / 2.4 measured). Every draw call and
// GS constant matches.
//
// The function links from the asm body in src/func_00225720.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00207D00(int slot, int mode);
extern void func_00207E40(int slot, int x, int y, int w, int h, int rgba, unsigned long long tex);
extern int float_to_int(float);

void func_00225720(unsigned char *m) {
    func_00207D00(1, 0);
    func_00207E40(1, 0x7000, 0x7F00, 0x100, 0x100, 0x40808080, 0x20043F0621321F00ULL);
    func_00207E40(1, 0x8000, 0x7F00, 0x100, 0x100, 0x40808080, 0x20043F8621321F40ULL);
    func_00207E40(1, 0x7000, 0x7A90, 0x100, 0x100, 0x40808080, 0x20043E8621321D40ULL);
    func_00207E40(1, 0x8000, 0x7A90, 0x100, 0x100, 0x40808080, 0x20043E0621321D00ULL);
    func_00207D00(1, 3);
    func_00207E40(1, 0x8800, 0x7900, 0x80, 0x80, 0x70808080, 0x20044A05DD422180ULL);
    if (m[0x14] == 2) {
        func_00207E40(1, 0x7080, 0x7900, 0x100, 0x40, 0x70808080, 0x200440859D3221C0ULL);
    } else {
        func_00207E40(1, 0x7080, 0x7900, 0x100, 0x40, 0x70808080, 0x200440059D3221A0ULL);
    }
    if (m[0x15] == 2) {
        func_00207E40(1, 0x7000, float_to_int(16.0f * (float)(((m[0xA] * 24 + 0x72) >> 1) + 0x790)),
                      0x100, 0x40, 0x40808080, 0x20044405A1322100ULL);
        func_00207E40(1, 0x8000, float_to_int(16.0f * (float)(((m[0xA] * 24 + 0x72) >> 1) + 0x790)),
                      0x100, 0x40, 0x40808080, 0x20044485A1322140ULL);
    }
}
