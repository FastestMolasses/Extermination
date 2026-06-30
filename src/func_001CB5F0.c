// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Appends a 0x20-byte DMA/GIF command block into the global ring buffer pointed
// to by D_00275670 (the +0x18 cursor field). arg1 is a 12-bit page address that
// is range-clamped: passed through unchanged if it equals 0xFFF000, otherwise
// floored at 0 and capped at 0xFFB000, then >>12 to a page index scaled *4 to
// index the page table at arg0. The new block (base+0x100): +0x100 = 0x20000000
// tag, +0x104 = low 28 bits of the block's own +0x110 self-pointer, +0x110 =
// arg2 | 0x20000000. The page slot at temp_a1: if already non-zero it chains the
// low-28-bit prev value into the block's +0x14; else the head pointer at +0x4000
// is set. Returns block+0x20; the cursor is advanced by (arg2+2)*0x10.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 only reaches
// 91.87% (tail register-allocation order). Sibling of func_001CB6B0/func_001CB760.
// Key idioms: `arg1 = arg1 >> 0xC;` reassignment after the !=0xFFF000 guard ->
// branch-likely `beql; sra` (idiom-13); upper clamp as `> 0xFFB000U` (not
// `>= 0xFFB001`) -> idiom-13b dead-const two distinct `lui 0xff`; low-28-bit
// mask as `(unsigned)(x & 0xFFFFFFFULL)` -> dsll32/dsrl32 pair; the +0x104 value
// derived from `blk + 0x10` (so mwcc reuses the early `addiu a3,a0,0x100`); the
// final cursor advance written as `+=` so mwcc colors the D_00275670 reload into
// a0 / the *0x10 delta into a1 like CW. D_00275670 is gp-relative (sdatathreshold 4).
extern char *D_00275670;

char *func_001CB5F0(int arg0, int arg1, int arg2) {
    char *temp_a1;
    char *base;
    char *blk;
    int v0;
    if (arg1 != 0xFFF000) {
        if (arg1 < 0) { arg1 = 0; }
        if ((unsigned int)arg1 > 0xFFB000U) { arg1 = 0xFFB000; }
    }
    arg1 = arg1 >> 0xC;
    temp_a1 = (char *)(arg0 + (arg1 * 4));
    base = *(char **)(D_00275670 + 0x18);
    blk = base + 0x100;
    *(int *)(base + 0x100) = 0x20000000;
    *(unsigned int *)(base + 0x104) = (unsigned int)((unsigned long long)(blk + 0x10) & 0xFFFFFFFULL);
    *(int *)(base + 0x110) = arg2 | 0x20000000;
    v0 = *(int *)temp_a1;
    if (v0 != 0) {
        *(unsigned int *)(blk + 0x14) = (unsigned int)((unsigned long long)v0 & 0xFFFFFFFULL);
    } else {
        *(char **)(temp_a1 + 0x4000) = blk;
    }
    *(char **)temp_a1 = blk;
    *(char **)(D_00275670 + 0x18) += (arg2 + 2) * 0x10;
    return blk + 0x20;
}
