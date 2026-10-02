// NEARMISS func_001D6DD0  (vram 0x001D6DD0, 0x90 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 40.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// the original computes the packed (x, y) doubleword before it
// reads the slot pointer and keeps its temporaries in t0..t3; mwcc 2.3.3 schedules the
// sign-extensions after the header writes and allocates a0..a3 / t-registers
// differently (mwcc 991202 reaches 69.00, 2.4 52.22, with the same C).
//
// The function links from the asm body in src/func_001D6DD0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Appends one GS register packet to the slot's chain (as func_001D7000) whose data
// word is (long long)x | ((long long)y << 32) for GS register 0x18, and
// returns the packet's data address (write pointer + 0x10).
typedef struct { char pad[0x10]; char *cur[1]; /* +0x10: one write pointer per slot */ } Ctx;
extern Ctx *D_00275670;
char *func_001D6DD0(int slot, int x, int y) {
    long long packed = ((long long)(int)y << 32) | (long long)(int)x;
    Ctx *c = D_00275670;
    char *p;
    c->cur[slot][3] = 0x10;
    *(int *)(c->cur[slot] + 4) = 0;
    *(short *)(c->cur[slot] + 0) = 3;
    p = c->cur[slot];
    c->cur[slot] = p + 0x40;
    *(unsigned __int128 *)(p + 0x10) = 0;
    *(int *)(p + 0x1C) = 0x50000002;
    *(long long *)(p + 0x20) = 0x8001LL | ((long long)0x10000000 << 32);
    *(long long *)(p + 0x28) = 0xE;
    *(long long *)(p + 0x30) = packed;
    *(long long *)(p + 0x38) = 0x18;
    return p + 0x10;
}
