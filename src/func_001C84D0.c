// INCLUDE_ASM func_001C84D0  (vram 0x001C84D0, 256 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/func_001C84D0.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// ---------------------------------------------------------------------------
// BEST ATTEMPT 2026-06-09: 92.56% (objdiff, -O4,p -sdatathreshold 0).
//
// Semantics (verified against docs/FINDINGS.md "Keyframe stream format"):
// rotation keyframe payload decoder. Unpacks four 20-bit channels
// (qx/qy/qz/qw) from the 10-byte packed record at `src`; per channel:
// stitch a 32-bit word from lhu halves into a VU0 scratchpad word
// (0x70003600/04/08/0C), shift left 12 so the channel's 20 bits become the
// top of an IEEE-754 single (sign+exp+11 mantissa bits, rest zero), then
// bit-cast the word to out[0..3] via lwc1/swc1.
//
// WALL: same "address-pair shadow-fill hoist" as anim_decode_translation
// (see that file for the full datum and falsified-fix list). Here CW itself
// packs two address pairs into the channel-0 RMW shadow, but mwcc hoists
// one pair FURTHER into the very first lhu shadow; every later pair then
// shifts one slot early and the block-1 temp registers cascade. Pinning the
// block-1 temps in $a2/$a3 with fake trailing params + compound shifts only
// pushed the hoisted pair into $t0/$t1 (91.6%). All four channel-packing
// blocks, every RMW <<12, and the bit-cast tail match.
//
#if 0
extern volatile int D_70003600;
extern volatile int D_70003604;
extern volatile int D_70003608;
extern volatile int D_7000360C;

void func_001C84D0(unsigned short *src, float *out) {
    *(volatile int *)0x70003600 = src[0] | (src[1] << 16);
    *(volatile int *)0x70003600 <<= 12;
    out[0] = *(volatile float *)&D_70003600;
    *(volatile int *)0x70003604 = (src[1] >> 4) | (src[2] << 12);
    *(volatile int *)0x70003604 <<= 12;
    out[1] = *(volatile float *)&D_70003604;
    *(volatile int *)0x70003608 = (src[2] >> 8) | (src[3] << 8);
    *(volatile int *)0x70003608 <<= 12;
    out[2] = *(volatile float *)&D_70003608;
    *(volatile int *)0x7000360C = (src[3] >> 12) | (src[4] << 4);
    *(volatile int *)0x7000360C <<= 12;
    out[3] = *(volatile float *)&D_7000360C;
}
#endif
