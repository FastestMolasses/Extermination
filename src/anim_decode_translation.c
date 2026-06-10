// INCLUDE_ASM anim_decode_translation  (vram unknown, 204 bytes)
// UNDECOMPILED placeholder. The byte-identical machine code for this
// function is assembled from the local splat disassembly (git-ignored;
// regenerate with `build.py setup` from your own disc) and linked by
// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or
// without this file. build.py does NOT compile INCLUDE_ASM stubs.
//
// To decompile: replace this file with C that compiles byte-identical,
// verified with objdiff against build/expected/anim_decode_translation.o. See
// docs/PROGRESS.md for the matching idioms and the function index in
// docs/FUNCTIONS.csv.
//
// ---------------------------------------------------------------------------
// BEST ATTEMPT 2026-06-09: 87.04% (objdiff, -O4,p -sdatathreshold 0).
//
// Semantics (verified against docs/FINDINGS.md "Keyframe stream format"):
// translation keyframe payload decoder. Unpacks three 26-bit channels
// (tx/ty/tz) from the 10-byte packed record at `src` into three VU0
// scratchpad words (0x70003600/04/08), shifts each left by 6 so the channel
// occupies the TOP bits (truncated IEEE-754 float: top 26 bits kept, low 6
// mantissa bits zero), then bit-casts each word out as floats to out[0..2].
//
// WALL (new datum, also blocks func_001C84D0 at 92.6%):
// "address-pair shadow-fill hoist". For back-to-back volatile-scratchpad
// blocks, mwcc's -O3+ global scheduler hoists EVERY independent lui/addiu
// address-materialization pair maximally early — it packs as many pairs as
// fit into the FIRST lhu load-shadow, moving them past volatile stores and
// allocating fresh temp registers as needed. CW 2.3.1 places at most ONE
// pair per load cluster, each pair waiting until a previously-used temp
// register dies (allocation-constrained scheduling). Falsified fixes:
// pointer locals at top / interleaved / after their store (placement is
// identical each time = maximal hoist); fake trailing parameters pinning
// $a2/$a3 (idiom 7) — mwcc just hoists the pair into the next free register
// ($t1 etc.), so pinning only displaces the mismatch; -O3,p / -O4 /
// -O4,p -opt nointerleave (identical output); -O2,p (much worse, 48%).
// The 47/54 matching rows include all three packing blocks, all RMW <<6
// shifts and the lwc1/swc1 bit-cast tail; only the hoisted pair rows and
// the block-1 temp registers they displace diverge.
//
// Matching notes that DO work here (reusable):
// * `// CFLAGS: -O4,p -sdatathreshold 0` forces CW-style absolute
//   addressing: mwcc then emits `lui $at; op %lo($at)` for direct global
//   accesses, byte-identical to CW's $at store/load form (raw constants,
//   no reloc — matches splat's unsymbolized $at rows).
// * `*(volatile int *)0x7000NNNN` constant-address volatile accesses
//   reproduce CW's $at form with no relocation, while `&D_7000NNNN` extern
//   accesses produce the relocated lui/addiu pointer materializations —
//   both forms coexist in the original function.
// * `out[k] = *(volatile float *)&D_7000NNNN;` reproduces CW's
//   lui/addiu/lwc1 0x0(reg) shape exactly (mwcc materializes the full
//   address instead of folding %lo into the lwc1).
//
#if 0
extern volatile int D_70003600;
extern volatile int D_70003604;
extern volatile int D_70003608;

void anim_decode_translation(unsigned short *src, float *out) {
    *(volatile int *)0x70003600 = src[0] | (src[1] << 16);
    *(volatile int *)0x70003604 = (src[3] << 22) | ((src[1] >> 10) | (src[2] << 6));
    *(volatile int *)0x70003608 = (src[3] >> 4) | (src[4] << 12);
    *(volatile int *)0x70003600 <<= 6;
    *(volatile int *)0x70003604 <<= 6;
    *(volatile int *)0x70003608 <<= 6;
    out[0] = *(volatile float *)&D_70003600;
    out[1] = *(volatile float *)&D_70003604;
    out[2] = *(volatile float *)&D_70003608;
}
#endif
