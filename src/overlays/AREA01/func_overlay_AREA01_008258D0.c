// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825910 (splat/link name 008258D0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: op09 callback of script 0x82AC10 (record 0x82ACD0); true while +0x3C
//  <= 505.0.
int func_overlay_AREA01_008258D0(char *self) {
    if (*(float *)(self + 0x3C) <= 505.0f) {
        return 1;
    }
    return 0;
}
