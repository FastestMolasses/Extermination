// NEARMISS func_001D7C30  (vram 0x001D7C30, 0x370 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.68% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). Object similarity does not prove semantic equivalence.
// Matching wave 2026-10-27 (was 86.54%): control flow, struct layout and all GPR
// colouring of the first loop now match. Fixes read from the original instructions:
// the 0xF00 input test is a one-case switch whose case ends in `continue` (a for
// loop, so the increment runs); D_00810700/1 are plain (non-small-data) byte arrays;
// the jitter clamps are ternaries; each light is a 0x80-byte struct (the old level
// at +0x2C is the blend input); the second loop has no extra `count > 0` guard; the
// random value is held in an int before conversion.
// Remaining differences in this candidate:
// In both jitter blocks the float temporaries for the 2^-31 scale constant and the
// converted random value are coloured swapped (f1/f2); in the pending-copy loop the
// saved registers holding the pending count and the 0x80 stride offset are swapped
// (s2/s3). Pure register colouring; no source lever found (declaration order, temp
// placement, operand order, helper inlining all tried).
//
// Original assembly remains linked. The corrected candidate is compiled and
// measured separately; see docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Per-frame light update. For each of the 32 lights with a positive level: the new
// level is base + level * scale (floored at 0), the colour is scaled by scale, then
// the light matrix is reset to identity, unless scale is exactly 1, the input word
// at 0x810700 is not 0xF00 and mode is 1, in which case the X/Y rotation drifts by a
// random step in [-0.001, 0.001) clamped to +-0.0314 rad and the matrix is rebuilt
// from those two rotations. Then each queued light (count at +0x214) is copied
// into the first free slot (level <= 0) and the queue is cleared.

#define RNORM 4.656612873e-10f

extern void func_00102900(void *dst, void *src, float s);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *a0);
extern void func_00102B08(void *dst, void *src, float v);
extern void func_00102BB0(void *dst, void *src, float v);
extern int func_00122BB8(void);
typedef struct Light {
    float scale;      /* 0x00 */
    float base;       /* 0x04 */
    int mode;         /* 0x08 */
    int kind;         /* 0x0C */
    float dir[4];     /* 0x10 */
    float col[3];     /* 0x20 */
    float level;      /* 0x2C */
    float rx;         /* 0x30 */
    float ry;         /* 0x34 */
    float rz;         /* 0x38 */
    float pad;        /* 0x3C */
    float mtx[16];    /* 0x40 */
} Light;

typedef struct LightSys {
    char pad0[0x214];
    int count;              /* 0x214 */
    char pad1[0x8];
    Light light[32];        /* 0x220 */
    Light pending[32];      /* 0x1220 */
} LightSys;

extern LightSys *D_00275670;
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];

void func_001D7C30(void) {
    int i;
    Light *l;
    Light *src;
    Light *dst;
    int count;
    int n;
    int j;
    float blend;
    float f;
    float r;
    float scratch[16];

    for (i = 0; i < 0x20; i++) {
        l = &D_00275670->light[i];
        blend = l->level;
        if (blend > 0.0f) {
            f = l->base + blend * l->scale;
            if (f < 0.0f) {
                f = 0.0f;
            }
            l->level = f;
            l->col[0] = l->col[0] * l->scale;
            l->col[1] = l->col[1] * l->scale;
            l->col[2] = l->col[2] * l->scale;
            if (l->scale == 1.0f) {
                switch ((D_00810700[0] << 8) + D_00810701[0]) {
                case 0xF00:
                    func_001029C0(l->mtx);
                    continue;
                }
                if (l->mode == 1) {
                    n = func_00122BB8();
                    r = l->rx + (-0.001f + 0.002f * ((float)n * RNORM));
                    r = (r < -0.03141593f) ? -0.03141593f : r;
                    r = (r > 0.03141593f) ? 0.03141593f : r;
                    l->rx = r;
                    n = func_00122BB8();
                    r = l->ry + (-0.001f + 0.002f * ((float)n * RNORM));
                    r = (r < -0.03141593f) ? -0.03141593f : r;
                    r = (r > 0.03141593f) ? 0.03141593f : r;
                    l->ry = r;
                    func_001029C0(scratch);
                    func_00102B08(scratch, scratch, l->rx);
                    func_00102BB0(l->mtx, scratch, l->ry);
                } else {
                    func_001029C0(l->mtx);
                }
            } else {
                func_001029C0(l->mtx);
            }
        }
    }

    count = D_00275670->count;
    if (count != 0) {
        for (n = 0; n < count; n++) {
            src = &D_00275670->pending[n];
                for (j = 0; j < 0x20; j++) {
                    dst = &D_00275670->light[j];
                    if (dst->level <= 0.0f) {
                    dst->scale = src->scale;
                    dst->base = src->base;
                    func_00102948(dst->dir, src->dir);
                    func_00102900(dst->col, src->col, 128.0f);
                    dst->kind = src->kind;
                    dst->mode = src->mode;
                    dst->rx = 0;
                    dst->ry = 0;
                    dst->rz = 0;
                    break;
                }
            }
        }
        D_00275670->count = 0;
    }
}
