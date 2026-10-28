// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031FC 0x70003200 0x70003204 0x70003684
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a readable companion of an
// asm body at 99.81%). Fix read from the original instructions:
// - the node's index-list and edge-list offsets (+0x1C, +0x20) are read through volatile
//   ints, the spelling of the byte-matched sibling func_0019F680; each pool base is then
//   loaded ahead of its offset, as in the original.
extern float D_70003684;
extern float *D_700031FC;
extern char *D_70003200;
extern char *D_70003204;

// Collision column pass 2's plane crossing (port COLL_LIST_PASSES.md:
// em_coll_list_passes_0019F330). The line a -> b is intersected with the
// node's plane (normal +0x24, distance +0x30): hit = a + (b - a) * t with
// t = (d - n.a) / ((b - a).n). The hit must lie inside the node's polygon:
// for each of its +0x18 vertices (index pool 0x70003204 + node+0x1C, vertex
// pool 0x700031FC, 12-byte vertices) the hit relative to the vertex, dotted
// with the edge normal (pool 0x70003200 + node+0x20), must be <= 1e-5;
// otherwise 0. On a hit q[0..2] = hit, the scratchpad gets the plane's
// horizontal length sqrtf(nx*nx + nz*nz) (0x70003680) and |ny| / that
// (0x70003684, 3.4e38 when below 1e-4), and q[3] = pi/2 - atanf(ratio),
// negated when ny < 0. Returns 1. The vector helpers are the SDK VU0
// routines: func_001028D0 subtract, func_001028B8 add, func_00102738 dot,
// func_00103230 scale.
typedef struct V4 {
    float v[3];
    int w;
} V4;

extern void func_001028D0(V4 *dst, V4 *a, V4 *b);
extern void func_001028B8(V4 *dst, V4 *a, V4 *b);
extern float func_00102738(V4 *a, V4 *b);
extern void func_00103230(V4 *dst, V4 *v, float t);
extern float func_0011E748(float x);
extern float func_0011DF78(float x);
extern float func_0011DBB8(float x);

int func_0019F330(float *a, float *b, float *q, char *node) {
    V4 va;
    V4 vb;
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
        va.v[i] = a[i];
        vb.v[i] = b[i];
    }
    va.w = 0;
    vb.w = 0;
    func_001028D0(&dir, &vb, &va);
    for (i = 0; i < 3; i++) {
        n.v[i] = *(float *)(node + 0x24 + i * 4);
    }
    n.w = 0;
    d = *(float *)(node + 0x30);
    along = func_00102738(&dir, &n);
    func_00103230(&hit, &dir, (d - func_00102738(&n, &va)) / along);
    func_001028B8(&hit, &va, &hit);
    idx = (short *)(D_70003204 + *(volatile int *)(node + 0x1C));
    e = (float *)(D_70003200 + *(volatile int *)(node + 0x20));
    for (i = 0; i < *(unsigned char *)(node + 0x18); i++) {
        v.v[0] = D_700031FC[*idx * 3 + 0];
        v.v[1] = D_700031FC[*idx * 3 + 1];
        v.v[2] = D_700031FC[*idx * 3 + 2];
        v.w = 0;
        func_001028D0(&rel, &hit, &v);
        v.v[0] = e[0];
        v.v[1] = e[1];
        v.v[2] = e[2];
        v.w = 0;
        if (!(func_00102738(&rel, &v) <= 1e-5f)) {
            return 0;
        }
        idx++;
        e += 3;
    }
    for (i = 0; i < 3; i++) {
        q[i] = hit.v[i];
    }
    *(volatile float *)0x70003680 = func_0011E748(n.v[0] * n.v[0] + n.v[2] * n.v[2]);
    if (*(volatile float *)0x70003680 < 0.0001f) {
        D_70003684 = 3.4e38f;
    } else {
        D_70003684 = func_0011DF78(n.v[1]) / *(volatile float *)0x70003680;
    }
    if (n.v[1] < 0.0f) {
        q[3] = -(1.5707964f - func_0011DBB8(D_70003684));
    } else {
        q[3] = 1.5707964f - func_0011DBB8(D_70003684);
    }
    return 1;
}
