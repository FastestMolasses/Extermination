// NEARMISS func_001D7080  (vram 0x001D7080, 0x78 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 96.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Register allocation: same channel-cursor addressing artifact as func_001D6F60.
//
// The function links from the asm body in src/func_001D7080.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Appends a vertex colour to packet channel ch of the render context
// (D_00275670 + 0x10 + ch * 4 is the channel's cursor): a DMA cnt tag (id
// 0x10, 3 quadwords), VIF NOP + DIRECT 2, a GIF A+D tag (NLOOP 1, EOP) and
// RGBAQ = rgba with Q = q. The cursor advances 4 quadwords.
typedef unsigned int u128 __attribute__((mode(TI)));

extern char *D_00275670;

void func_001D7080(int ch, int rgba, float q_) {
    char *c = D_00275670 + ch * 4;
    char *q;

    (*(char **)(c + 0x10))[3] = 0x10;
    *(int *)(*(char **)(c + 0x10) + 4) = 0;
    *(short *)*(char **)(c + 0x10) = 3;
    q = *(char **)(c + 0x10);
    *(char **)(c + 0x10) = q + 0x40;
    *(u128 *)(q + 0x10) = 0;
    *(int *)(q + 0x1C) = 0x50000002;
    *(long long *)(q + 0x20) = 0x8001 | (long long)0x10000000 << 32;
    *(long long *)(q + 0x28) = 0xE;
    *(int *)(q + 0x30) = rgba;
    *(float *)(q + 0x34) = q_;
    *(long long *)(q + 0x38) = 1;
}
