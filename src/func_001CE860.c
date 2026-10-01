// NEARMISS func_001CE860  (vram 0x001CE860, 0x678 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 27.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// C with hand-written VU0 macro sections in the original (clip judgement, the camera transform
// with Q-register divides, fog clamp and fixed-point conversion); the C states the arithmetic in
// floats with small static helpers, so the packet header, the per-vertex record writes and the
// call sequence line up but the transforms do not.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md. First-level lane 2026-10-01
// (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// The textured ribbon (strip) packet of the cable / trail nodes (called from
// func_0021A500; port em_area06_port_strip.c a6_001CE860). The original is C
// with hand-written VU0 macro sections; this C states the same arithmetic in
// floats.
//  1. Clip matrix func_001CD370(0) and camera matrix 0x70003AC0 are loaded;
//     0x70003400 gets the screen matrix 0x70003A40 with its last row set to
//     (0, 0, 0, 1). The sort key comes from func_001CCF70(points).
//  2. func_001CB5F0 reserves 5n + 4 quadwords on chain D_0028F700 +
//     (chain_no << 15) + 0x4D3EC0: VIF DIRECT 5n + 3, an A+D GIF tag that
//     writes `tex` to TEX0_1, then the strip tag (n vertices pairs, PACKED,
//     registers RGBAQ, ST, XYZF2, ST, XYZF2).
//  3. Each point (w = 1): through the clip matrix, any of the six clip flags
//     sets the ADC bit 0x8000 for its two vertices; through the camera
//     matrix: xy /= w, w -= 3.0, z /= the new w, w = clamp(fog.z + fog.w * w,
//     0, fog.x) with the fog quadword D_00275670 + 0xA0, to 12.4 fixed point
//     (double-buffered on the stack by the parity `side`); 1 / camera w is
//     kept per side. The vertex record (0x50 bytes) is the colour as
//     integers, ST (t, 0, 1), XYZF, ST (t, 1, 1), XYZF; t alternates 0 / 1.
//  4. From the second point on, the screen-space normal of the segment
//     (-(dy), dx) scaled to `width` / length, 1 / w and 1 at 0x70003630,
//     goes through 0x70003400 (func_001026A0) and, times 16 / w, to integers;
//     the previous record's two XYZF positions are pushed apart by it (+ to
//     the first, - to the second), and so are the last record's.
//  5. func_001CB900 closes the packet with `mode`.
extern float *func_001CD370(int n);
extern int func_001CCF70(void *points);
extern char *func_001CB5F0(char *chain, int id, int qwc);
extern void func_001CB900(char *chain, int id, int mode);
extern void func_001026A0(float *dst, float *m, float *v);
extern float func_0011E748(float);   /* sqrtf */
extern int float_to_int(float);
extern char *D_00275670;
extern char D_0028F700[];

static void xform(float *out, float *m, float *v) {
    int i;

    for (i = 0; i < 4; i++) {
        out[i] = m[i] * v[0] + m[4 + i] * v[1] + m[8 + i] * v[2] + m[12 + i];
    }
}

static int ftoi4(float x) {
    return (int)(x * 16.0f);
}

void func_001CE860(int chain_no, int mode, float (*points)[4], float *colour, int n,
                   unsigned long long tex, float width) {
    float *clip;
    float *fog;
    float *nrm = (float *)0x70003630;
    unsigned int scr[2][4];
    float raw[4];
    float inv_w[2];
    float c[4];
    float lim;
    float t;
    float len;
    float k;
    int side;
    int adc;
    int key;
    int i;
    int j;
    char *pkt;
    char *rec;
    char *prev;
    unsigned long long *q;

    clip = func_001CD370(0);
    side = 0;
    t = 0.0f;
    fog = (float *)(D_00275670 + 0xA0);
    for (j = 0; j < 16; j++) {
        ((float *)0x70003400)[j] = ((float *)0x70003A40)[j];
    }
    ((float *)0x70003400)[12] = 0.0f;
    ((float *)0x70003400)[13] = 0.0f;
    ((float *)0x70003400)[14] = 0.0f;
    ((float *)0x70003400)[15] = 1.0f;
    key = func_001CCF70(points);

    pkt = func_001CB5F0(D_0028F700 + (chain_no << 15) + 0x4D3EC0, key, n * 5 + 4);
    q = (unsigned long long *)pkt;
    q[0] = 0;
    q[1] = 0;
    *(unsigned int *)(pkt + 0xC) = 0x50000000 | (n * 5 + 3);
    q[2] = 0x1000000000000001ULL;
    q[3] = 0xE;
    q[4] = tex;
    q[5] = 6;
    q[6] = (0x502E4000ULL << 32) | 0x8000 | (long long)n;
    q[7] = 0x42421;
    rec = pkt + 0x40;
    prev = rec;

    for (i = 0; i < n; i++) {
        xform(c, clip, points[i]);
        lim = (c[3] < 0.0f) ? -c[3] : c[3];
        adc = 0;
        for (j = 0; j < 3; j++) {
            if (c[j] > lim || c[j] < -lim) {
                adc = 0x8000;
            }
        }

        xform(raw, (float *)0x70003AC0, points[i]);
        c[0] = raw[0] * (1.0f / raw[3]);
        c[1] = raw[1] * (1.0f / raw[3]);
        c[3] = raw[3] - 3.0f;
        c[2] = raw[2] * (1.0f / c[3]);
        c[3] = fog[2] + fog[3] * c[3];
        if (c[3] > fog[0]) c[3] = fog[0];
        if (c[3] < 0.0f) c[3] = 0.0f;
        for (j = 0; j < 4; j++) {
            scr[side][j] = ftoi4(c[j]);
        }
        inv_w[side] = 1.0f / raw[3];

        ((int *)rec)[0] = float_to_int(colour[0]);
        ((int *)rec)[1] = float_to_int(colour[1]);
        ((int *)rec)[2] = float_to_int(colour[2]);
        ((int *)rec)[3] = float_to_int(colour[3]);
        ((float *)rec)[4] = t;
        ((float *)rec)[5] = 0.0f;
        ((float *)rec)[6] = 1.0f;
        ((unsigned int *)rec)[8] = scr[side][0];
        ((unsigned int *)rec)[9] = scr[side][1];
        ((unsigned int *)rec)[10] = scr[side][2];
        ((unsigned int *)rec)[11] = adc | scr[side][3];
        ((float *)rec)[12] = t;
        ((float *)rec)[13] = 1.0f;
        ((float *)rec)[14] = 1.0f;
        ((unsigned int *)rec)[16] = scr[side][0];
        ((unsigned int *)rec)[17] = scr[side][1];
        ((unsigned int *)rec)[18] = scr[side][2];
        ((unsigned int *)rec)[19] = adc | scr[side][3];

        if (i != 0) {
            nrm[1] = (float)scr[side][0] - (float)scr[1 - side][0];
            nrm[0] = -((float)scr[side][1] - (float)scr[1 - side][1]);
            len = func_0011E748(nrm[0] * nrm[0] + nrm[1] * nrm[1]);
            k = width * (1.0f / len);
            *(float *)0x7000368C = k;
            nrm[0] *= k;
            nrm[1] *= k;
            nrm[2] = inv_w[side];
            nrm[3] = 1.0f;
            func_001026A0(nrm, (float *)0x70003400, nrm);
            ((int *)nrm)[0] = float_to_int(16.0f * nrm[0] * inv_w[side]);
            ((int *)nrm)[1] = float_to_int(16.0f * nrm[1] * inv_w[side]);
            ((int *)prev)[8] += ((int *)nrm)[0];
            ((int *)prev)[9] += ((int *)nrm)[1];
            ((int *)prev)[16] -= ((int *)nrm)[0];
            ((int *)prev)[17] -= ((int *)nrm)[1];
            if (i == n - 1) {
                ((int *)rec)[8] += ((int *)nrm)[0];
                ((int *)rec)[9] += ((int *)nrm)[1];
                ((int *)rec)[16] -= ((int *)nrm)[0];
                ((int *)rec)[17] -= ((int *)nrm)[1];
            }
        }
        side = 1 - side;
        prev = rec;
        t = 1.0f - t;
        rec += 0x50;
    }
    func_001CB900(D_0028F700 + (chain_no << 15) + 0x4D3EC0, key, mode);
}
