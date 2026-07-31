// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Proximity/awareness test between actor `self` and actor `other`: if `other` is
// still active (bit 1 of other[0] clear), measure the 3D distance between the two
// position vectors at +0xB0/+0xB4/+0xB8, look up `other`'s alert radius from its
// kind byte at other[3] (0,4 -> 15.0f; 1,5,6,7 -> 20.0f; everything else 0.0f =
// "never alerts"), and when the distance is inside a non-zero radius set bit 0 of
// other[0xA], the "has noticed self" flag.
//
// Shape notes (load-bearing for the match): the `oy` pointer temp for other's Y
// and the dx/dz/dy declaration order are what give mwcc the target's FP register
// coloring (dy in $f3, dz in $f2) instead of the swapped one.

extern float func_0011E748(float x);

void func_001A9C40(char *self, char *other) {
    float *oy;
    float dist;
    float radius;
    float dx;
    float dz;
    float dy;

    if (*(unsigned char *)other & 2) {
        return;
    }
    dx = *(float *)(self + 0xB0) - *(float *)(other + 0xB0);
    oy = (float *)(other + 0xB4);
    dy = *(float *)(self + 0xB4) - *oy;
    dz = *(float *)(self + 0xB8) - *(float *)(other + 0xB8);
    dist = func_0011E748(((dx * dx) + (dy * dy)) + (dz * dz));

    radius = 0.0f;
    switch (*(unsigned char *)(other + 3)) {
    case 0:
    case 4:
        radius = 15.0f;
        break;
    case 1:
    case 5:
    case 6:
    case 7:
        radius = 20.0f;
        break;
    }

    if ((radius != 0.0f) && (dist <= radius)) {
        *(unsigned char *)(other + 0xA) |= 1;
    }
}
