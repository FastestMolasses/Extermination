// NEARMISS func_001D7000  (vram 0x001D7000, 0x7C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 98.39% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// register allocation only: the original holds slot * 4 in v1,
// the entry in t2 and the first write pointer in v0; mwcc 2.3.3 picks a2 / a6 / a4
// (char-pointer, struct, index, local-order and int-to-64 spellings and mwcc 991202 /
// 2.4 measured: 82.06 / 94.52).
//
// The function links from the asm body in src/func_001D7000.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Appends one GS register packet to the slot's chain of the render context
// D_00275670 (the same 0x40-byte layout as func_00207D90): header bytes, a zero
// quadword, the tag word 0x50000002, the GIF tag 0x1000000000008001 / 0xE, and the
// data word (int)v sign-extended to 64 bits for GS register 0x3B; the slot's write
// pointer advances by 0x40.
typedef struct { char pad[0x10]; char *cur[1]; /* +0x10: one write pointer per slot */ } Ctx;
extern Ctx *D_00275670;
void func_001D7000(int slot, int v) {
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
    *(long long *)(p + 0x30) = (long long)(int)v;
    *(long long *)(p + 0x38) = 0x3B;
}
