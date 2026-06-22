// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Cylinder/range test between two points. a0 = subject position, a2 = target
// position, a1 = bounds (a1->x = horizontal radius, a1->y = vertical height).
// Returns 1 if the target is within the horizontal radius (xz plane) AND
// within the vertical height of the subject, else 0.
//   horiz = (a2.x-a0.x)^2 + (a2.z-a0.z)^2
//   if (a1.x*a1.x < horiz) return 0;
//   if (fabs(a2.y-a0.y) <= a1.y) return 1;
//   return 0;
extern float func_0011DF78(float v);

int func_001B34F0(float *a0, float *a1, float *a2) {
    float dx;
    float dz;

    dx = a2[0] - a0[0];
    dz = a2[2] - a0[2];
    if (a1[0] * a1[0] < dx * dx + dz * dz) {
        return 0;
    }
    if (func_0011DF78(a2[1] - a0[1]) > a1[1]) {
        return 0;
    }
    return 1;
}
