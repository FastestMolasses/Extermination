// func_001A4030 -- byte-matched from C (objdiff 100%, mwcc 2.3.3). Promoted from
// NEARMISS (81.64%) in the decomp-readable-fixes lane (2026-09-23) after the
// readable C was corrected against the .s: the edge-normal pool is at
// (arg0+4) + (3n+4)*4 (0x001A4248..0x001A4264), not 16 bytes further; the plane
// normal is copied from the prim into the hit record at D_700030B0+0x24
// (0x001A4460 loop), not to 0x700030B0; the ny^2/(nx^2+nz^2) ratio is stored to
// scratchpad D_70003680 (0x001A4358) and re-read by each test; the locals are
// vec4 with w zeroed; the six interval rejects are one && condition; the
// classification stores 0x700030CA per branch with the non-negative ny case
// first. Scratchpad extern D_70003680 per idiom-32 (// SPAD below).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003680

//
// Collision: convex planar n-gon segment-intersection test (world-cell prim
// type 0x1000). arg0 -> prim header: +0x2 vert count byte n; +0x4 plane
// normal+d (vec3f + f32); +0x14 n verts (vec3f); then the edge-normal pool
// (vec3f * n) at +0x14 + 12n (0x001A4248..0x001A4264: s1 = (arg0+4) +
// (3n+4)*4). Query segment endpoints live in scratchpad D_70003190
// (+0x00 = A, +0x10 = B). Front-facing test dot(dir,n) <= -1e-5, plane t,
// per-axis hit-in-interval reject, convex "inside" test against each edge
// normal, then on accept stores the hit at D_70003190+0x20 (0x700031B0,
// 0x001A4318 loop), stores ny^2/(nx^2+nz^2) to SPR 0x70003680 (0x001A4358)
// and classifies it vs 0.49029058f / 3.0f into SPR 0x700030CA, and copies
// the plane normal from the prim into the hit record D_700030B0 at +0x24
// (0x700030D4.., 0x001A4460 loop: swc1 0x24($v1), source lwc1 0($s2)).
// The locals are vec4: the .s zeroes w of A, B, n (0x001A40C4..0x001A40D0)
// and of the edge scratch (0x001A426C).
extern float D_70003190[];
extern float D_700030B0[];
extern float D_70003680;
extern float func_00102738(void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00103230(void *dst, void *a, float scale);

int func_001A4030(char *arg0) {
    float qa[4];
    float qb[4];
    float dir[4];
    float n[4];
    float hit[4];
    float e0[4];
    float v[4];
    float d;
    float t;
    int count;
    int i;
    char *verts;
    char *edgeNormals;
    char *plane;

    plane = arg0 + 4;

    for (i = 0; i < 3; i++) {
        qa[i] = D_70003190[i];
        qb[i] = D_70003190[i + 4];
        n[i] = *(float *)(plane + i * 4);
    }
    n[3] = 0.0f;
    qb[3] = 0.0f;
    qa[3] = 0.0f;

    func_001028D0(dir, qb, qa);
    d = *(float *)(plane + 0xC);
    t = func_00102738(dir, n);
    if (!(t <= -0.00001f)) {
        return 0;
    }

    func_00103230(hit, dir, (d - func_00102738(n, qa)) / t);
    func_001028B8(hit, qa, hit);

    if (!((qa[0] <= hit[0] || qb[0] <= hit[0]) && !(qa[0] < hit[0] && qb[0] < hit[0]) &&
          (qa[1] <= hit[1] || qb[1] <= hit[1]) && !(qa[1] < hit[1] && qb[1] < hit[1]) &&
          (qa[2] <= hit[2] || qb[2] <= hit[2]) && !(qa[2] < hit[2] && qb[2] < hit[2]))) {
        return 0;
    }

    count = *(unsigned char *)(arg0 + 2);
    verts = plane + 0x10;
    edgeNormals = plane + (count * 3 + 4) * 4;
    v[3] = 0.0f;

    for (i = 0; i < *(unsigned char *)(arg0 + 2); i++) {
        v[0] = *(float *)(verts + 0);
        v[1] = *(float *)(verts + 4);
        v[2] = *(float *)(verts + 8);
        func_001028D0(e0, hit, v);

        v[0] = *(float *)(edgeNormals + 0);
        v[1] = *(float *)(edgeNormals + 4);
        v[2] = *(float *)(edgeNormals + 8);
        if (!(func_00102738(e0, v) <= 0.00001f)) {
            return 0;
        }
        verts += 0xC;
        edgeNormals += 0xC;
    }

    for (i = 0; i < 3; i++) {
        D_70003190[i + 8] = hit[i];
    }

    D_70003680 =
        ((*(float *)(plane + 4)) * (*(float *)(plane + 4))) /
        ((*(float *)(plane + 0)) * (*(float *)(plane + 0)) + (*(float *)(plane + 8)) * (*(float *)(plane + 8)));
    if (!(*(float *)(plane + 4) < 0.0f)) {
        if (D_70003680 < 0.49029058f) {
            *(short *)0x700030CA = 0x2000;
        } else if (!(D_70003680 <= 3.0f)) {
            *(short *)0x700030CA = 0x4000;
        } else {
            *(short *)0x700030CA = 0x1000;
        }
    } else {
        if (D_70003680 < 0.49029058f) {
            *(short *)0x700030CA = 0x2000;
        } else if (!(D_70003680 <= 3.0f)) {
            *(short *)0x700030CA = -0x8000;
        } else {
            *(short *)0x700030CA = 0x800;
        }
    }

    for (i = 0; i < 3; i++) {
        D_700030B0[i + 9] = *(float *)plane;
        plane += 4;
    }

    return 1;
}
