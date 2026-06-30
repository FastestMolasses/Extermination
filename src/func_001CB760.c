// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Writes a GIF/DMA-tag-style command into the global ring/command buffer
// pointed to by D_00275670 (+0x18 cursor). Sibling of func_001CB6B0 with the
// same clamp/append structure, but here the +0x100 word is the fixed tag
// 0x50000000 (not arg|0x30000000) and arg2 (the 28-bit payload) goes to +0x104.
// arg1 is a 12-bit-page address that is range-clamped: passed through unchanged
// if it equals 0xFFF000, otherwise floored at 0 and capped at 0xFFB000, then
// >>12 to a page index scaled *4 to index the page table at arg0. The command
// words: +0x100 = 0x50000000, +0x104 = low 28 bits of arg2, +0x110 =
// 0x20000000. The page slot at *temp_a3: if already non-zero it chains the
// low-28-bit prev value into +0x14 of the new block; else the head pointer at
// +0x4000 is set. Cursor advanced by 0x20.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 build
// (which scores 90.0%). Key idioms: the `arg1 = arg1 >> 0xC;` reassignment
// after the !=0xFFF000 guard yields the branch-likely `beql; sra a1,a1,12`
// (idiom-13); the upper clamp written as `> 0xFFB000U` makes mwcc use $at for
// the throwaway 0xFFB001 compare and re-materialize 0xFFB000 (idiom-13b
// dead-const, matching CW's two distinct `lui 0xff`); the low-28-bit
// extraction as `(unsigned)(x & 0xFFFFFFFULL)` produces the target
// dsll32/dsrl32 pair. Verified TRUE objdiff 100.0 byte-identical vs
// build/expected/func_001CB760.o. D_00275670 is gp-relative (sdatathreshold 8).
extern char *D_00275670;

void func_001CB760(int arg0, int arg1, unsigned long long arg2) {
    char *temp_a1;
    char *temp_a0;
    char *temp_a3;
    int v1;
    if (arg1 != 0xFFF000) {
        if (arg1 < 0) { arg1 = 0; }
        if ((unsigned int)arg1 > 0xFFB000U) { arg1 = 0xFFB000; }
    }
    arg1 = arg1 >> 0xC;
    temp_a3 = (char *)(arg0 + (arg1 * 4));
    temp_a1 = *(char **)(D_00275670 + 0x18);
    *(int *)(temp_a1 + 0x100) = 0x50000000;
    *(unsigned int *)(temp_a1 + 0x104) = (unsigned int)(arg2 & 0xFFFFFFFULL);
    *(int *)(temp_a1 + 0x110) = 0x20000000;
    v1 = *(int *)temp_a3;
    temp_a0 = temp_a1 + 0x100;
    if (v1 != 0) {
        *(unsigned int *)(temp_a0 + 0x14) = (unsigned int)((unsigned long long)v1 & 0xFFFFFFFULL);
    } else {
        *(char **)(temp_a3 + 0x4000) = temp_a0;
    }
    *(char **)temp_a3 = temp_a0;
    *(char **)(D_00275670 + 0x18) = *(char **)(D_00275670 + 0x18) + 0x20;
}
