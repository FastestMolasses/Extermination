// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x7000319C 0x700031AC 0x70003680 0x70003684
extern float D_7000319C;
extern float D_700031AC;
extern float D_70003680;
extern float D_70003684;
extern float func_00102738(float *a, float *b);           /* VU0 dot product (xyz) */
extern void func_00103230(float *dst, float *a, float s); /* VU0 dst = a * s */
extern void func_001028B8(float *dst, float *a, float *b); /* VU0 dst = a + b */
extern void func_001028D0(float *dst, float *a, float *b); /* VU0 dst = a - b */
extern float func_0011E748(float);                        /* sqrtf */
extern float func_0011DF78(float);                        /* fabsf */
extern float func_0011DBB8(float);                        /* atanf */

int func_001A58B0(float *q, float *out, unsigned char *p) {
    float *n = (float *)(p + 4);
    float *vert;
    float *edge;
    float along;
    int i;
    float pos[4];
    float dir[4];
    float nrm[4];
    float hit[4];
    float rel[4];
    float v[4];

    for (i = 0; i < 3; i++) {
        nrm[i] = n[i];
    }
    pos[0] = q[0];
    pos[2] = q[1];
    dir[2] = 0.0f;
    dir[0] = 0.0f;
    dir[3] = 0.0f;
    pos[3] = 0.0f;
    dir[1] = 1.0f;
    nrm[3] = 0.0f;
    pos[1] = 0.0f;
    along = func_00102738(dir, nrm);
    func_00103230(hit, dir, (n[3] - func_00102738(nrm, pos)) / along);
    func_001028B8(hit, pos, hit);

    vert = n + 4;
    edge = n + (p[2] * 3 + 4);
    for (i = 0; i < p[2]; i++) {
        v[0] = vert[0];
        v[1] = vert[1];
        v[2] = vert[2];
        v[3] = 0.0f;
        func_001028D0(rel, hit, v);
        v[0] = edge[0];
        v[1] = edge[1];
        v[2] = edge[2];
        v[3] = 0.0f;
        if (!(func_00102738(rel, v) <= 1e-05f)) {
            return 0;
        }
        vert += 3;
        edge += 3;
    }

    D_70003680 = func_0011E748(nrm[0] * nrm[0] + nrm[2] * nrm[2]);
    if (D_70003680 < 0.0001f) {
        D_70003684 = 3.4e38f;
    } else {
        D_70003684 = func_0011DF78(nrm[1]) / D_70003680;
    }

    if (!(n[1] <= 0.0f)) {
        out[0] = hit[1];
        out[1] = 3.4e38f;
        out[2] = 1.0f;
        out[3] = 0.0f;
        if (nrm[1] < 0.0f) {
            D_7000319C = -(1.5707964f - func_0011DBB8(D_70003684));
        } else {
            D_7000319C = 1.5707964f - func_0011DBB8(D_70003684);
        }
    } else {
        out[0] = -3.4e38f;
        out[1] = hit[1];
        out[2] = 0.0f;
        out[3] = -1.0f;
        if (nrm[1] < 0.0f) {
            D_700031AC = -(1.5707964f - func_0011DBB8(D_70003684));
        } else {
            D_700031AC = 1.5707964f - func_0011DBB8(D_70003684);
        }
    }
    return 1;
}
