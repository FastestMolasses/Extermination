// NEARMISS func_00204250  (vram 0x00204250, 0x138 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 100.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// The original keeps r in v0 across the calls to the leaf func_002041A0 (intra-TU register
// analysis); the match needs a static copy of the callee in this TU, whose extra .text the
// per-function link cannot take.
//
// The function links from the asm body in src/func_00204250.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Movie decoder: resets the IPU input ring r and points DMA channel 4
// (toIPU) at it. Clears the cursors +0x0C / +0x10 / +0x14 and +0x58 / +0x5C,
// sets +0x44 = 1, marks every 0x18-byte sector record (+0x50 array, +0x54
// entries) empty (two -1 doublewords, two zero words), builds a ref DMA tag
// per sector in the tag array +4 (sector i at base + i * 0x800, 0x80
// quadwords) closed by a next tag back to the first, then sets the channel's
// QWC = 0, MADR = base and TADR = the tag array and starts it with
// func_00204140(5). Returns 1.
// The original keeps r in v0 across the calls to the leaf func_002041A0
// (intra-TU register analysis), so the callee is copied here as a static of
// the same TU.
static void func_002041A0(unsigned long long *out, unsigned int a, unsigned int b, unsigned int c) {
    *out = ((unsigned long long)a << 32) | ((unsigned long long)b << 28) | (unsigned long long)c;
}

extern void func_00204140(int mode);

int func_00204250(int *r) {
    int i;
    int ofs;
    int addr;
    int tag;

    r[3] = 0;
    r[4] = 0;
    r[5] = 0;
    r[0x11] = 1;
    r[0x16] = 0;
    r[0x17] = 0;
    for (i = 0, ofs = 0; i < r[0x15]; i++, ofs += 0x18) {
        *(long long *)(r[0x14] + ofs) = -1;
        *(long long *)(r[0x14] + ofs + 8) = -1;
        *(int *)(r[0x14] + ofs + 0x10) = 0;
        *(int *)(r[0x14] + ofs + 0x14) = 0;
    }
    for (i = 0, addr = 0, tag = 0; i < r[2]; addr += 0x800, tag += 0x10, i++) {
        func_002041A0((unsigned long long *)(r[1] + tag), (r[0] + addr) & 0x0FFFFFFF, 3, 0x80);
    }
    func_002041A0((unsigned long long *)(r[1] + (i << 4)), r[1] & 0x0FFFFFFF, 2, 0);
    *(volatile int *)0x1000B420 = 0;
    *(volatile int *)0x1000B410 = r[0] & 0x0FFFFFFF;
    *(volatile int *)0x1000B430 = r[1] & 0x0FFFFFFF;
    func_00204140(5);
    return 1;
}
