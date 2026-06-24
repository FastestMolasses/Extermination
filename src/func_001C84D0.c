// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Rotation keyframe payload decoder. Unpacks four 20-bit channels
// (qx/qy/qz/qw) from the 10-byte packed record at `src`; per channel:
// stitch a 32-bit word from lhu halves into a VU0 scratchpad word
// (0x70003600/04/08/0C), shift left 12 so the channel's 20 bits become the
// top of an IEEE-754 single (sign+exp+11 mantissa bits, rest zero), then
// bit-cast the word to out[0..3] via lwc1/swc1.
//
// WALL on the pinned 991202 build (92.56%): the "address-pair shadow-fill
// hoist" — mwcc 991202 hoists one address pair into the first lhu shadow and
// every later pair cascades one slot early. mwcc 2.3.3 schedules the address
// pairs exactly as CW did, giving objdiff 100.0 with no C change.
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
