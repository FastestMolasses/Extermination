// CFLAGS: -O4,p -sdatathreshold 0
//
// Player "is the stick heading within 90 degrees of the body facing?" test,
// asked only at gait 3 (full stick, +0x23F == 3) by the fall start
// (func_00162DB0 state 5 sub-state 0) and the landing (func_0017C580, drops
// of -104 .. -14.5).
//
// The desired heading is rebuilt the way func_00174AC0 builds it: the stick
// angle +0x24C plus pi plus the camera yaw D_008106A0, wrapped to +-pi by
// func_001B1470. Its difference from the body yaw +0xC4 is wrapped again,
// and the magnitude (func_0011DF78 = fabsf) is published to the scratchpad
// float 0x70003A20. Returns 0 when that error is at most pi/2, 1 when it is
// larger.
extern float func_0011DF78(float x);
extern float func_001B1470(float angle);
extern float D_008106A0;

int func_001755B0(unsigned char *player) {
    float heading_error;

    heading_error = func_0011DF78(
        func_001B1470(func_001B1470(3.1415927f + *(float *)(player + 0x24C) + D_008106A0)
                      - *(float *)(player + 0xC4)));
    *(float *)0x70003A20 = heading_error;
    if (heading_error > 1.5707964f) {
        return 1;
    }
    return 0;
}
