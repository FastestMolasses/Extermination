// NEARMISS func_001A7BA0  (vram 0x001A7BA0, 0xABC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was 94.89%; matching wave 2026-10-27).
// Fixes read from the original: the vertex loops index va[w], va[w+1], va[w+2] and the point blocks by
// the loop counter (the pointer walks are mwcc's strength reduction); both vertex loops reuse n and w;
// vb declared first; the scratchpad counts 0x70003B86/88 are relocated externs (SPAD) and the clamps
// and the pair-count gate are written as > comparisons; the w = 1.0 store goes through D_70003600; the
// A-header step adds to ea before the count decrement; the outer tail advances ea before i.
// Remaining differences: the B vertex loop gives n and w each other's saved register, the two
// best-axis 'hit' tests get two label-alignment nops each that the original lacks, and the
// final normal negation loads its three words in a different order.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B86 0x70003B88

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

extern short D_70003B86;
extern short D_70003B88;
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
    char *vb;
    char *eb;
    int n;
    int w;
    char *va;
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
        ea += *(unsigned short *)(ea + 4);
        na -= 1;
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
                D_70003B86 = *(short *)(ea + 6);
                matA = *(unsigned char *)(ea + 3);
                if (lastA != matA) {
                    lastA = matA;
                    copy_qw4(D_70003480, *(char **)(a + matA * 4 + 0x110) + 0x90);
                }
                va = ea + 8;
                if (D_70003B86 > 4) {
                    D_70003B86 = 4;
                }
                w = 4;
                for (n = 0; n < D_70003B86; n++) {
                    *(float *)0x70003600 = *(float *)(va + w * 4);
                    *(float *)0x70003604 = *(float *)(va + (w + 1) * 4);
                    *(float *)0x70003608 = *(float *)(va + (w + 2) * 4);
                    w += 3;
                    *(float *)0x7000360C = 1.0f;
                    func_001026A0((D_70003400 + n * 0x10), D_70003480, D_70003600);
                    *(int *)((D_70003400 + n * 0x10) + 0xC) = 0;
                    if (n == 0) {
                        axmin = axmax = *(float *)0x70003400;
                        aymin = aymax = *(float *)0x70003404;
                        azmin = azmax = *(float *)0x70003408;
                    } else {
                        v = *(float *)(D_70003400 + n * 0x10);
                        if (!(axmin <= v)) axmin = v;
                        if (axmax < v) axmax = v;
                        v = *(float *)((D_70003400 + n * 0x10) + 4);
                        if (!(aymin <= v)) aymin = v;
                        if (aymax < v) aymax = v;
                        v = *(float *)((D_70003400 + n * 0x10) + 8);
                        if (!(azmin <= v)) azmin = v;
                        if (azmax < v) azmax = v;
                    }
                }
                lastB = 0x3E7;
                j = 0;
                if (0 < nb) {
                    do {
                        if (((fb10 != 0) && ((*(unsigned char *)eb & *(unsigned char *)(b + 0x5C)) != 0)) ||
                            ((fb20 != 0) && ((*(unsigned char *)(eb + 1) & *(unsigned char *)(b + 0x5D)) != 0))) {
                            D_70003B88 = *(short *)(eb + 6);
                            matB = *(unsigned char *)(eb + 3);
                            if (lastB != matB) {
                                lastB = matB;
                                copy_qw4(D_700034C0, *(char **)(b + matB * 4 + 0x110) + 0x90);
                            }
                            vb = eb + 8;
                            if (D_70003B88 > 4) {
                                D_70003B88 = 4;
                            }
                            w = 4;
                            for (n = 0; n < D_70003B88; n++) {
                                *(float *)0x70003600 = *(float *)(vb + w * 4);
                                *(float *)0x70003604 = *(float *)(vb + (w + 1) * 4);
                                *(float *)0x70003608 = *(float *)(vb + (w + 2) * 4);
                                w += 3;
                                *(float *)0x7000360C = 1.0f;
                                func_001026A0((D_70003440 + n * 0x10), D_700034C0, D_70003600);
                                *(int *)((D_70003440 + n * 0x10) + 0xC) = 0;
                                if (n == 0) {
                                    bxmin = bxmax = *(float *)0x70003440;
                                    bymin = bymax = *(float *)0x70003444;
                                    bzmin = bzmax = *(float *)0x70003448;
                                } else {
                                    v = *(float *)(D_70003400 + n * 0x10);
                                    if (!(bxmin <= v)) bxmin = *(float *)(D_70003440 + n * 0x10);
                                    if (bxmax < v) bxmax = *(float *)(D_70003440 + n * 0x10);
                                    v = *(float *)((D_70003400 + n * 0x10) + 4);
                                    if (!(bymin <= v)) bymin = *(float *)((D_70003440 + n * 0x10) + 4);
                                    if (bymax < v) bymax = *(float *)((D_70003440 + n * 0x10) + 4);
                                    v = *(float *)((D_70003400 + n * 0x10) + 8);
                                    if (!(bzmin <= v)) bzmin = *(float *)((D_70003440 + n * 0x10) + 8);
                                    if (bzmax < v) bzmax = *(float *)((D_70003440 + n * 0x10) + 8);
                                }
                            }
                            prod = D_70003B86 * D_70003B88;
                            if ((prod > 3) && (axmin < bxmax) && (aymin < bymax) && (azmin < bzmax) &&
                                !(axmax <= bxmin) && !(aymax <= bymin) && !(azmax <= bzmin)) {
                                pA = D_70003400;
                                for (i2 = 0, k = 0; i2 < D_70003B86; i2++) {
                                    pB = D_70003440;
                                    for (j2 = 0; j2 < D_70003B88; j2++) {
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
                                        float n0 = -*(float *)0x70003600;
                                        float n1 = -*(float *)0x70003604;
                                        float n2 = -*(float *)0x70003608;
                                        *(float *)0x70003630 = n0;
                                        *(float *)0x70003634 = n1;
                                        *(float *)0x70003638 = n2;
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
            ea += *(unsigned short *)(ea + 4);
            i += 1;
        } while (i < na);
    }
    return 0;
}
