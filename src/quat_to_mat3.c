// Converts a unit quaternion (a1 = x,y,z,w) plus a translation vec3 (a2) into
// a 4x4 row-major transform matrix (a0). The nine quaternion products are
// staged in EE scratchpad (0x70003760..) before the rows are assembled, which
// is why each access reloads its absolute address.
#define XX (*(volatile float *)0x70003760)
#define YY (*(volatile float *)0x70003764)
#define ZZ (*(volatile float *)0x70003768)
#define XY (*(volatile float *)0x70003770)
#define XZ (*(volatile float *)0x70003774)
#define YZ (*(volatile float *)0x70003778)
#define WX (*(volatile float *)0x70003780)
#define WY (*(volatile float *)0x70003784)
#define WZ (*(volatile float *)0x70003788)

void quat_to_mat3(float *m, const float *q, const float *translation) {
    XX = q[0] * q[0];
    YY = q[1] * q[1];
    ZZ = q[2] * q[2];
    XY = q[0] * q[1];
    XZ = q[0] * q[2];
    YZ = q[1] * q[2];
    WX = q[3] * q[0];
    WY = q[3] * q[1];
    WZ = q[3] * q[2];

    m[0]  = 1.0f - 2.0f * (YY + ZZ);
    m[1]  =        2.0f * (XY - WZ);
    m[2]  =        2.0f * (XZ + WY);
    m[3]  = 0.0f;

    m[4]  =        2.0f * (XY + WZ);
    m[5]  = 1.0f - 2.0f * (XX + ZZ);
    m[6]  =        2.0f * (YZ - WX);
    m[7]  = 0.0f;

    m[8]  =        2.0f * (XZ - WY);
    m[9]  =        2.0f * (YZ + WX);
    m[10] = 1.0f - 2.0f * (XX + YY);
    m[11] = 0.0f;

    m[12] = translation[0];
    m[13] = translation[1];
    m[14] = translation[2];
    m[15] = 1.0f;
}
