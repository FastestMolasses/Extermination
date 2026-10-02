// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code
//  is linked 0x40 below where it runs), 0x54 bytes.
// Byte-identical: the compiled .text equals the original 0x54 bytes at link
//  0x00823540 (a splat piece inside the entry-pad group; build/ovlc/piece.py).
// Role: helper with no static reference (the twin of 0x8235E0): the +0x110
//  object's +0x7C falls by 0.2; returns 1 below -9.0.
int overlay_AREA08_func_00823540(unsigned char *self) {
    *(float *)(*(unsigned char **)(self + 0x110) + 0x7C) -= 0.2f;
    if (*(float *)(*(unsigned char **)(self + 0x110) + 0x7C) < -9.0f) {
        return 1;
    }
    return 0;
}
