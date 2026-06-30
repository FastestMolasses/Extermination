// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player position-region classifier. Reads the player's world XYZ from
// q[0x28]/q[0x29]/q[0x2A] (X/Y/Z). For each of 5 circular regions (center cx,cz,
// radius 8 -> r^2 = 64), if the player is inside: tag p[2] with the region index,
// clamp the Y-altitude p[0x14] to a per-region ceiling, and if Y exceeds a
// per-region threshold fire an event (func_001B0C60) and set p[6] = 7.
// Falls through to region 5 (p[2] = 5) when outside all circles. Gated off by
// the global D_008106B8.
extern unsigned char D_008106B8;
extern void func_001B0C60(int, int, int);

void func_00196970(char *p, float *q) {
    float x;
    float z;

    if (D_008106B8 != 0) {
        return;
    }

    x = q[0x28];
    z = q[0x2A];

    if ((x - 710.0f) * (x - 710.0f) + (z - 1266.1f) * (z - 1266.1f) < 64.0f) {
        *(char *)(p + 2) = 0;
        if (!(*(float *)(p + 0x14) <= 268.1f)) {
            *(float *)(p + 0x14) = 268.1f;
        }
        if (!(q[0x29] < 284.5f)) {
            func_001B0C60(0xD, 0, 6);
            *(char *)(p + 6) = 7;
        }
        return;
    }

    if ((x - 907.0f) * (x - 907.0f) + (z - 960.2f) * (z - 960.2f) < 64.0f) {
        *(char *)(p + 2) = 1;
        if (!(*(float *)(p + 0x14) <= 306.1f)) {
            *(float *)(p + 0x14) = 306.1f;
        }
        if (!(q[0x29] < 314.0f)) {
            func_001B0C60(0x13, 1, 8);
            *(char *)(p + 6) = 7;
        }
        return;
    }

    if ((x - 925.1f) * (x - 925.1f) + (z - 859.7f) * (z - 859.7f) < 64.0f) {
        *(char *)(p + 2) = 2;
        if (!(*(float *)(p + 0x14) <= 308.0f)) {
            *(float *)(p + 0x14) = 308.0f;
        }
        if (!(q[0x29] < 314.0f)) {
            func_001B0C60(0x13, 1, 7);
            *(char *)(p + 6) = 7;
        }
        return;
    }

    if ((x - 1077.2f) * (x - 1077.2f) + (z - 845.0f) * (z - 845.0f) < 64.0f) {
        *(char *)(p + 2) = 3;
        if (!(*(float *)(p + 0x14) <= 242.5f)) {
            *(float *)(p + 0x14) = 242.5f;
        }
        if (!(q[0x29] < 252.0f)) {
            func_001B0C60(0xD, 0, 7);
            *(char *)(p + 6) = 7;
        }
        return;
    }

    if ((x - 855.1f) * (x - 855.1f) + (z - 851.3f) * (z - 851.3f) < 64.0f) {
        *(char *)(p + 2) = 4;
        if (!(*(float *)(p + 0x14) <= 309.0f)) {
            *(float *)(p + 0x14) = 309.0f;
        }
        if (!(q[0x29] < 314.0f)) {
            func_001B0C60(0x13, 1, 6);
            *(char *)(p + 6) = 7;
        }
        return;
    }

    *(char *)(p + 2) = 5;
}
