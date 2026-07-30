// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sibling of anim_decode_translation: translation keyframe payload decoder.
// Unpacks three 26-bit channels (tx/ty/tz) from the 10-byte packed record at
// `src` into three VU0 scratchpad words (0x70003600/04/08), shifts each left
// by 6 so the channel occupies the TOP bits (truncated IEEE-754 single, low 6
// mantissa bits zero), then bit-casts each word out as a float to out[0..2].
//
// WALL on the pinned 991202 build (87.04%): the "address-pair shadow-fill
// hoist" -- 991202's global scheduler hoists the lui/addiu address pairs into
// the first lhu load shadow and cascades subsequent pairs one slot early.
// mwcc 2.3.3 schedules the pairs exactly as CW 2.3.1 did, giving objdiff 100.0
// with no C change (identical fix as func_001C84D0/anim_decode_translation).
extern volatile int D_70003600;
extern volatile int D_70003604;
extern volatile int D_70003608;

void anim_decode_translation(unsigned short *src, float *out) {
    *(volatile int *)0x70003600 = src[0] | (src[1] << 16);
    *(volatile int *)0x70003604 = (src[3] << 22) | (((int)src[1] >> 10) | (src[2] << 6));
    *(volatile int *)0x70003608 = ((int)src[3] >> 4) | (src[4] << 12);
    *(volatile int *)0x70003600 <<= 6;
    *(volatile int *)0x70003604 <<= 6;
    *(volatile int *)0x70003608 <<= 6;
    out[0] = *(volatile float *)&D_70003600;
    out[1] = *(volatile float *)&D_70003604;
    out[2] = *(volatile float *)&D_70003608;
}
