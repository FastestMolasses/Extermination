// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Rotation matrix to quaternion (x, y, z, w). m is a 4x4 float matrix
// (rows of four). With a positive trace the standard w-major form is used;
// otherwise the largest diagonal element picks the major axis (x, then y, then
// z; ties keep the earlier axis). func_0011E748 is sqrtf.
extern float func_0011E748(float x);

void func_001C9E40(float *q, float *m) {
    float m00 = m[0];
    float m11 = m[5];
    float m22 = m[10];
    float tr = m00 + m11 + m22;
    float s;
    float k;
    float big;
    int i;

    i = 0;
    if (tr > 0.0f) {
        s = func_0011E748(tr + 1.0f);
        q[3] = 0.5f * s;
        k = 0.5f / s;
        q[0] = (m[9] - m[6]) * k;
        q[1] = (m[2] - m[8]) * k;
        q[2] = (m[4] - m[1]) * k;
        return;
    }
    big = m00;
    if (m11 > big) {
        i = 1;
        big = m11;
    }
    if (m22 > big) {
        i = 2;
    }
    switch (i) {
    case 0:
        s = func_0011E748(m00 - m11 - m22 + 1.0f);
        q[0] = 0.5f * s;
        k = 0.5f / s;
        q[1] = (m[1] + m[4]) * k;
        q[2] = (m[8] + m[2]) * k;
        q[3] = (m[9] - m[6]) * k;
        break;
    case 1:
        s = func_0011E748(m11 - m00 - m22 + 1.0f);
        q[1] = 0.5f * s;
        k = 0.5f / s;
        q[2] = (m[6] + m[9]) * k;
        q[0] = (m[1] + m[4]) * k;
        q[3] = (m[2] - m[8]) * k;
        break;
    case 2:
        s = func_0011E748(m22 - m00 - m11 + 1.0f);
        q[2] = 0.5f * s;
        k = 0.5f / s;
        q[0] = (m[8] + m[2]) * k;
        q[1] = (m[6] + m[9]) * k;
        q[3] = (m[4] - m[1]) * k;
        break;
    }
}
