// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Converts an analog stick byte to a d-pad direction bit for one axis:
// axis 0 (X) gives 0x8000 (left) below 0x10 and 0x2000 (right) above 0xE0;
// any other axis (Y) gives 0x1000 (up) and 0x4000 (down). 0 in between.
int func_001B5D70(unsigned char v, int axis) {
    int ret = 0;
    unsigned short lo;
    unsigned short hi;

    if (axis == 0) {
        lo = 0x8000;
        hi = 0x2000;
    } else {
        lo = 0x1000;
        hi = 0x4000;
    }
    if (v < 0x10) {
        ret = lo;
    } else if (v > 0xE0) {
        ret = hi;
    }
    return ret;
}
