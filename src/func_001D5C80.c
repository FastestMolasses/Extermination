// NEARMISS func_001D5C80  (vram 0x001D5C80, 0x728 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 47.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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
extern char *D_0028A5A0[];        /* [0] = base of the static-object bank */

/* EE scratchpad (0x70000000) scratch matrices, 4x4 float, row-major. */
extern float D_70003400[16];      /* screen-frustum world->clip matrix    */
extern float D_70003440[16];      /* guard-band world->clip matrix        */

extern int  float_to_int(float v);        /* SDK float->int (truncating)  */
extern char *func_001C6120(char *bank, int id);
extern void func_001D2D20(float *m, float focal, float w, float h,
                          float near, float far);
extern void func_001026D0(float *dst, float *m, float *v); /* dst = m * v  */
extern void func_001D4CD0(void);          /* attach clipping VIF/VU chain */
extern void func_001D4FB0(char *obj);     /* emit obj geometry to chan 0  */
extern void func_001D4B50(char *obj);     /* emit obj through clip path   */
extern void func_001D1F80(int chan, int a, int b); /* push render state   */
extern void func_001D1FF0(int chan, int a);        /* push render state   */

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

void func_001D5C80(float *pos)
{
    char *ctx;
    int *grid;
    int stride;
    float cellX, cellZ, origX, origZ;
    int cx, cz;
    int x0, x1, z0, z1;
    int ix, iz, slot;
    float zoom;

    ctx    = D_00275670;
    grid   = *(int **)(ctx + 0x140);
    stride = *(int *)(ctx + 0x148);
    cellX  = *(float *)(ctx + 0x150);
    cellZ  = *(float *)(ctx + 0x154);
    origX  = *(float *)(ctx + 0x158);
    origZ  = *(float *)(ctx + 0x15C);

    /* Centre cell, and a +-15 world-unit box around the position. */
    cx = float_to_int(1.0f + (pos[0] - origX) / cellX);
    cz = float_to_int(1.0f + (pos[2] - origZ) / cellZ);

    x0 = float_to_int(((pos[0] - origX) - 15.0f) / cellX - 0.5f);
    x1 = float_to_int((15.0f + (pos[0] - origX)) / cellX + 0.5f);
    z0 = float_to_int(((pos[2] - origZ) - 15.0f) / cellZ - 0.5f);
    z1 = float_to_int((15.0f + (pos[2] - origZ)) / cellZ + 0.5f);

    /* Always cover at least the 3x3 cells around the centre... */
    if (x0 >= cx - 1) x0 = cx - 1;
    if (x1 <= cx + 1) x1 = cx + 1;
    if (z0 >= cz - 1) z0 = cz - 1;
    if (z1 <= cz + 1) z1 = cz + 1;

    /* ...then clamp into the 32x32 grid. */
    if (x0 < 0)       x0 = 0;
    if (x1 >= 0x20)   x1 = 0x1F;
    if (z0 < 0)       z0 = 0;
    if (z1 >= 0x20)   z1 = 0x1F;

    func_001D4CD0();
    func_001D1FF0(0, 0);

    /* Rebuild both clip matrices in scratchpad: P * V, in place. */
    zoom = *(float *)(ctx + 0x2468);
    func_001D2D20(D_70003440, zoom, 4096.0f, 4096.0f, 0.1f, 16711680.0f);
    func_001026D0(D_70003440, D_70003440, D_00810610);
    func_001D2D20(D_70003400, zoom, 1024.0f, 448.0f, 0.1f, 16711680.0f);
    func_001026D0(D_70003400, D_70003400, D_00810610);

    /* vf24..vf27 are held loaded with the screen matrix across the whole sweep;
     * the guard-band retest below swaps them out and back. */

    for (iz = z0; iz <= z1; iz++) {
        for (ix = x0; ix <= x1; ix++) {
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
                    func_001D1F80(0, 2, 6);
                    func_001D4B50(obj);
                    func_001D4CD0();
                }
                /* original reloads vf24..vf27 from the screen matrix here */
            }
        }
    }

    func_001D1FF0(0, 1);
}
