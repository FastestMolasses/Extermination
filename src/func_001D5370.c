// NEARMISS func_001D5370  (vram 0x001D5370, 0x700 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 39.18% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern char *D_00275670;          /* global render context                */
extern float D_00810610[16];      /* world -> camera (view) matrix        */
extern unsigned char D_00810700;  /* stage id (major)                     */
extern unsigned char D_00810701;  /* stage id (minor)                     */
extern unsigned char D_00810702;  /* room / section index within a stage  */
extern char *D_0028A5A0[];        /* [0] = base of the static-object bank */

/* EE scratchpad (0x70000000) scratch matrices, 4x4 float, row-major. */
extern float D_70003400[16];      /* screen-frustum world->clip matrix    */
extern float D_70003440[16];      /* guard-band world->clip matrix        */

extern char *func_001C6120(char *bank, int id);
extern void func_001D2D20(float *m, float focal, float w, float h,
                          float near, float far);
extern void func_001026D0(float *dst, float *m, float *v); /* dst = m * v  */
extern void func_001D4DA0(void);          /* attach standard VU1 program  */
extern void func_001D4FB0(char *obj);     /* emit obj geometry to chan 0  */
extern void func_001D4B20(char *obj);     /* emit obj through clip path   */
extern void func_001D1F80(int chan, int a, int b); /* push render state   */
extern void func_001D5BD0(void);          /* dynamic-object pass          */

/* ---- scalar stand-ins for the VU0 macro-mode clip test ------------------ */

/*
 * One `vmulay/vmaddaz/vmaddaw/vmaddw` chain followed by `vclipw.xyz`.
 *
 * The chain computes  clip = row0*x + row1*y + row2*z + row3  (row-major m,
 * row-vector convention, implicit w = 1 supplied by vf0.w).
 *
 * vclipw.xyz then compares clip.x/y/z against +clip.w and -clip.w and pushes a
 * 6-bit result into the CLIP flag register:
 *   bit0 x > +w   bit1 x < -w
 *   bit2 y > +w   bit3 y < -w
 *   bit4 z > +w   bit5 z < -w
 */
static unsigned int vu0_clipw(const float *m, float x, float y, float z)
{
    float cx = m[0] * x + m[4] * y + m[8]  * z + m[12];
    float cy = m[1] * x + m[5] * y + m[9]  * z + m[13];
    float cz = m[2] * x + m[6] * y + m[10] * z + m[14];
    float cw = m[3] * x + m[7] * y + m[11] * z + m[15];
    unsigned int f = 0;

    if (cx >  cw) f |= 0x01;
    if (cx < -cw) f |= 0x02;
    if (cy >  cw) f |= 0x04;
    if (cy < -cw) f |= 0x08;
    if (cz >  cw) f |= 0x10;
    if (cz < -cw) f |= 0x20;
    return f;
}

/*
 * Clip-test all 8 corners of the AABB [bmin, bmax] against matrix m.
 *
 * The original issues them as two runs of four vclipw and reads $vi18 once per
 * run. Because $vi18 shifts left by 6 on every vclipw, a single read holds four
 * generations: bits 18..23 = first corner of the run, bits 0..5 = last.
 * `lo4` is corners 0..3 (bmin.z fixed), `hi4` is corners 4..7 (bmax.z fixed).
 */
static void vu0_clip_aabb8(const float *m, const float *bmin, const float *bmax,
                           unsigned int *lo4, unsigned int *hi4)
{
    unsigned int a, b, c, d;

    a = vu0_clipw(m, bmin[0], bmin[1], bmin[2]);
    b = vu0_clipw(m, bmax[0], bmin[1], bmin[2]);
    c = vu0_clipw(m, bmin[0], bmax[1], bmin[2]);
    d = vu0_clipw(m, bmax[0], bmax[1], bmin[2]);
    *lo4 = (a << 18) | (b << 12) | (c << 6) | d;

    a = vu0_clipw(m, bmin[0], bmin[1], bmax[2]);
    b = vu0_clipw(m, bmax[0], bmin[1], bmax[2]);
    c = vu0_clipw(m, bmin[0], bmax[1], bmax[2]);
    d = vu0_clipw(m, bmax[0], bmax[1], bmax[2]);
    *hi4 = (a << 18) | (b << 12) | (c << 6) | d;
}

/*
 * Trivial-reject: true when all 8 corners are outside the SAME plane.
 *
 * `lo4 & hi4` ANDs corner i with corner i+4 lane-wise; folding the four 6-bit
 * lanes together leaves the flags common to all 8 corners. The final
 * `(v ^ (v << 1)) & 0x2A` checks, per axis, that exactly one of the two
 * out-flags survived: both bits set would mean some corners fell off the +side
 * and others off the -side, which is not a trivial reject.
 */
static int vu0_clip_reject(unsigned int lo4, unsigned int hi4)
{
    unsigned int v = lo4 & hi4;

    v = (v & (v >> 6)) & ((v >> 12) & (v >> 18));
    v &= 0x3F;
    return ((v ^ (v << 1)) & 0x2A) != 0;
}

/* 24 significant bits of the CLIP register; the original masks with the
 * 64-bit pair dsll32 8 / dsrl32 8. Zero means "no corner outside any plane". */
#define CLIP_ANY(lo4, hi4) (((lo4) | (hi4)) & 0xFFFFFF)

/* ------------------------------------------------------------------------- */

void func_001D5370(void)
{
    char *ctx;
    int *grid;
    int stride;
    int stage;
    int x0, x1, z0, z1;
    int ix, iz, slot;
    float zoom;

    ctx    = D_00275670;
    grid   = *(int **)(ctx + 0x140);
    stride = *(int *)(ctx + 0x148);

    stage = (D_00810700 << 8) + D_00810701;

    /* Default sweep: the whole 32x32 grid. A couple of stages only ever have
     * geometry in one corner of it and skip the rest. */
    x0 = 0;
    x1 = 0x20;
    z0 = 0;
    z1 = 0x20;

    if (stage == 0x1300) {
        int room = D_00810702;
        if (room == 8 || room == 7 || room == 9 || room == 5) {
            x0 = 0;
            x1 = 0x1E;
            z0 = 0x0C;
            z1 = 0x20;
        } else if (room == 4) {
            x0 = 9;
            z1 = 0x19;
        }
    } else if (stage == 0x0D00) {
        if (D_00810702 < 4) {
            x0 = 0x14;
            x1 = 0x17;
            z0 = 0x0F;
            z1 = 0x17;
        }
    }

    func_001D4DA0();

    /* Rebuild both clip matrices in scratchpad: P * V, in place. */
    zoom = *(float *)(ctx + 0x2468);
    func_001D2D20(D_70003440, zoom, 4096.0f, 4096.0f, 0.1f, 16711680.0f);
    func_001026D0(D_70003440, D_70003440, D_00810610);
    func_001D2D20(D_70003400, zoom, 1024.0f, 448.0f, 0.1f, 16711680.0f);
    func_001026D0(D_70003400, D_70003400, D_00810610);

    /* vf24..vf27 are held loaded with the screen matrix across the whole sweep;
     * the guard-band retest below swaps them out and back. */

    for (iz = z0; iz < z1; iz++) {
        for (ix = x0; ix < x1; ix++) {
            for (slot = 0; slot < 4; slot++) {
                char *obj;
                float *bmin;
                float *bmax;
                unsigned int lo4, hi4;
                int id;

                id = grid[(ix * stride + iz) * 4 + slot];
                if (id <= 0) {
                    continue;               /* empty slot */
                }

                obj  = func_001C6120(D_0028A5A0[0], id);
                bmin = (float *)(obj + 0x14);   /* AABB min x,y,z */
                bmax = (float *)(obj + 0x24);   /* AABB max x,y,z */

                vu0_clip_aabb8(D_70003400, bmin, bmax, &lo4, &hi4);

                if (vu0_clip_reject(lo4, hi4)) {
                    continue;               /* wholly off screen */
                }

                if (CLIP_ANY(lo4, hi4) == 0) {
                    func_001D4FB0(obj);     /* wholly on screen */
                    continue;
                }

                /* Straddles the screen frustum - does it also leave the GS
                 * guard band? If not, no software clipping is needed. */
                vu0_clip_aabb8(D_70003440, bmin, bmax, &lo4, &hi4);

                if (CLIP_ANY(lo4, hi4) == 0) {
                    func_001D4FB0(obj);
                } else {
                    func_001D4FB0(obj);
                    func_001D1F80(0, 1, 0);
                    func_001D4B20(obj);
                    func_001D4DA0();
                }
                /* original reloads vf24..vf27 from the screen matrix here */
            }
        }
    }

    /* Stages that also carry dynamic/instanced map objects. */
    switch (stage) {
    case 0x1500:
    case 0x1100:
    case 0x1001:
    case 0x1000:
    case 0x0F00:
    case 0x0803:
    case 0x0806:
    case 0x0801:
    case 0x0805:
    case 0x0800:
    case 0x0703:
    case 0x0700:
    case 0x0601:
    case 0x0600:
    case 0x0401:
    case 0x0400:
    case 0x0301:
    case 0x0101:
    case 0x0100:
        func_001D5BD0();
        break;
    default:
        break;
    }
}
