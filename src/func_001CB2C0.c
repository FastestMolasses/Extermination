// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 83.72% NEARMISS).
// Fixes read from the original instructions:
// - Spelled in the matched packet family's form (func_001D6580): `char **D_00275670`,
//   slot = &T[idx], cursor slot[4], and the body pointer (p + 0x10) taken BEFORE the
//   cursor advance. That ordering, not a scheduler wall, puts the body pointer ahead
//   of the zero store at +0x10.
// - func_00102948 declared with its real pointer parameters (dst, src).
//
// Builds a GS/DMA packet in the slot table. base = *(arg0+0x90). slot = the
// gp-relative table D_00275670 indexed by arg2. The current packet pointer lives
// at slot[4]: writes the tag header (byte +3 = 0x10, word +4 = 0, half +0 = 3),
// captures p = slot[4] and the body pointer p + 0x10, advances slot[4] by 0x40,
// zeroes +0x10/+0x14, writes 0x01000404 at +0x18 and (arg1 | 0x6C020000) at +0x1C,
// then copies two quadwords with func_00102948: (body+0x10, base+0x40) and
// (body+0x20, base+0x50). D_00275670 is reached gp-relative (-sdatathreshold 8).
extern char **D_00275670;
extern void func_00102948(void *dst, void *src);

void func_001CB2C0(char *arg0, int arg1, int arg2) {
    char *base;
    char **slot;
    char *p;
    char *s1;

    base = *(char **)(arg0 + 0x90);
    slot = (char **)(&D_00275670[arg2]);
    *(unsigned char *)(slot[4] + 3) = 0x10;
    *(int *)(slot[4] + 4) = 0;
    *(short *)(slot[4] + 0) = 3;
    p = slot[4];
    s1 = p + 0x10;
    slot[4] = p + 0x40;
    *(int *)(p + 0x10) = 0;
    *(int *)(p + 0x14) = 0;
    *(int *)(p + 0x18) = 0x01000404;
    *(int *)(p + 0x1C) = arg1 | 0x6C020000;
    func_00102948(s1 + 0x10, base + 0x40);
    func_00102948(s1 + 0x20, base + 0x50);
}
