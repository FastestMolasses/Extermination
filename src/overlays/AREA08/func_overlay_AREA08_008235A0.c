// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x008235E0 (splat/link name 008235A0; overlay code
//  is linked 0x40 below where it runs), 0x4C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Role: script callback (script 0x825880, op09 record 0x825A00): the +0x110
//  object's +0x7C rises by 0.2; returns 1 above 0.0.
int func_overlay_AREA08_008235A0(unsigned char *self) {
    *(float *)(*(unsigned char **)(self + 0x110) + 0x7C) += 0.2f;
    if (*(float *)(*(unsigned char **)(self + 0x110) + 0x7C) > 0.0f) {
        return 1;
    }
    return 0;
}
