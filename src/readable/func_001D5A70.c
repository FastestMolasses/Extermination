// NEARMISS func_001D5A70  (vram 0x001D5A70, 0x158 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 43.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// C with hand-written VU0 sections in the original (the clip matrix held in vf24..vf27 across the
// three vertex copies, vclipw judgements read from the clip-flag register); the C states the
// arithmetic with a static helper, so only the calls and the final flag logic line up.
//
// The function links from the asm body in src/func_001D5A70.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Triangle clip classification. The three vertices at +0x40, +0x80 and +0xC0
// (w taken as 1.0) go through the clip matrix func_001CD370(0); each gets
// the VU0 clip judgement against |w| (six flags: +x, -x, +y, -y, +z, -z).
// Returns 0xFF when one flag is set for all three vertices (the triangle is
// wholly outside one plane), 1 when any vertex has a flag set (it needs
// clipping), else 0. The original is C with hand-written VU0 sections (the
// matrix stays in vf24..vf27 across the copies); this C states the
// arithmetic.
extern float *func_001CD370(int n);
extern void func_00102948(void *dst, void *src);   /* quadword copy */

static int clip_flags(float *m, float *p) {
    float c[4];
    float w;
    int i;
    int f = 0;

    for (i = 0; i < 4; i++) {
        c[i] = m[i] * p[0] + m[4 + i] * p[1] + m[8 + i] * p[2] + m[12 + i] * p[3];
    }
    w = (c[3] < 0.0f) ? -c[3] : c[3];
    for (i = 0; i < 3; i++) {
        if (c[i] > w) f |= 1 << (2 * i);
        if (c[i] < -w) f |= 2 << (2 * i);
    }
    return f;
}

int func_001D5A70(int unused, char *tri) {
    float *m;
    float v[4];
    int f0;
    int f1;
    int f2;

    m = func_001CD370(0);
    func_00102948(v, tri + 0x40);
    v[3] = 1.0f;
    f0 = clip_flags(m, v) & 0x3F;
    func_00102948(v, tri + 0x80);
    v[3] = 1.0f;
    f1 = clip_flags(m, v) & 0x3F;
    func_00102948(v, tri + 0xC0);
    v[3] = 1.0f;
    f2 = clip_flags(m, v) & 0x3F;
    if (f2 & (f0 & f1)) {
        return 0xFF;
    }
    if (f2 | (f0 | f1)) {
        return 1;
    }
    return 0;
}
