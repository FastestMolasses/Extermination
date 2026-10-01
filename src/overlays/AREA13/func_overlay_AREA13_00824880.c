// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008248C0 (splat/link name 00824880; overlay code
//  is linked 0x40 below where it runs), 0x94 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: script callback (scripts 0x82B3D0 and 0x82B810): moves D_00810358 by
//  -0.7 per call and returns 1 once the count passes 45.
int func_overlay_AREA13_00824880(void *ctx, unsigned char *self, unsigned char *blk) {
    switch (self[4]) {
    case 0:
        *(float *)(blk + 0x10) = 0.0f;
        self[4] = 1;
        break;
    case 1:
        *(float *)(blk + 0x10) += 1.0f;
        *(float *)0x810358 -= 0.7f;
        if (!(*(float *)(blk + 0x10) <= 45.0f)) {
            *(float *)(blk + 0x10) = 0.0f;
            return 1;
        }
        break;
    }
    return 0;
}
