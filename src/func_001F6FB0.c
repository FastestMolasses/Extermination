// NEARMISS func_001F6FB0  (vram 0x001F6FB0, 0x800 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 62.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The logic follows the instructions; remaining differences:
// the original transforms the spark offset and blends the colour
// with VU0 macro instructions (quadword loads into vf registers, lane-broadcast
// multiplies, ACC multiply-add, per-lane max / min); this C writes them as FPU
// arithmetic (the VU0 results are not IEEE-rounded; the port models them with its
// em_vu helpers), which changes the middle of each spark iteration. Register
// allocation and the scheduling of the three LCG draws differ as well.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Glow effect node (parent at self+0x24, work block self+0x1F0: [0] phase, [1] fade,
// +8 seed, +0xC running seed). See the comments in the body.
extern void func_00102948(void *dst, void *src);
extern void copy_qw4(void *dst, void *src);
extern int func_00122BB8(void);
extern int float_to_int(float f);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern void func_001029C0(void *m);
extern void func_00102918(void *dst, void *src, void *v);
extern void func_00102900(void *dst, void *src, float s);
extern int func_001CA7B0(char *p, float x);
extern void func_001C7900(void *m, void *rgb, int a, int b);
extern int func_001C6120(int font, int code);
extern void func_001CA940(int a, int b);
extern int func_001CCF70(void *pos);
extern void func_001CFA60(void *out, void *m, float a, float b);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(void *self);

extern int D_0028A56C;
extern float D_0025D800[];
extern float D_0025D810;
extern float D_0025D814;
extern float D_0025D818;
extern float D_70003A20;
extern float D_700036A0[4];     /* colour at the spark's start (r, g, b, a) */
extern float D_700036B0[4];     /* colour at the spark's end */
extern char D_700036E0[];       /* the spark's matrix, rows 0x700036E0 / F0 / 0x70003700 */
extern char D_700036F0[];
extern char D_70003700[];
extern char D_70003710[];
extern float D_700038A0[4];     /* the spark's radii (x, y, z, 1) */
extern float D_700038B0[4];     /* the spark's position */
extern float D_700038C0[4];     /* the spark's colour */

/* One LCG step of the seed at *seed: (high half / 65535) + 1e-4, then
 * seed = seed * 37 + 11. */
#define GLOW_UNIT(seed) ((float)(((seed) >> 16) & 0xFFFF) / 65535.0f + 0.0001f)

void func_001F6FB0(unsigned char *self) {
    unsigned char *parent = *(unsigned char **)(self + 0x24);
    float *work = (float *)(self + 0x1F0);          /* [0] phase, [1] fade, +8 seed, +0xC running seed */
    unsigned char *pwork = parent + 0x1F0;
    float g;
    float f;
    float ang;
    float spin;
    float t;
    float v[3];
    int seed;
    int i;
    int h;
    int lane;
    float blend[2];
    char sprite[0x60];

    switch (self[4]) {
    case 2:
    case 3:
        func_001AFC10(self);
        return;
    case 0:
        func_00102948(self + 0x60, parent + 0x60);
        work[0] = 0.0f;
        work[1] = 0.0f;
        *(int *)(self + 0x1F8) = func_00122BB8();
        self[4] = 1;
        self[5] = 0;
        /* fall through */
    case 1:
        break;
    default:
        return;
    }

    if (parent[4] == 3) {
        self[4] = 3;
        return;
    }
    copy_qw4(self + 0xD0, parent + 0xD0);
    func_00102948(self + 0x100, parent + 0xB0);
    *(int *)(self + 0x1FC) = *(int *)(self + 0x1F8);

    switch (self[5]) {
    case 0:
        work[1] += 0.016f;
        if (*(short *)(pwork + 0xF4) & 0x1000) {
            work[1] = 0.0f;
            self[5]++;
        }
        break;
    case 1:
        *(float *)(parent + 0x60) = *(float *)(self + 0x60) * (1.0f - work[1]);
        *(float *)(parent + 0x68) = *(float *)(self + 0x68) * (1.0f - work[1]);
        *(float *)(parent + 0x64) = *(float *)(self + 0x64) * (1.0f - work[1]);
        *(float *)(parent + 0x80) = 1.0f - 0.5f * work[1];
        *(float *)(parent + 0x84) = 1.0f - 0.5f * work[1];
        *(float *)(parent + 0x88) = 1.0f - work[1];
        *(float *)(self + 0x80) = *(float *)(parent + 0x80);
        *(float *)(self + 0x84) = *(float *)(parent + 0x84);
        *(float *)(self + 0x88) = *(float *)(parent + 0x88);
        work[1] = work[1] + 0.016f;
        if (!(work[1] <= 1.0f)) {
            self[5]++;
        }
        break;
    case 2:
        *(float *)(parent + 0x60) = 0.0f;
        *(float *)(parent + 0x68) = 0.0f;
        *(float *)(parent + 0x64) = 0.0f;
        self[5]++;
        break;
    }

    /* The glow's size grows with the phase up to 1.5, then holds. */
    D_70003A20 = work[0] / 1.5f;
    D_70003A20 = (D_70003A20 <= 1.0f) ? D_70003A20 : 1.0f;
    g = D_70003A20;
    D_700036A0[3] = 1.0f;
    D_700036A0[2] = 1.0f;
    D_700036A0[1] = 1.0f;
    D_700038A0[0] = 1.2f * g;
    D_700038A0[1] = 0.2f;
    D_700036A0[0] = 1.0f;
    D_700036B0[3] = 0.0f;
    D_700036B0[2] = 0.0f;
    D_700036B0[1] = 0.0f;
    D_700038A0[2] = 4.8f * g;
    D_700038A0[3] = 1.0f;
    D_700036B0[0] = 0.0f;

    if (work[0] < 3.5f) {
        for (i = 0; i < 24; i++) {
            /* The spark's age: 2 * phase - 2 * unit, wrapped into [0, 1) by its
             * integer part; -1 (no spark) outside [0, 4]. */
            seed = *(int *)(self + 0x1FC);
            f = 2.0f * work[0] - 2.0f * GLOW_UNIT(seed);
            *(int *)(self + 0x1FC) = seed * 37 + 11;
            if (f < 0.0f || !(f <= 4.0f)) {
                f = -1.0f;
            } else {
                f = f - (float)float_to_int(f);
            }

            /* Its direction: an angle in [-pi, pi) and a tilt in [0, pi/2). */
            seed = *(int *)(self + 0x1FC);
            t = GLOW_UNIT(seed);
            *(int *)(self + 0x1FC) = seed * 37 + 11;
            seed = *(int *)(self + 0x1FC);
            ang = 6.2831855f * t - 3.1415927f;
            spin = 0.5f * (3.1415927f * GLOW_UNIT(seed));
            *(int *)(self + 0x1FC) = seed * 37 + 11;
            D_700038B0[0] = D_700038A0[0] * func_0011DE90(ang);
            D_700038B0[1] = D_700038A0[1] * func_0011DE90(spin);
            D_700038B0[2] = D_700038A0[2] * func_0011E2A8(ang);
            D_700038B0[3] = 1.0f;

            /* VU0: scale the offset by the age, transform it by the node's
             * matrix (self +0xD0, rows x, y, z, translation) and add
             * 0 * age * age to y. */
            {
                float *row = (float *)(self + 0xD0);
                float zero = 0.0f;
                v[0] = D_700038B0[0] * f;
                v[1] = D_700038B0[1] * f;
                v[2] = D_700038B0[2] * f;
                zero = zero * (f * f);
                D_700038B0[0] = row[0] * v[0] + row[4] * v[1] + row[8] * v[2] + row[12];
                D_700038B0[1] = row[1] * v[0] + row[5] * v[1] + row[9] * v[2] + row[13] + zero;
                D_700038B0[2] = row[2] * v[0] + row[6] * v[1] + row[10] * v[2] + row[14];
                D_700038B0[3] = row[3] * v[0] + row[7] * v[1] + row[11] * v[2] + row[15];
            }

            /* Its colour: the start colour while the age is at most 0.5, then
             * (end - start) * ((age - 0.5) / 0.5) + start clamped to [0, 255]
             * (VU0 max / min per lane). */
            if (f <= 0.5f) {
                func_00102948(D_700038C0, D_700036A0);
            } else {
                blend[0] = (f - 0.5f) / 0.5f;
                blend[1] = 255.0f;
                for (lane = 0; lane < 4; lane++) {
                    t = (D_700036B0[lane] - D_700036A0[lane]) * blend[0] + D_700036A0[lane];
                    if (t < 0.0f) t = 0.0f;
                    if (t > blend[1]) t = blend[1];
                    D_700038C0[lane] = t;
                }
            }

            if (f < 0.0f) {
                continue;
            }
            func_001029C0(D_700036E0);
            func_00102918(D_700036E0, D_700036E0, D_700038B0);
            func_00102900(D_700036E0, D_700036E0, D_700038C0[0]);
            func_00102900(D_700036F0, D_700036F0, D_700038C0[1]);
            func_00102900(D_70003700, D_70003700, D_700038C0[2]);
            h = func_001CA7B0(D_70003710, 10.0f);
            if (h >= 0) {
                func_001C7900(D_700036E0, self + 0x80, 0x3F5, 0);
                func_001CA940(h, func_001C6120(D_0028A56C, 6));
            }
        }
    }

    /* The glow sprite at the node's position. */
    if (work[0] < 3.5f) {
        h = func_001CCF70(self + 0x100);
        if (h != 0xFFFFFF) {
            f = (float)*(int *)(self + 0x1FC) / 2147483648.0f;
            D_0025D810 = D_700038A0[0];
            D_0025D814 = D_700038A0[1];
            D_0025D818 = D_700038A0[2];
            func_001CFA60(sprite, self + 0xD0, work[0], f);
            func_001CFBE0(h, 1, D_0025D800, sprite, 1);
        }
    }

    work[0] = work[0] + 0.016f;
    if (!(work[0] <= 4.0f)) {
        parent[4] = 3;
        self[4] = 3;
    }
}
