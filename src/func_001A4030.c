// NEARMISS func_001A4030  (vram 0x001A4030, 0x480 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.64% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Fully recovered logic/structure (convex n-gon segment test, per-axis interval reject, edge-normal inside test, ny^2/(nx^2+nz^2) surface classification) with idiom-10 reloads applied for the count byte and classification normal. Residual is a pure register-allocation-ORDER wall: target keeps 7 sav...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Collision: convex planar n-gon segment-intersection test (world-cell prim
// type 0x1000). arg0 -> prim header: +0x2 vert count byte; +0x4 plane
// normal+d (vec3f + f32); +0x14 N verts (vec3f); then narrow edge-normal
// pool (vec3f * N). Query segment endpoints live in scratchpad D_70003190
// (+0x00 = A, +0x10 = B). Front-facing test dot(dir,n) <= -1e-5, plane t,
// per-axis hit-in-interval reject, convex "inside" test against each edge
// normal, then on accept classifies the surface via ny^2/(nx^2+nz^2) vs
// 0.49029058f / 3.0f into SPR 0x700030CA and stages the hit normal at
// D_700030B0 (0x700030B0..).
extern float D_70003190[];
extern float func_00102738(void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00103230(void *dst, void *a, float scale);

int func_001A4030(char *arg0) {
    float qa[3];
    float qb[3];
    float n[3];
    float dir[3];
    float hit[3];
    float v0[3];
    float v1[3];
    float e0[3];
    float d;
    float t;
    float ny2, nsum;
    short surf;
    char *plane;
    char *verts;
    char *edgeNormals;
    int count;
    int i;

    plane = arg0 + 4;

    for (i = 0; i < 3; i++) {
        qa[i] = D_70003190[i];
        qb[i] = D_70003190[i + 4];
        n[i] = *(float *)(plane + i * 4);
    }

    func_001028D0(dir, qb, qa);
    d = *(float *)(plane + 0xC);
    t = func_00102738(dir, n);
    if (!(t <= -0.00001f)) {
        return 0;
    }

    func_00103230(hit, dir, (d - func_00102738(n, qa)) / t);
    func_001028B8(hit, qa, hit);

    if (!(qa[0] <= hit[0] || qb[0] <= hit[0])) {
        return 0;
    }
    if (qa[0] < hit[0] && qb[0] < hit[0]) {
        return 0;
    }
    if (!(qa[1] <= hit[1] || qb[1] <= hit[1])) {
        return 0;
    }
    if (qa[1] < hit[1] && qb[1] < hit[1]) {
        return 0;
    }
    if (!(qa[2] <= hit[2] || qb[2] <= hit[2])) {
        return 0;
    }
    if (qa[2] < hit[2] && qb[2] < hit[2]) {
        return 0;
    }

    count = *(unsigned char *)(arg0 + 2);
    verts = plane + 0x10;
    edgeNormals = plane + 0x10 + (count * 3 + 4) * 4;

    for (i = 0; i < *(unsigned char *)(arg0 + 2); i++) {
        v0[0] = *(float *)(verts + 0);
        v0[1] = *(float *)(verts + 4);
        v0[2] = *(float *)(verts + 8);
        func_001028D0(e0, hit, v0);

        v1[0] = *(float *)(edgeNormals + 0);
        v1[1] = *(float *)(edgeNormals + 4);
        v1[2] = *(float *)(edgeNormals + 8);
        if (!(func_00102738(e0, v1) <= 0.00001f)) {
            return 0;
        }
        verts += 0xC;
        edgeNormals += 0xC;
    }

    for (i = 0; i < 3; i++) {
        D_70003190[i + 8] = hit[i];
    }

    nsum = (*(float *)(plane + 0)) * (*(float *)(plane + 0)) + (*(float *)(plane + 8)) * (*(float *)(plane + 8));
    ny2 = ((*(float *)(plane + 4)) * (*(float *)(plane + 4))) / nsum;
    if (*(float *)(plane + 4) < 0.0f) {
        if (ny2 < 0.49029058f) {
            surf = 0x2000;
        } else if (ny2 <= 3.0f) {
            surf = 0x800;
        } else {
            surf = -0x8000;
        }
    } else {
        if (ny2 < 0.49029058f) {
            surf = 0x2000;
        } else if (ny2 <= 3.0f) {
            surf = 0x1000;
        } else {
            surf = 0x4000;
        }
    }
    *(short *)0x700030CA = surf;

    for (i = 0; i < 3; i++) {
        *(float *)(0x700030B0 + i * 4) = n[i];
        plane += 4;
    }

    return 1;
}
