// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Shadow-decal clipper outcode for one axis (port SHADOW_DECAL.md section 1:
// em_shadow_decal_001CF870). Each vertex carries clip coordinates at +0x40
// (x, y, z, w). For vertex a: bit 0 when its axis coordinate is above |w|,
// bit 1 when below -|w|; for vertex b the same tests set bits 4 and 5.
// func_0011DF78 is fabsf.
typedef struct ClipVtx {
    char pad[0x40];
    float clip[4];  /* 0x40: x, y, z, w */
} ClipVtx;

extern float func_0011DF78(float x);

unsigned char func_001CF870(ClipVtx *a, ClipVtx *b, int axis) {
    unsigned char code = 0;

    if (a->clip[axis] > func_0011DF78(a->clip[3])) {
        code |= 1;
    }
    if (a->clip[axis] < -func_0011DF78(a->clip[3])) {
        code |= 2;
    }
    if (b->clip[axis] > func_0011DF78(b->clip[3])) {
        code |= 0x10;
    }
    if (b->clip[axis] < -func_0011DF78(b->clip[3])) {
        code |= 0x20;
    }
    return code;
}
