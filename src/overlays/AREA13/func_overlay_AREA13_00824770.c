// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008247B0 (splat/link name 00824770; overlay code
//  is linked 0x40 below where it runs), 0x10C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: script callback (script 0x82B810, record 0x82B9D0): 0x8246D0 with
//  D_00810350 moved by -0.7 and D_00810358 by -0.7 per call.
extern float func_0011DE90(float a);

int func_overlay_AREA13_00824770(void *ctx, unsigned char *self, unsigned char *blk) {
    switch (self[4]) {
    case 0:
        *(float *)(blk + 0x10) = 0.0f;
        self[4] = 1;
        break;
    case 1:
        *(float *)(blk + 0x10) += 1.0f;
        *(float *)0x810350 -= 0.7f;
        *(float *)0x810354 += 0.2f * func_0011DE90(0.034906585f * *(float *)(blk + 0x10));
        *(float *)0x810358 -= 0.7f;
        if (!(*(float *)(blk + 0x10) <= 50.0f)) {
            *(float *)(blk + 0x10) = 0.0f;
            return 1;
        }
        break;
    }
    return 0;
}
