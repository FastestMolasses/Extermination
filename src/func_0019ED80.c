// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031FC 0x70003200 0x70003204
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a readable companion of an
// asm body, 99.92%). Fixes read from the original instructions:
//  - the pool bases are added as unsigned addresses with the pool first
//    ((unsigned int)pool + node offset) for both the edge-normal and the vertex-index
//    pools; with the node offset first (or an int cast) the two loads swapped registers.
extern float *D_700031FC;
extern char *D_70003200;
extern char *D_70003204;

// Grid collision node test (port COLL_GRID_HULL.md / em_coll_probe_original:
// em_coll_probe_0019ED80). s is the probe state (start +0x00, end +0x10, hit
// point +0x20, hit node +0x40; the scratchpad block at 0x70003190). The
// segment must approach the node's plane (normal +0x24, distance +0x30) from
// the front: dir.n <= -1e-5. The crossing hit = start + dir * t,
// t = (d - n.start) / (dir.n), must lie within the segment's span on every
// axis and inside the node's polygon (for each of its +0x18 vertices, the
// hit relative to the vertex dotted with the edge normal <= 1e-5; pools as in
// func_0019F330). A hit stores the point and the node and returns 1;
// otherwise 0. The vector helpers are the SDK VU0 routines (func_001028D0
// subtract, func_001028B8 add, func_00102738 dot, func_00103230 scale).
typedef struct V4 {
    float v[3];
    int w;
} V4;

extern void func_001028D0(V4 *dst, V4 *a, V4 *b);
extern void func_001028B8(V4 *dst, V4 *a, V4 *b);
extern float func_00102738(V4 *a, V4 *b);
extern void func_00103230(V4 *dst, V4 *v, float t);

int func_0019ED80(float *s, char *node) {
    V4 qa;
    V4 qb;
    V4 hit;
    V4 rel;
    V4 v;
    V4 dir;
    V4 n;
    float d;
    float along;
    short *idx;
    float *e;
    int i;

    for (i = 0; i < 3; i++) {
        qa.v[i] = s[i];
        qb.v[i] = s[i + 4];
    }
    qa.w = 0;
    qb.w = 0;
    func_001028D0(&dir, &qb, &qa);
    for (i = 0; i < 3; i++) {
        n.v[i] = *(float *)(node + 0x24 + i * 4);
    }
    n.w = 0;
    d = *(float *)(node + 0x30);
    along = func_00102738(&dir, &n);
    if (!(along <= -1e-5f)) {
        return 0;
    }
    func_00103230(&hit, &dir, (d - func_00102738(&n, &qa)) / along);
    func_001028B8(&hit, &qa, &hit);
    if (!(qa.v[0] <= qb.v[0])) {
        if (qa.v[0] < hit.v[0]) {
            return 0;
        }
        if (!(qb.v[0] <= hit.v[0])) {
            return 0;
        }
    } else {
        if (!(qa.v[0] <= hit.v[0])) {
            return 0;
        }
        if (qb.v[0] < hit.v[0]) {
            return 0;
        }
    }
    if (!(qa.v[1] <= qb.v[1])) {
        if (qa.v[1] < hit.v[1]) {
            return 0;
        }
        if (!(qb.v[1] <= hit.v[1])) {
            return 0;
        }
    } else {
        if (!(qa.v[1] <= hit.v[1])) {
            return 0;
        }
        if (qb.v[1] < hit.v[1]) {
            return 0;
        }
    }
    if (!(qa.v[2] <= qb.v[2])) {
        if (qa.v[2] < hit.v[2]) {
            return 0;
        }
        if (!(qb.v[2] <= hit.v[2])) {
            return 0;
        }
    } else {
        if (!(qa.v[2] <= hit.v[2])) {
            return 0;
        }
        if (qb.v[2] < hit.v[2]) {
            return 0;
        }
    }
    e = (float *)((unsigned int)D_70003200 + *(int *)(node + 0x20));
    idx = (short *)((unsigned int)D_70003204 + *(int *)(node + 0x1C));
    v.w = 0;
    rel.w = 0;
    hit.w = 0;
    for (i = 0; i < *(unsigned char *)(node + 0x18); i++) {
        v.v[0] = D_700031FC[*idx * 3 + 0];
        v.v[1] = D_700031FC[*idx * 3 + 1];
        v.v[2] = D_700031FC[*idx * 3 + 2];
        func_001028D0(&rel, &hit, &v);
        v.v[0] = e[0];
        v.v[1] = e[1];
        v.v[2] = e[2];
        if (!(func_00102738(&rel, &v) <= 1e-5f)) {
            return 0;
        }
        idx++;
        e += 3;
    }
    for (i = 0; i < 3; i++) {
        s[i + 8] = hit.v[i];
    }
    *(char **)(s + 0x10) = node;
    return 1;
}
