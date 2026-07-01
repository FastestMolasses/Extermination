// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Segment-vs-collision-mesh query over the global object table (D_00275B8C
// list / D_00275B94 count). The query segment lives in the scratchpad block
// D_70003190 (+0x00 start xyz, +0x10 end xyz); flags bits 4-6 select which of
// the three per-poly layer bytes are tested against the object masks at
// obj+0x5C/0x5D/0x5E. For each active object (flag bit0) with collision geo
// (obj+0x58, type nibble obj+2 & 0x1F not 0/2): walks the polygon records
// (u16 size link at +4; leading skip record when the s16 at geo+0xA is -2).
// Per polygon that passes the layer masks (s16 vertex count at +6, matrix
// index byte at +3): lazily fetches the bone matrix (copy_qw4 from
// *(obj+0x110+idx*4)+0x90, cached by index), transforms the plane normal
// (pd+0..8, w=0) and plane point (pd+0x10..0x18, w=1) via func_001026A0,
// intersects the segment with the plane (t = (dot(n,pt) - dot(n,start)) /
// dot(dir,n), only front-facing hits dot(dir,n) <= -1e-5; point via
// func_00103230 + func_001028B8), rejects hits outside the segment bounds on
// every axis, then edge-tests all nv edges (edge normals block at
// pd+(3*nv+4)*4, verts at pd+0x10): the hit must be behind every transformed
// edge normal (dot <= 1e-5). On containment: records the hit -- hit point ->
// D_70003190+0x20, plane normal -> D_700030B0+0x24, masked contact layer bits
// -> 0x700030CC, poly header word -> 0x700030D0, prim/object ptrs ->
// 0x700031D0/0x700031D4 -- and SHORTENS the segment to the hit point
// (nearest-hit search continues). After any object that hit, classifies the
// contact slope from the mailbox delta at 0x700030D4/D8/DC: q = dy*dy /
// (dx*dx + dz*dz) (FPU mula.s/madd.s pair) -> code at 0x700030CA: 0x2000
// near-flat (q < 0.49029058 = tan^2 35deg), rising dy>=0: 0x1000 (q <= 3 =
// tan^2 60deg) / 0x4000 steeper, falling dy<0: 0x800 / -0x8000 (drop).
// Returns 1 if anything was hit.
//
// MATCHING-CRITICAL: the volatile reads of the 0x700030Dx mailbox pin mwcc's
// load order; the private "p += size; goto next_poly" advance reproduces the
// duplicated back-face skip path; `e` (not k) must be the counter of the
// plane-copy loop. Byte-verified 100.0 objdiff.

extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *v, void *m, void *out);
extern float func_00102738(void *a, void *b);
extern void func_001028B8(void *a, void *b, void *out);
extern void func_001028D0(void *a, void *b, void *out);
extern void func_00103230(void *out, void *v, float t);

extern char *D_00275B8C;
extern short D_00275B94;
extern char D_700030B0[];
extern char D_70003190[];

int func_001A6AD0(int flags) {
    int j;
    int count;
    int last;
    int hit;
    int result;
    char **list;
    int m10;
    int m20;
    int m40;
    int i;
    float seg0[4];
    float seg1[4];
    float dir[4];
    float pln[4];
    float hitp[4];
    float pv[4];
    float tv[4];
    float mtx[16];
    char *obj;
    int type;
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
    float dz;
    float dx;
    float dy;
    float den;
    float q;

    *(short *)0x700030CA = 0;
    *(int *)0x700030CC = 0;
    *(int *)0x700030D0 = 0;
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

    i = 0;
    list = (char **)D_00275B8C;
    m10 = flags & 0x10;
    m20 = flags & 0x20;
    m40 = flags & 0x40;
    while (i < D_00275B94) {
        obj = *list;
        last = 0x270F;
        list += 1;
        hit = 0;
        if (*(unsigned char *)obj & 1) {
            geo = *(char **)(obj + 0x58);
            if (geo != 0) {
                type = *(unsigned char *)(obj + 2) & 0x1F;
                if (type != 0 && type != 2) {
                    count = *(int *)geo;
                    p = geo + 4;
                    if (*(short *)(geo + 0xA) == -2) {
                        p += *(unsigned short *)(p + 4);
                        count -= 1;
                    }
                    j = 0;
                    if (0 < count) {
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
                                if (!(d21 <= -0.00001f)) {
                                    p += *(unsigned short *)(p + 4);
                                    goto next_poly;
                                }
                                {
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
                                            d1 = (float *)D_700030B0;
                                            do {
                                                k += 1;
                                                d0[8] = *s;
                                                s += 1;
                                                d0 += 1;
                                                d1[9] = *pb;
                                                pb += 1;
                                                d1 += 1;
                                            } while (k < 3);
                                            if (mask & 2) {
                                                *(int *)0x700030CC =
                                                    *(unsigned char *)(p + 1) & *(unsigned char *)(obj + 0x5D) & 0xFE;
                                            }
                                            *(int *)0x700030D0 = *(int *)p;
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
                            }
                            p += *(unsigned short *)(p + 4);
                        next_poly:
                            j += 1;
                        } while (j < count);
                    }
                    if (hit != 0) {
                        *(char **)0x700031D4 = obj;
                        dx = *(volatile float *)0x700030D4;
                        dz = *(volatile float *)0x700030DC;
                        den = dx * dx + dz * dz;
                        dy = *(volatile float *)0x700030D8;
                        q = (dy * dy) / den;
                        if (!(dy < 0.0f)) {
                            if (q < 0.49029058f) {
                                *(short *)0x700030CA = 0x2000;
                            } else if (q > 3.0f) {
                                *(short *)0x700030CA = 0x4000;
                            } else {
                                *(short *)0x700030CA = 0x1000;
                            }
                        } else {
                            if (q < 0.49029058f) {
                                *(short *)0x700030CA = 0x2000;
                            } else if (q > 3.0f) {
                                *(short *)0x700030CA = -0x8000;
                            } else {
                                *(short *)0x700030CA = 0x800;
                            }
                        }
                    }
                }
            }
        }
        i += 1;
    }
    return result;
}
