// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 72.05% NEARMISS).
// Fixes read from the original instructions (the old body was wrong in places):
// - Rewritten in the spelling of the byte-matched twin func_001A6AD0 (same
//   per-polygon loop over one fixed object instead of the object list):
//   do-while walkers for the vector copies, w words cleared/set explicitly.
// - The object is a struct-typed extern D_008102B0 addressed through a pointer,
//   so its fields are read relative to one register.
// - The segment start/end come from D_70003190 (w words cleared); the bounds
//   test ACCEPTS when the hit lies strictly between start and
//   end on ANY axis (six-term OR, as in the twin); the old C inverted it.
// - The edge test breaks on the first edge with dot > 1e-5 and the polygon hits
//   when every edge passed (also when nv is 0); the plane normal (not the plane
//   point) is copied to D_700030B0+0x24, in a loop of its own.
// - No back-face rejection here (the twin has one): t = (d(n,p) - d(n,start)) /
//   d(dir,n) unconditionally.
//
// Segment-vs-collision-mesh query against the single object D_008102B0
// (the twin func_001A6AD0 walks the object list). The query segment lives in the
// scratchpad block D_70003190 (+0x00 start xyz, +0x10 end xyz); flags bits 4-6
// select which of the three per-poly layer bytes are tested against the object
// masks at obj+0x5C/0x5D/0x5E. Returns 0 at once unless the object has collision
// geo (obj+0x58) and a nonzero first byte. Walks the polygon records (u16 size
// link at +4; leading skip record when the s16 at geo+0xA is -2). Per polygon
// that passes the layer masks (s16 vertex count at +6, a negative count skips;
// matrix index byte at +3): lazily fetches the bone matrix (copy_qw4 from
// *(obj+0x110+idx*4)+0x90, cached by index, initial 0x270F), transforms the
// plane normal (pd+0..8, w=0) and plane point (pd+0x10..0x18, w=1) via
// func_001026A0, intersects the segment with the plane (func_00103230 +
// func_001028B8), keeps hits strictly inside the segment bounds on some axis,
// then edge-tests all nv edges (edge normals at pd+(3*nv+4)*4, verts at
// pd+0x10): the hit must be behind every transformed edge normal (dot <= 1e-5).
// On containment: hit point -> D_70003190+0x20, plane normal -> D_700030B0+0x24,
// and the segment end is SHORTENED to the hit point (nearest-hit search
// continues). After each polygon, once anything hit, 0x700031D4 = the object.
// Clears 0x700030CA and points 0x700031D0 at D_700030B0 on entry.
// Returns 1 if anything was hit, 0 otherwise.
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *v, void *m, void *out);
extern float func_00102738(void *a, void *b);
extern void func_001028B8(void *a, void *b, void *out);
extern void func_001028D0(void *a, void *b, void *out);
extern void func_00103230(void *out, void *v, float t);

typedef struct Obj { char b[0x200]; } Obj;
extern Obj D_008102B0;
extern char D_700030B0[];
extern char D_70003190[];

int func_001A7280(int flags) {
    int count;
    int last;
    int hit;
    int result;
    int m10;
    int m20;
    int m40;
    int j;
    float seg0[4];
    float seg1[4];
    float dir[4];
    float pln[4];
    float hitp[4];
    float pv[4];
    float tv[4];
    float mtx[16];
    char *obj;
    char *geo;
    char *p;
    char *pd;
    char *nrm;
    int nv;
    int mask;
    int mtxidx;
    float *pb;
    int e;
    int k;
    float *s;
    float *d0;
    float *d1;
    float d20;
    float d21;

    *(short *)0x700030CA = 0;
    *(char **)0x700031D0 = D_700030B0;

    s = (float *)D_70003190;
    d0 = seg0;
    d1 = seg1;
    result = 0;
    k = 0;
    do {
        k += 1;
        *d0 = s[0];
        d0 += 1;
        *d1 = s[4];
        s += 1;
        d1 += 1;
    } while (k < 3);
    seg0[3] = 0.0f;
    seg1[3] = 0.0f;
    func_001028D0(dir, seg1, seg0);

    last = 0x270F;
    obj = (char *)&D_008102B0;
    hit = 0;
    geo = *(char **)(obj + 0x58);
    if (geo == 0 || *(unsigned char *)obj == 0) {
        return 0;
    }
    count = *(int *)geo;
    p = geo + 4;
    if (*(short *)(geo + 0xA) == -2) {
        p += *(unsigned short *)(p + 4);
        count -= 1;
    }
    j = 0;
    if (0 < count) {
        m10 = flags & 0x10;
        m20 = flags & 0x20;
        m40 = flags & 0x40;
        do {
            mask = 0;
            if (m10 != 0 && (*(unsigned char *)p & *(unsigned char *)(obj + 0x5C))) {
                mask |= 1;
            }
            if (m20 != 0 && (*(unsigned char *)(p + 1) & *(unsigned char *)(obj + 0x5D))) {
                mask |= 2;
            }
            if (m40 != 0 && (*(unsigned char *)(p + 2) & *(unsigned char *)(obj + 0x5E))) {
                mask |= 4;
            }
            nv = *(short *)(p + 6);
            if (nv < 0) {
                mask = 0;
            }
            if (mask != 0) {
                mtxidx = *(unsigned char *)(p + 3);
                if (last != mtxidx) {
                    last = mtxidx;
                    copy_qw4(mtx, (void *)(*(int *)(obj + mtxidx * 4 + 0x110) + 0x90));
                }
                pd = p + 8;
                pb = pln;
                e = 0;
                s = (float *)pd;
                d1 = pb;
                do {
                    e += 1;
                    *d1 = *s;
                    s += 1;
                    d1 += 1;
                } while (e < 3);
                pln[3] = 0.0f;
                func_001026A0(pln, mtx, pln);
                pv[0] = *(float *)(pd + 0x10);
                pv[1] = *(float *)(pd + 0x14);
                pv[2] = *(float *)(pd + 0x18);
                pv[3] = 1.0f;
                func_001026A0(pv, mtx, pv);
                d20 = func_00102738(pln, pv);
                d21 = func_00102738(dir, pln);
                func_00103230(hitp, dir, (d20 - func_00102738(pln, seg0)) / d21);
                func_001028B8(hitp, seg0, hitp);
                if ((seg0[0] > hitp[0] && seg1[0] < hitp[0])
                    || (seg0[0] < hitp[0] && seg1[0] > hitp[0])
                    || (seg0[1] > hitp[1] && seg1[1] < hitp[1])
                    || (seg0[1] < hitp[1] && seg1[1] > hitp[1])
                    || (seg0[2] > hitp[2] && seg1[2] < hitp[2])
                    || (seg0[2] < hitp[2] && seg1[2] > hitp[2])) {
                    nrm = pd + (nv * 3 + 4) * 4;
                    pd += 0x10;
                    e = 0;
                    if (0 < nv) {
                        do {
                            tv[0] = *(float *)pd;
                            tv[1] = *(float *)(pd + 4);
                            tv[2] = *(float *)(pd + 8);
                            tv[3] = 1.0f;
                            func_001026A0(tv, mtx, tv);
                            tv[3] = 0.0f;
                            func_001028D0(pv, hitp, tv);
                            tv[0] = *(float *)nrm;
                            tv[1] = *(float *)(nrm + 4);
                            tv[2] = *(float *)(nrm + 8);
                            tv[3] = 0.0f;
                            func_001026A0(tv, mtx, tv);
                            tv[3] = 0.0f;
                            if (func_00102738(pv, tv) > 0.00001f) {
                                break;
                            }
                            e += 1;
                            pd += 0xC;
                            nrm += 0xC;
                        } while (e < nv);
                    }
                    if (e >= nv) {
                        hit = 1;
                        result = hit;
                        k = 0;
                        s = hitp;
                        d0 = (float *)D_70003190;
                        do {
                            k += 1;
                            d0[8] = *s;
                            s += 1;
                            d0 += 1;
                        } while (k < 3);
                        k = 0;
                        s = pb;
                        d1 = (float *)D_700030B0;
                        do {
                            k += 1;
                            d1[9] = *s;
                            s += 1;
                            d1 += 1;
                        } while (k < 3);
                        k = 0;
                        d0 = (float *)D_70003190;
                        d1 = seg1;
                        do {
                            k += 1;
                            *d1 = d0[8];
                            d0 += 1;
                            d1 += 1;
                        } while (k < 3);
                        func_001028D0(dir, seg1, seg0);
                    }
                }
            }
            p += *(unsigned short *)(p + 4);
            if (hit != 0) {
                *(char **)0x700031D4 = obj;
            }
            j += 1;
        } while (j < count);
    }
    return result;
}
