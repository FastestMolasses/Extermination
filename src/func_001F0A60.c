// NEARMISS func_001F0A60  (vram 0x001F0A60, 0x6AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 17.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Hand-written VU0 macro code in the original (clip judgement, Q-register divides, fixed-point
// conversion, the matrix transforms); the C states the arithmetic in floats with small static
// helpers, so only the call sequence and the packet writes line up.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md. First-level lane 2026-10-01
// (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// The camera-facing pickup glint (port EFFECT_MANAGER.md section 8,
// em_effect_manager_001F0A60; CHAIN_PAGE.md). The original is largely
// hand-written VU0 macro code; this C states the same arithmetic in floats.
//  1. pos (w = 1) through the clip matrix func_001CD370(0); when any of the
//     six clip flags is set (a lane outside +-|w|) nothing is drawn.
//  2. Fog programmer func_0021B9A0(2 / 3, 1.0, 150.0).
//  3. pos through the camera matrix 0x70003AC0: xy /= w, w -= depth, z /=
//     the new w, w = clamp(fog.z + fog.w * w, 0, fog.x) with the fog
//     quadword at D_00275670 + 0xA0; the four lanes go to 12.4 fixed point
//     at 0x70003600.
//  4. Two corner offsets (size, 0) and (0, size) rotated by angle about z
//     (func_001029C0 / func_00102A60 / func_001026A0 in the scratchpad at
//     0x70003400 / 0x70003440); each corner (half size, unscaled camera w)
//     through the screen matrix 0x70003A40 with row 2's xy cleared, xy /= w,
//     to 12.4 fixed point.
//  5. With a non-zero mode the two colours are weighted by the fog value
//     (0x7000360C >> 4, clamped to 0..255): mode 1 scales alpha, modes 2..4
//     scale r, g, b and set 0x7000360C to 0xFF0.
//  6. 14 quadwords on chain D_0028F700 + (chain << 15) + 0x4D3EC0 with id
//     0x70003608 (func_001CB5F0): first quadword cleared, then VIF DIRECT
//     13, a GIF tag for two strips of
//     three Gouraud fogged blended vertices, each strip outer colour at
//     centre + offset (fog word bit 15 set), centre colour at the centre,
//     outer colour at centre - offset; then func_001CB900 appends the blend
//     state and func_0021B9A0(1, 0, 0) resets the fog.
extern float *func_001CD370(int n);
extern void func_0021B9A0(int mode, float a, float b);
extern void func_001029C0(float *m);
extern void func_00102A60(float *dst, float *src, float angle);
extern void func_001026A0(float *dst, float *m, float *v);
extern char *func_001CB5F0(char *chain, int id, int qwc);
extern void func_001CB900(char *chain, int id, int mode);
extern char *D_00275670;
extern char D_0028F700[];

static int ftoi4(float x) {
    return (int)(x * 16.0f);
}

static unsigned int fog_weight(unsigned int c, int mode, int *w360C) {
    int f = *w360C >> 4;
    unsigned int r;
    unsigned int g;
    unsigned int b;

    if (!(f < 0x100)) {
        f = 0xFF;
    }
    if (f < 0) {
        f = 0;
    }
    if (mode == 1) {
        return (c & 0x00FFFFFF) | ((((c >> 24) & 0xFF) * f) >> 8) << 24;
    }
    if (mode == 4 || mode == 3 || mode == 2) {
        r = ((c & 0xFF) * f) >> 8;
        g = (((c >> 8) & 0xFF) * f) >> 8;
        b = (((c >> 16) & 0xFF) * f) >> 8;
        *w360C = 0xFF0;
        return (c & 0xFF000000) | b << 16 | g << 8 | r;
    }
    return c;
}

static void xform(float *out, float *m, float *v) {
    int i;

    for (i = 0; i < 4; i++) {
        out[i] = m[i] * v[0] + m[4 + i] * v[1] + m[8 + i] * v[2] + m[12 + i] * v[3];
    }
}

void func_001F0A60(int chain_no, int mode, float *pos, unsigned int inner, unsigned int outer,
                   float angle, float size, float depth) {
    int *sp = (int *)0x70003600;
    float *corner = (float *)0x70003400;
    float *rot = (float *)0x70003440;
    float *fog;
    float v[4];
    float p[4];
    float s[4];
    float rows[16];
    float keep_w;
    float w;
    int off[2][2];
    char *chain;
    char *q;
    char *vtx;
    int i;
    int k;

    p[0] = pos[0];
    p[1] = pos[1];
    p[2] = pos[2];
    p[3] = 1.0f;
    xform(v, func_001CD370(0), p);
    w = (v[3] < 0.0f) ? -v[3] : v[3];
    for (i = 0; i < 3; i++) {
        if (v[i] > w || v[i] < -w) {
            return;
        }
    }
    func_0021B9A0(2, 1.0f, 150.0f);
    func_0021B9A0(3, 1.0f, 150.0f);
    fog = (float *)(D_00275670 + 0xA0);
    xform(v, (float *)0x70003AC0, p);
    keep_w = v[3];
    v[0] *= 1.0f / v[3];
    v[1] *= 1.0f / v[3];
    v[3] -= depth;
    v[2] *= 1.0f / v[3];
    w = fog[2] + fog[3] * v[3];
    if (w > fog[0]) {
        w = fog[0];
    }
    if (w < 0.0f) {
        w = 0.0f;
    }
    v[3] = w;
    for (i = 0; i < 4; i++) {
        sp[i] = ftoi4(v[i]);
    }
    corner[0] = size;
    corner[1] = 0.0f;
    corner[2] = 0.0f;
    corner[3] = 1.0f;
    corner[4] = 0.0f;
    corner[5] = size;
    corner[6] = 0.0f;
    corner[7] = 1.0f;
    func_001029C0(rot);
    func_00102A60(rot, rot, angle);
    func_001026A0(corner, rot, corner);
    func_001026A0(corner + 4, rot, corner + 4);
    for (k = 0; k < 2; k++) {
        p[0] = 0.5f * corner[k * 4];
        p[1] = 0.5f * corner[k * 4 + 1];
        p[2] = keep_w;
        p[3] = 1.0f;
        for (i = 0; i < 16; i++) {
            rows[i] = ((float *)0x70003A40)[i];
        }
        rows[8] = 0.0f;
        rows[9] = 0.0f;
        xform(s, rows, p);
        off[k][0] = ftoi4(s[0] * (1.0f / s[3]));
        off[k][1] = ftoi4(s[1] * (1.0f / s[3]));
    }
    if (mode != 0) {
        inner = fog_weight(inner, mode, &sp[3]);
        outer = fog_weight(outer, mode, &sp[3]);
    }
    chain = D_0028F700 + (chain_no << 15) + 0x4D3EC0;
    q = func_001CB5F0(chain, sp[2], 0xE);
    ((unsigned long long *)q)[0] = 0;   /* whole first quadword cleared: VIF NOPs */
    ((unsigned long long *)q)[1] = 0;
    *(int *)(q + 0xC) = 0x5000000D;
    *(unsigned long long *)(q + 0x10) = 0x6035400000008002ULL;
    *(unsigned long long *)(q + 0x18) = 0x414141;
    for (k = 0; k < 2; k++) {
        vtx = q + 0x20 + k * 0x60;
        for (i = 0; i < 4; i++) {
            ((int *)vtx)[i] = (outer >> (i * 8)) & 0xFF;
            ((int *)vtx)[8 + i] = (inner >> (i * 8)) & 0xFF;
            ((int *)vtx)[16 + i] = (outer >> (i * 8)) & 0xFF;
        }
        ((int *)vtx)[4] = sp[0] + off[k][0];
        ((int *)vtx)[5] = sp[1] + off[k][1];
        ((int *)vtx)[6] = sp[2];
        ((int *)vtx)[7] = sp[3] | 0x8000;
        ((int *)vtx)[12] = sp[0];
        ((int *)vtx)[13] = sp[1];
        ((int *)vtx)[14] = sp[2];
        ((int *)vtx)[15] = sp[3];
        ((int *)vtx)[20] = sp[0] - off[k][0];
        ((int *)vtx)[21] = sp[1] - off[k][1];
        ((int *)vtx)[22] = sp[2];
        ((int *)vtx)[23] = sp[3];
    }
    func_001CB900(chain, sp[2], mode);
    func_0021B9A0(1, 0.0f, 0.0f);
}
