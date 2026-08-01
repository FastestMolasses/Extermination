// NEARMISS func_001E4A00  (vram 0x001E4A00, 0x2D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 38.04% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct { float x, y, z, w; } Vec4;

extern int  float_to_int(float x);
extern int  func_00128250(float x);                  /* float -> int (soft) */
extern void func_00102948(void *dst, const void *src);   /* VU0 qword copy */

extern void func_001CD520(int kind, int mode, const Vec4 *pos,
                          unsigned long long tex, float w, float h, float depth,
                          unsigned int rgba);

extern Vec4 D_002541F0[2];       /* layer-1 colour ramp: stop0, stop1 (0..255) */
extern Vec4 D_00254210[2];       /* layer-2 colour ramp                        */
extern unsigned long long D_00253C50[10];   /* 10-frame sprite descriptor table */

/*
 * The VU0 lerp block, written scalar. `scratch` is the fixed EE-scratchpad qword
 * the original always uses.
 */
static void flash_ramp(const Vec4 *ramp, float t, Vec4 *scratch)
{
    float u, c;

    if (t <= 0.2f) {
        func_00102948(scratch, &ramp[0]);       /* hold the first stop */
        return;
    }

    u = (t - 0.2f) / 0.8f;

    c = ramp[0].x + (ramp[1].x - ramp[0].x) * u;
    scratch->x = (c < 0.0f) ? 0.0f : (c > 255.0f ? 255.0f : c);
    c = ramp[0].y + (ramp[1].y - ramp[0].y) * u;
    scratch->y = (c < 0.0f) ? 0.0f : (c > 255.0f ? 255.0f : c);
    c = ramp[0].z + (ramp[1].z - ramp[0].z) * u;
    scratch->z = (c < 0.0f) ? 0.0f : (c > 255.0f ? 255.0f : c);
    c = ramp[0].w + (ramp[1].w - ramp[0].w) * u;
    scratch->w = (c < 0.0f) ? 0.0f : (c > 255.0f ? 255.0f : c);
}

static unsigned int pack_rgba(const Vec4 *c)
{
    return (unsigned int)func_00128250(c->x)
         | ((unsigned int)func_00128250(c->y) << 8)
         | ((unsigned int)func_00128250(c->z) << 16)
         | ((unsigned int)func_00128250(c->w) << 24);
}

void func_001E4A00(char *obj, float age, float size)
{
    Vec4 *scratch = (Vec4 *)0x700038B0;
    const Vec4 *pos = (const Vec4 *)(obj + 0x30);
    unsigned int rgba1, rgba2;
    float t, frame_f, w, depth;
    int frame;

    if (!(age < 1.8f)) {
        return;                                  /* the flash has expired */
    }

    t       = age / 1.8f;
    frame_f = 10.0f * t;
    w       = size + (2.0f * size) * t;          /* size * (1 + 2t) */
    depth   = size / 2.0f;

    flash_ramp(D_002541F0, t, scratch);
    rgba1 = pack_rgba(scratch);

    flash_ramp(D_00254210, t, scratch);
    rgba2 = pack_rgba(scratch);

    frame = float_to_int(frame_f);

    func_001CD520(0, 2, pos, D_00253C50[frame], w, w, depth, rgba1);
    func_001CD520(0, 2, pos, 0x20045BA5154222DCULL,
                  2.0f * w, 2.0f * w, depth, rgba2);
}
