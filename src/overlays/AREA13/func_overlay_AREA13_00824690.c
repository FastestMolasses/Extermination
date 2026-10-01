// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008246D0 (splat/link name 00824690; overlay code
//  is linked 0x40 below where it runs), 0xE0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: script callback (script 0x82B3D0, record 0x82B550): the first call
//  zeroes the counter (third argument + 0x10) and sets the object's +4 = 1;
//  then each call adds 1 to it, moves D_00810350 by -1 and D_00810354 by 0.2
//  * func_0011DE90(0.034906585 * count), and returns 1 (counter reset) once
//  the count passes 50.
extern float func_0011DE90(float a);

int func_overlay_AREA13_00824690(void *ctx, unsigned char *self, unsigned char *blk) {
    switch (self[4]) {
    case 0:
        *(float *)(blk + 0x10) = 0.0f;
        self[4] = 1;
        break;
    case 1:
        *(float *)(blk + 0x10) += 1.0f;
        *(float *)0x810350 -= 1.0f;
        *(float *)0x810354 += 0.2f * func_0011DE90(0.034906585f * *(float *)(blk + 0x10));
        if (!(*(float *)(blk + 0x10) <= 50.0f)) {
            *(float *)(blk + 0x10) = 0.0f;
            return 1;
        }
        break;
    }
    return 0;
}
