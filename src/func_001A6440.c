// NEARMISS func_001A6440  (vram 0x001A6440, 0x690 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.58% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Not a genuine idiom-13 wall: body/logic fully recovered (segment-vs-collision-mesh sweep matching sibling func_001A4030's convex n-gon test, verified field-for-field against the .s). Residual is a stack-frame-layout / scalar-spill-order artifact -- target spills 10 scalars (result/hitThisEntity/m...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Segment-vs-world-collision-mesh sweep. Copies the query segment endpoints
// D_70003190[0..2]/[4..6] into scratch A/B, computes dir = B-A, then walks
// the global entity list D_00275B8C/D_00275B94; for each live entity with a
// collision-geometry chain (+0x58), walks that chain's polygon records
// (each: 3 face-mask bytes, material index byte, u16 stride to next record
// at +4, s16 vertex count at +6, then N verts + N edge-normals starting at
// +8). Records that pass the requested-axis face mask and have a
// non-negative vertex count get the convex n-gon segment-intersection test
// (front-facing plane dot when the z-face mask bit is set, hit point, per-
// axis interval reject, "inside" test against each edge normal -- the same
// test as func_001A4030). The record's material transform (copy_qw4 from
// the pointer stored at e+matIdx*4+0x110, +0x90 offset) is cached per
// material index and only reloaded when the index changes. On acceptance:
// stashes the hit point into D_70003190[8..10], optionally clears bit0 of
// the masked y-face byte into the pending-surface global D_700030CC,
// stores the record's first mask byte into D_700030D0, and restages the
// swept segment to [A, hit] so later records in the SAME entity's chain
// (and later entities) test against the shortened segment. After a chain
// finishes, if it produced a hit the entity becomes the new collision
// entity D_700031D4. Final return is always 0 in this build (matches the
// target: the loop never assigns a nonzero return value).
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern float func_00102738(void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00103230(void *dst, void *a, float scale);

extern unsigned char **D_00275B8C;
extern short D_00275B94;
extern float D_70003190[];
extern float D_700030B0[];

int func_001A6440(int arg0) {
    float qa[4];
    float qb[4];
    float dir[4];
    float v0[4];
    float hit[4];
    float hit2[4];
    float sc[4];
    float mat[4];
    int i;
    short n;
    unsigned char **pp;
    unsigned char *e;
    unsigned char *node;
    unsigned char *rec;
    unsigned char *vp;
    unsigned char *np;
    int recCount;
    int matIdx;
    short vcount;
    int mask;
    int hitThisEntity;
    int result;
    int wantX, wantY, wantZ;
    float t, d;
    int j, k;
    int ok;

    *(short *)0x700030CA = 0;
    *(int *)0x700030CC = 0;
    *(int *)0x700030D0 = 0;
    *(float **)0x700031D0 = D_700030B0;

    result = 0;
    for (i = 0; i < 3; i++) {
        qa[i] = D_70003190[i];
        qb[i] = D_70003190[i + 4];
    }
    qa[3] = 0;
    qb[3] = 0;
    func_001028D0(dir, qb, qa);

    pp = D_00275B8C;
    n = 0;
    wantX = arg0 & 0x10;
    wantY = arg0 & 0x20;
    wantZ = arg0 & 0x40;

    for (; n < D_00275B94; n++) {
        e = pp[0];
        matIdx = 0x270F;
        pp += 1;
        hitThisEntity = 0;

        if (*(unsigned char *)e & 1) {
            node = *(unsigned char **)(e + 0x58);
            if (node != 0) {
                rec = node + 4;
                recCount = *(int *)node;
                if (*(short *)(node + 0xA) == -2) {
                    rec += *(unsigned short *)(rec + 4);
                    recCount -= 1;
                }

                for (j = 0; j < recCount; j++) {
                    mask = 0;
                    if (wantX && (*(unsigned char *)(rec + 0) & *(unsigned char *)(e + 0x5C))) {
                        mask = 1;
                    }
                    if (wantY && (*(unsigned char *)(rec + 1) & *(unsigned char *)(e + 0x5D))) {
                        mask |= 2;
                    }
                    if (wantZ && (*(unsigned char *)(rec + 2) & *(unsigned char *)(e + 0x5E))) {
                        mask |= 4;
                    }
                    vcount = *(short *)(rec + 6);
                    if (vcount < 0) {
                        mask = 0;
                    }

                    if (mask != 0) {
                        unsigned char m = *(unsigned char *)(rec + 3);
                        vp = rec + 8;
                        if (matIdx != m) {
                            matIdx = m;
                            copy_qw4(mat, (char *)(*(int *)(e + (m * 4) + 0x110)) + 0x90);
                            vp = rec + 8;
                        }

                        for (i = 0; i < 3; i++) {
                            v0[i] = *(float *)(vp + i * 4);
                        }
                        func_001026A0(v0, mat, v0);

                        hit2[0] = *(float *)(vp + 0x10);
                        hit2[1] = *(float *)(vp + 0x14);
                        hit2[2] = *(float *)(vp + 0x18);
                        hit2[3] = 1.0f;
                        func_001026A0(hit2, mat, hit2);

                        t = func_00102738(v0, hit2);
                        d = func_00102738(dir, v0);

                        if ((mask & 4) && !(d <= -0.00001f)) {
                            rec += *(unsigned short *)(rec + 4);
                            continue;
                        }

                        func_00103230(hit, dir, (t - func_00102738(v0, qa)) / d);
                        func_001028B8(hit, qa, hit);

                        if ((qa[0] <= hit[0] || qb[0] <= hit[0]) &&
                            !(qa[0] < hit[0] && qb[0] < hit[0]) &&
                            (qa[1] <= hit[1] || qb[1] <= hit[1]) &&
                            !(qa[1] < hit[1] && qb[1] < hit[1]) &&
                            (qa[2] <= hit[2] || qb[2] <= hit[2]) &&
                            !(qa[2] < hit[2] && qb[2] < hit[2])) {

                            vp = rec + 8 + 0x10;
                            np = rec + 8 + ((vcount * 2 + vcount + 4) * 4);
                            ok = 1;
                            for (k = 0; k < vcount; k++) {
                                sc[0] = *(float *)(vp + 0);
                                sc[1] = *(float *)(vp + 4);
                                sc[2] = *(float *)(vp + 8);
                                sc[3] = 1.0f;
                                func_001026A0(sc, mat, sc);
                                sc[3] = 0.0f;
                                func_001028D0(hit2, hit, sc);
                                sc[0] = *(float *)(np + 0);
                                sc[1] = *(float *)(np + 4);
                                sc[2] = *(float *)(np + 8);
                                sc[3] = 0.0f;
                                func_001026A0(sc, mat, sc);
                                sc[3] = 0.0f;
                                if (func_00102738(hit2, sc) <= 0.00001f) {
                                    ok = 0;
                                    break;
                                }
                                vp += 0xC;
                                np += 0xC;
                            }

                            if (ok) {
                                hitThisEntity = 1;
                                for (i = 0; i < 3; i++) {
                                    D_70003190[8 + i] = hit[i];
                                }
                                if (mask & 2) {
                                    *(int *)0x700030CC = (int) (*(unsigned char *)(rec + 1) & *(unsigned char *)(e + 0x5D) & 0xFE);
                                }
                                *(int *)0x700030D0 = (int) *(unsigned char *)rec;
                                for (i = 0; i < 3; i++) {
                                    qb[i] = D_70003190[8 + i];
                                }
                                func_001028D0(dir, qb, qa);
                            }
                        }
                    }
                    rec += *(unsigned short *)(rec + 4);
                }

                if (hitThisEntity) {
                    *(unsigned char **)0x700031D4 = e;
                }
            }
        }
    }
    return result;
}
