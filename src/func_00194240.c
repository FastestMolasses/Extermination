// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Proximity zone check: arg0 is an entity whose world position is at
// float fields 0xA0 (X), 0xA4 (Y), 0xA8 (Z). For each of 4 waypoints,
// tests squared horizontal distance (dx*dx + dz*dz) < 225.0 (15^2) AND
// vertical |Y - cy| < 4.0 (via fabsf func_0011DF78). On a hit it writes a
// zone id to D_008106F2 and returns 1; falls through to 0 if none match.
extern float func_0011DF78(float);
extern unsigned char D_008106F2;

int func_00194240(float *p) {
    float dx, dz;

    dx = p[0x28] - 1017.9f;
    dz = p[0x2A] - 1030.9f;
    if (dx * dx + dz * dz < 225.0f && func_0011DF78(p[0x29] - 223.0f) < 4.0f) {
        D_008106F2 = 6;
        return 1;
    }
    dx = p[0x28] - 997.2f;
    dz = p[0x2A] - 929.8f;
    if (dx * dx + dz * dz < 225.0f && func_0011DF78(p[0x29] - 198.0f) < 4.0f) {
        D_008106F2 = 1;
        return 1;
    }
    dx = p[0x28] - 896.5f;
    dz = p[0x2A] - 930.2f;
    if (dx * dx + dz * dz < 225.0f && func_0011DF78(p[0x29] - 197.0f) < 4.0f) {
        D_008106F2 = 2;
        return 1;
    }
    dx = p[0x28] - 760.7f;
    dz = p[0x2A] - 883.1f;
    if (dx * dx + dz * dz < 225.0f && func_0011DF78(p[0x29] - 268.0f) < 4.0f) {
        D_008106F2 = 3;
        return 1;
    }
    return 0;
}
