// CFLAGS: -O4,p -sdatathreshold 0
typedef float f32;

void func_001CA1C0(f32 *m, f32 *q, f32 *t)
{
    volatile f32 *T = (volatile f32 *)0x70003760;
    f32 two = 2.0f;
    f32 one = 1.0f;
    int w = 0x3F800000;

    T[0] = q[0] * q[0];   /* 0x60 = xx */
    T[1] = q[1] * q[1];   /* 0x64 = yy */
    T[2] = q[2] * q[2];   /* 0x68 = zz */
    T[4] = q[0] * q[1];   /* 0x70 = xy */
    T[5] = q[0] * q[2];   /* 0x74 = xz */
    T[6] = q[1] * q[2];   /* 0x78 = yz */
    T[8] = q[3] * q[0];   /* 0x80 = wx */
    T[9] = q[3] * q[1];   /* 0x84 = wy */
    T[10] = q[3] * q[2];  /* 0x88 = wz */

    m[0] = one - two * (T[1] + T[2]);
    m[1] = two * (T[4] - T[10]);
    m[2] = two * (T[5] + T[9]);
    m[3] = 0;
    m[4] = two * (T[4] + T[10]);
    m[5] = one - two * (T[0] + T[2]);
    m[6] = two * (T[6] - T[8]);
    m[7] = 0;
    m[8] = two * (T[5] - T[9]);
    m[9] = two * (T[6] + T[8]);
    m[10] = one - two * (T[0] + T[1]);
    m[11] = 0;
    m[12] = t[0];
    m[13] = t[1];
    m[14] = t[2];
    *(int *)&m[15] = w;
}
