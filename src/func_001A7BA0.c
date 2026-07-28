// NEARMISS func_001A7BA0  (vram 0x001A7BA0, 0xABC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single s0/s1 web-coloring transposition (eb<->n) that cascades through vb/k/pk (~40 diff lines) — decl-order tuning fixed the entire s2-s7 map and all spill-slot offsets, but the eb/n pair colors reversed under 233 regardless of declaration or statement order (target colors the later-born loop co...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: separating-axis (SAT) collision test between two transformed convex
// polytopes (a/b are object records; fa/fb are face-flag masks). For each active
// element pair (flag-mask gated, material-keyed transform reload via copy_qw4 from
// obj+0x110 material table), transforms up to 4 vertices of each element into the
// scratchpad point blocks 0x70003400/0x70003440 (vertex counts staged through the
// mailbox shorts 0x70003B86/B88, clamped to 4) while accumulating AABBs, does an
// AABB overlap early-out, then builds candidate separating axes (edge-edge cross
// products of the point deltas) into axes[16] and refines a support simplex
// (0x70003440/50/60 + normal 0x70003630) against each candidate axis. Returns 1 on
// the first axis with negative support (separation), else 0.

extern void copy_qw4(void *a0, void *a1);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern void func_00102718(void *a0, void *a1, void *a2);
extern float func_00102738(void *a0, void *a1);
extern void func_00102760(void *a0, void *a1);
extern void func_00102948(void *a0, void *a1);

extern char D_70003400[];
extern char D_70003440[];
extern char D_70003450[];
extern char D_70003460[];
extern char D_70003480[];
extern char D_700034C0[];
extern char D_70003600[];
extern char D_70003610[];
extern char D_70003620[];
extern char D_70003630[];

int func_001A7BA0(char *a, char *b, int fa, int fb)
{
    float axes[16][4];
    char *eb;
    int n;
    int w;
    char *va;
    float *p;
    char *dst;
    char *vb;
    float *pv;
    char *dstB;
    char *pa4;
    int m;
    int w2;
    int k;
    int prod;
    char *pB;
    char *pA;
    int i2;
    int j2;
    float *pk;
    int kk;
    int idx;
    int j;
    int i;
    int na;
    int lastA;
    char *ea;
    int nb;
    int lastB;
    char *ebStart;
    int fa10, fa20, fb10, fb20;
    char *ha, *hb;
    int hit;
    unsigned char matA, matB;
    float v, d;
    float axmax, axmin, aymax, aymin, azmax, azmin;
    float bxmax, bxmin, bymax, bymin, bzmax, bzmin;

    ha = *(char **)(a + 0x58);
    if ((ha == 0) || (hb = *(char **)(b + 0x58), hb == 0)) {
        return 0;
    }

    na = *(int *)ha;
    ea = ha + 4;
    if (*(short *)(ha + 0xA) == -2) {
        unsigned short step = *(unsigned short *)(ea + 4);
        na -= 1;
        ea += step;
    }

    nb = *(int *)hb;
    eb = hb + 4;
    if (*(short *)(hb + 0xA) == -2) {
        eb += *(unsigned short *)(eb + 4);
        nb -= 1;
    }

    lastA = 0x3E7;
    ebStart = eb;
    i = 0;
    if (0 < na) {
        fa10 = fa & 0x10;
        fa20 = fa & 0x20;
        fb10 = fb & 0x10;
        fb20 = fb & 0x20;
        do {
            if (((fa10 != 0) && ((*(unsigned char *)ea & *(unsigned char *)(a + 0x5C)) != 0)) ||
                ((fa20 != 0) && ((*(unsigned char *)(ea + 1) & *(unsigned char *)(a + 0x5D)) != 0))) {
                *(short *)0x70003B86 = *(short *)(ea + 6);
                matA = *(unsigned char *)(ea + 3);
                if (lastA != matA) {
                    lastA = matA;
                    copy_qw4(D_70003480, *(char **)(a + matA * 4 + 0x110) + 0x90);
                }
                va = ea + 8;
                if (*(short *)0x70003B86 >= 5) {
                    *(short *)0x70003B86 = 4;
                }
                w = 4;
                p = (float *)(va + 0x10);
                dst = D_70003400;
                for (n = 0; n < *(short *)0x70003B86; n++) {
                    *(float *)0x70003600 = *p;
                    p += 3;
                    *(float *)0x70003604 = *(float *)(va + (w + 1) * 4);
                    *(float *)0x70003608 = *(float *)(va + (w + 2) * 4);
                    w += 3;
                    *(float *)0x7000360C = 1.0f;
                    func_001026A0(dst, D_70003480, D_70003600);
                    *(int *)(dst + 0xC) = 0;
                    if (n == 0) {
                        axmin = axmax = *(float *)0x70003400;
                        aymin = aymax = *(float *)0x70003404;
                        azmin = azmax = *(float *)0x70003408;
                    } else {
                        v = *(float *)dst;
                        if (!(axmin <= v)) axmin = v;
                        if (axmax < v) axmax = v;
                        v = *(float *)(dst + 4);
                        if (!(aymin <= v)) aymin = v;
                        if (aymax < v) aymax = v;
                        v = *(float *)(dst + 8);
                        if (!(azmin <= v)) azmin = v;
                        if (azmax < v) azmax = v;
                    }
                    dst += 0x10;
                }
                lastB = 0x3E7;
                j = 0;
                if (0 < nb) {
                    do {
                        if (((fb10 != 0) && ((*(unsigned char *)eb & *(unsigned char *)(b + 0x5C)) != 0)) ||
                            ((fb20 != 0) && ((*(unsigned char *)(eb + 1) & *(unsigned char *)(b + 0x5D)) != 0))) {
                            *(short *)0x70003B88 = *(short *)(eb + 6);
                            matB = *(unsigned char *)(eb + 3);
                            if (lastB != matB) {
                                lastB = matB;
                                copy_qw4(D_700034C0, *(char **)(b + matB * 4 + 0x110) + 0x90);
                            }
                            vb = eb + 8;
                            if (*(short *)0x70003B88 >= 5) {
                                *(short *)0x70003B88 = 4;
                            }
                            dstB = D_70003440;
                            w2 = 4;
                            pv = (float *)(vb + 0x10);
                            pa4 = D_70003400;
                            for (m = 0; m < *(short *)0x70003B88; m++) {
                                *(float *)0x70003600 = *pv;
                                pv += 3;
                                *(float *)0x70003604 = *(float *)(vb + (w2 + 1) * 4);
                                *(float *)0x70003608 = *(float *)(vb + (w2 + 2) * 4);
                                w2 += 3;
                                *(float *)0x7000360C = 1.0f;
                                func_001026A0(dstB, D_700034C0, D_70003600);
                                *(int *)(dstB + 0xC) = 0;
                                if (m == 0) {
                                    bxmin = bxmax = *(float *)0x70003440;
                                    bymin = bymax = *(float *)0x70003444;
                                    bzmin = bzmax = *(float *)0x70003448;
                                } else {
                                    v = *(float *)pa4;
                                    if (!(bxmin <= v)) bxmin = *(float *)dstB;
                                    if (bxmax < v) bxmax = *(float *)dstB;
                                    v = *(float *)(pa4 + 4);
                                    if (!(bymin <= v)) bymin = *(float *)(dstB + 4);
                                    if (bymax < v) bymax = *(float *)(dstB + 4);
                                    v = *(float *)(pa4 + 8);
                                    if (!(bzmin <= v)) bzmin = *(float *)(dstB + 8);
                                    if (bzmax < v) bzmax = *(float *)(dstB + 8);
                                }
                                dstB += 0x10;
                                pa4 += 0x10;
                            }
                            prod = *(short *)0x70003B86 * *(short *)0x70003B88;
                            if ((prod >= 4) && (axmin < bxmax) && (aymin < bymax) && (azmin < bzmax) &&
                                !(axmax <= bxmin) && !(aymax <= bymin) && !(azmax <= bzmin)) {
                                pA = D_70003400;
                                k = 0;
                                for (i2 = 0; i2 < *(short *)0x70003B86; i2++) {
                                    pB = D_70003440;
                                    for (j2 = 0; j2 < *(short *)0x70003B88; j2++) {
                                        func_001028D0(D_70003600, pA, pB);
                                        func_00102760(axes[k++], D_70003600);
                                        pB += 0x10;
                                    }
                                    pA += 0x10;
                                }
                                func_00102948(D_70003440, axes[0]);
                                func_00102948(D_70003450, axes[1]);
                                func_00102948(D_70003460, axes[2]);
                                func_001028D0(D_70003610, D_70003450, D_70003440);
                                func_001028D0(D_70003620, D_70003460, D_70003440);
                                func_00102718(D_70003630, D_70003610, D_70003620);
                                func_00102760(D_70003630, D_70003630);
                                d = func_00102738(D_70003440, D_70003630);
                                *(float *)0x70003680 = d;
                                if (d < 0.0f) {
                                    *(float *)0x70003680 = -d;
                                    *(float *)0x70003630 = -*(float *)0x70003630;
                                    *(float *)0x70003634 = -*(float *)0x70003634;
                                    *(float *)0x70003638 = -*(float *)0x70003638;
                                }
                                for (kk = 3, pk = axes[3]; kk < prod; kk++, pk += 4) {
                                    *(float *)0x70003684 = func_00102738(D_70003630, pk);
                                    if (*(float *)0x70003680 < *(float *)0x70003684)
                                        continue;
                                    idx = 0;
                                    func_001028D0(D_70003600, D_70003440, pk);
                                    *(float *)0x70003684 = func_00102738(D_70003600, D_70003600);
                                    func_001028D0(D_70003600, D_70003450, pk);
                                    d = func_00102738(D_70003600, D_70003600);
                                    hit = 1;
                                    *(float *)0x70003684 = d;
                                    if (d <= *(float *)0x70003684) {
                                        hit = 0;
                                    }
                                    if (hit) {
                                        idx = 1;
                                        *(float *)0x70003684 = d;
                                    }
                                    func_001028D0(D_70003600, D_70003460, pk);
                                    d = func_00102738(D_70003600, D_70003600);
                                    hit = 1;
                                    *(float *)0x70003684 = d;
                                    if (d <= *(float *)0x70003684) {
                                        hit = 0;
                                    }
                                    if (hit) {
                                        idx = 2;
                                    }
                                    func_00102948(D_70003440 + idx * 0x10, pk);
                                    func_001028D0(D_70003610, D_70003450, D_70003440);
                                    func_001028D0(D_70003620, D_70003460, D_70003440);
                                    func_00102718(D_70003600, D_70003610, D_70003620);
                                    func_00102760(D_70003630, D_70003600);
                                    d = func_00102738(D_70003630, D_70003600);
                                    *(float *)0x70003684 = d;
                                    if (d < 0.0f) {
                                        float n0 = *(float *)0x70003600;
                                        float n1 = *(float *)0x70003604;
                                        *(float *)0x70003630 = -n0;
                                        *(float *)0x70003634 = -n1;
                                        *(float *)0x70003638 = -*(float *)0x70003608;
                                    } else {
                                        func_00102948(D_70003630, D_70003600);
                                    }
                                    if (func_00102738(D_70003630, D_70003440) < 0.0f) {
                                        return 1;
                                    }
                                }
                            }
                        }
                        j += 1;
                        eb += *(unsigned short *)(eb + 4);
                    } while (j < nb);
                }
            }
            eb = ebStart;
            i += 1;
            ea += *(unsigned short *)(ea + 4);
        } while (i < na);
    }
    return 0;
}
