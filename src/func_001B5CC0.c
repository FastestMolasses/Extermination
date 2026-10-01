// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Analog stick deflection level. x and y are the raw stick bytes (0x80 =
// centre); the distance from the centre, sqrtf(dx*dx + dy*dy) through
// func_0011E748, is graded 0 (<= 48), 1 (<= 88), 2 (<= 122) or 3.
extern float func_0011E748(float x);

int func_001B5CC0(unsigned char x, unsigned char y) {
    int dx = x - 0x80;
    int dy = y - 0x80;
    float d = func_0011E748((float)(dx * dx + dy * dy));

    if (d <= 48.0f) {
        return 0;
    }
    if (d <= 88.0f) {
        return 1;
    }
    if (d <= 122.0f) {
        return 2;
    }
    return 3;
}
