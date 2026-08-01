// NEARMISS func_001CE660  (vram 0x001CE660, 0x1F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 39.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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

/* Column-major: p' = c0*p.x + c1*p.y + c2*p.z + c3*p.w */
typedef struct { Vec4 c0, c1, c2, c3; } Mtx4;

typedef struct {
    int   steps;
    float start_deg;
    float end_deg;
    float in_rx,  in_rz,  in_y;
    float out_rx, out_rz, out_y;
} RingBand;

/* func_0011E2A8 = sinf, func_0011DE90 = cosf (pinned in docs/FINDINGS.md). */
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);

/* Textured-quad emitter: 4 consecutive Vec4 at `quad`. */
extern void func_001CDDC0(int layer, int tag, Vec4 *quad,
                          unsigned long long tex0, unsigned int rgba);

/*
 * The VU0 macro-mode kernel, written out scalar. The original is
 *   vmulax.xyzw  ACC, mtx.c0, p.x
 *   vmadday.xyzw ACC, mtx.c1, p.y
 *   vmaddaz.xyzw ACC, mtx.c2, p.z
 *   vmaddw.xyzw  out, mtx.c3, vf0.w   ; vf0.w is hardwired to 1.0
 * i.e. one four-wide matrix-vector multiply with an implicit w of 1.0.
 */
static void vu0_mat4_mul_point(const Mtx4 *m, Vec4 *p)
{
    float x = p->x, y = p->y, z = p->z;

    p->x = m->c0.x * x + m->c1.x * y + m->c2.x * z + m->c3.x;
    p->y = m->c0.y * x + m->c1.y * y + m->c2.y * z + m->c3.y;
    p->z = m->c0.z * x + m->c1.z * y + m->c2.z * z + m->c3.z;
    p->w = m->c0.w * x + m->c1.w * y + m->c2.w * z + m->c3.w;
}

void func_001CE660(int layer, int tag, const Mtx4 *mtx, const RingBand *band,
                   unsigned long long tex0, unsigned int rgba)
{
    Vec4 *quad = (Vec4 *)0x70003400;    /* 4 qwords of EE scratchpad */
    float *spill_sin = (float *)0x70003680;
    float *spill_cos = (float *)0x70003684;
    float angle, step;
    int i;

    angle = band->start_deg * 3.14159274f / 180.0f;
    step  = (band->end_deg * 3.14159274f / 180.0f - angle)
            / (float)(band->steps - 1);

    for (i = 0; i < band->steps; i++) {
        /* Odd samples fill quad[0..1]; even samples fill quad[2..3]. */
        Vec4 *v = (i & 1) ? &quad[0] : &quad[2];
        float s, c;

        *spill_sin = s = func_0011E2A8(angle);
        *spill_cos = c = func_0011DE90(angle);

        v[0].x = band->in_rx * s;
        v[0].y = band->in_y;
        v[0].z = band->in_rz * c;
        v[0].w = 1.0f;

        v[1].x = band->out_rx * s;
        v[1].y = band->out_y;
        v[1].z = band->out_rz * c;
        v[1].w = 1.0f;

        vu0_mat4_mul_point(mtx, &v[0]);
        vu0_mat4_mul_point(mtx, &v[1]);

        if (i != 0) {
            func_001CDDC0(layer, tag, quad, tex0, rgba);
        }
        angle += step;
    }
}
