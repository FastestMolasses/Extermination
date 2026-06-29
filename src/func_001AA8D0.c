// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Proximity/placement check between two entities (a0=self, a1=other).
// Returns 0 if not within range; otherwise writes a spawn/placement record
// to a2 (x,y,z,flags) and an angle to a3, returning 2 (+offset side) or
// 3 (-offset side). func_0011DF78=fabs, func_0011E748=2D distance (hypot),
// func_001B1470=angle normalize (uses pi=0x40490FDB).
extern float func_0011DF78(float);
extern float func_0011E748(float);
extern float func_001B1470(float);

int func_001AA8D0(int a0, int a1, int a2, int a3) {
    float dx, dy;
    int state;

    state = 2;
    if (!(func_0011DF78(*(float *)(a0 + 0xA4) - *(float *)(a1 + 0xB4)) <= 3.0f)) {
        return 0;
    }
    dx = *(float *)(a0 + 0xA0) - (13.0f + *(float *)(a1 + 0xB0));
    dy = *(float *)(a0 + 0xA8) - *(float *)(a1 + 0xB8);
    if (!(func_0011E748(dx * dx + dy * dy) <= 3.5f)) {
        state = 3;
        dx = *(float *)(a0 + 0xA0) - (*(float *)(a1 + 0xB0) - 13.0f);
        dy = *(float *)(a0 + 0xA8) - *(float *)(a1 + 0xB8);
        if (!(func_0011E748(dx * dx + dy * dy) <= 3.5f)) {
            return 0;
        }
    }
    if (state == 2) {
        *(float *)(a2 + 0x0) = 12.6f + *(float *)(a1 + 0xB0);
        *(float *)(a2 + 0x4) = *(float *)(a0 + 0xA4);
        *(float *)(a2 + 0x8) = *(float *)(a1 + 0xB8);
        *(int *)(a2 + 0xC) = 0x3F800000;
    } else {
        *(float *)(a2 + 0x0) = *(float *)(a1 + 0xB0) - 12.6f;
        *(float *)(a2 + 0x4) = *(float *)(a0 + 0xA4);
        *(float *)(a2 + 0x8) = *(float *)(a1 + 0xB8);
        *(int *)(a2 + 0xC) = 0x3F800000;
    }
    *(float *)(a3 + 0x0) = func_001B1470(3.1415927f + *(float *)(a1 + 0xC4));
    return state;
}
