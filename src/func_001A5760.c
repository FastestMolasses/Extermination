// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x7000319C 0x700031AC
extern float D_7000319C;
extern float D_700031AC;
// Column query against one axis-face collision record (ACTOR_COLLISION.md:
// record kind 0x2000, faces 3 / 4; port em_collision_column_box_face). Only
// faces 3 and 4 are handled. The face spans x0..x0+dx and z0..z0+dz (signed
// extents). When the query point p (x at +0, z at +4) lies strictly inside
// both spans the face's plane is written to out and 1 is returned:
// face 3: (y, 3.4e38, 1, 0) with the scratchpad epsilon 0x7000319C = 1e-8;
// face 4: (-3.4e38, y, 0, -1) with 0x700031AC = -1e-8. Otherwise 0.
typedef struct AxisFace {
    float x;    /* rec + 0x04 */
    float y;    /* rec + 0x08 */
    float z;    /* rec + 0x0C */
    float dx;   /* rec + 0x10 */
    float pad;
    float dz;   /* rec + 0x18 */
} AxisFace;

int func_001A5760(float *p, float *out, unsigned char *rec) {
    AxisFace *f = (AxisFace *)(rec + 4);
    unsigned char face = rec[2];
    float x0;
    float x1;
    float z0;
    float z1;

    if ((unsigned int)(face - 3) > 1) {
        return 0;
    }
    if (f->dx < 0.0f) {
        x1 = f->x;
        x0 = f->x + f->dx;
    } else {
        x0 = f->x;
        x1 = f->x + f->dx;
    }
    if (f->dz < 0.0f) {
        z1 = f->z;
        z0 = f->z + f->dz;
    } else {
        z0 = f->z;
        z1 = f->z + f->dz;
    }
    if (!(p[0] <= x0) && p[0] < x1 && !(p[1] <= z0) && p[1] < z1) {
        if (face == 3) {
            out[0] = f->y;
            out[1] = 3.4e38f;
            out[2] = 1.0f;
            out[3] = 0.0f;
            D_7000319C = 1e-8f;
        } else {
            out[0] = -3.4e38f;
            out[1] = f->y;
            out[2] = 0.0f;
            out[3] = -1.0f;
            D_700031AC = -1e-8f;
        }
        return 1;
    } else {
        return 0;
    }
}
