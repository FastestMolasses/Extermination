// NEARMISS func_001E0750  (vram 0x001E0750, 0x36C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 13.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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

/* Render context: +0x1C is the write cursor into the current command buffer. */
typedef struct { char pad[0x1C]; unsigned char *cur; } RenderCtx;
extern RenderCtx *D_00275670;

/* Returns the 4x4 clip matrix for camera slot `n` inside the render context
   (byte-matched: D_00275670 + (n << 6) + 0x2240). */
extern Vec4 *func_001CD370(int n);

/* Pseudo-random generator (byte-matched LCG): returns a 31-bit non-negative int. */
extern int func_00122BB8(void);

/* CodeWarrior runtime float -> int cast helper. */
extern int float_to_int(float f);

/* .rodata: read as 6 pairs of (x,y) corner offsets for the strip, in units that
   get scaled by 2048/w before being added to the projected centre. Sourced from
   the game's own data section — the port must take it from there, not hard-code
   it. NOTE for the port: only the first two pairs are clean float values; the
   remaining words decode as sub-denormal bit patterns that the PS2 FPU flushes
   to zero, so vertices 2..5 collapse onto the projected centre. That is what
   the original binary does; do not "fix" it without checking in-game. */
extern float D_00253530[12];

/* EE scratchpad. 0x70003600 is the shared "one projected point" slot; the
   0x70003AC0 block is the current world->screen matrix published by the camera. */
#define SPR_PROJECTED  ((volatile int *)0x70003600)      /* x,y,z (12.4), w (fade) */
#define SPR_SCREEN_MTX ((const Vec4 *)0x70003AC0)

/* vf23 is a globally reserved VU0 register holding the depth-fade constants
   published by the camera setup: .x = fade ceiling, .z = bias, .w = slope. */
extern float VF23_x, VF23_z, VF23_w;

/* --- VU0 macro-mode helpers, written out as plain scalar float math --------
   vmulax / vmadday / vmaddaz / vmaddw against ACC is exactly a row-vector by
   4x4 matrix multiply with v.w forced to 1.0 (the vf0.w operand). */
static Vec4 vu0_transform(const Vec4 *m, Vec4 v) {
    Vec4 r;
    r.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z + m[3].x;
    r.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z + m[3].y;
    r.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z + m[3].z;
    r.w = m[0].w * v.x + m[1].w * v.y + m[2].w * v.z + m[3].w;
    return r;
}

/* vclipw.xyz + `cfc2 $vi18; andi 0x3F`: the VU0 clip unit compares +-w against
   x,y,z and leaves six sticky bits in CLIPflag. Non-zero => outside the frustum.
   There is no C equivalent; this is the exact predicate it computes. */
static int vu0_clip_outside(Vec4 c) {
    float w = c.w, nw = -c.w;
    return (c.x > w) || (c.x < nw) ||
           (c.y > w) || (c.y < nw) ||
           (c.z > w) || (c.z < nw);
}

/* vftoi4: convert to signed 12.4 fixed point (truncating, saturating). */
static int vu0_ftoi4(float f) { return float_to_int(f * 16.0f); }

void func_001E0750(const Vec4 *pos, const int *style) {
    const Vec4 *clip_mtx;
    const Vec4 *screen_mtx;
    Vec4 clip, scr;
    unsigned int rgba_centre[4];
    unsigned int rgba_edge[4];
    float corner[12];
    float w, scale, depth;
    unsigned char *pkt;
    unsigned char *vtx;
    int i;

    /* --- frustum reject -------------------------------------------------- */
    clip_mtx = func_001CD370(0);            /* camera slot 0 clip matrix      */
    clip = vu0_transform(clip_mtx, *pos);
    if (vu0_clip_outside(clip)) {
        return;                             /* off-screen: emit nothing       */
    }

    /* --- vertex colours (GS PACKED RGBAQ: one 32-bit word per component) -- */
    rgba_centre[0] = 0x80;                  /* R */
    rgba_centre[1] = 0x80;                  /* G */
    rgba_centre[2] = 0x80;                  /* B */
    rgba_centre[3] = (unsigned int)style[3];/* A supplied by the caller       */
    rgba_edge[0]   = 0x80;
    rgba_edge[1]   = 0x80;
    rgba_edge[2]   = 0x80;
    rgba_edge[3]   = 0;                     /* rim is fully transparent       */

    /* --- project to screen ----------------------------------------------- */
    screen_mtx = SPR_SCREEN_MTX;
    scr = vu0_transform(screen_mtx, *pos);
    w = scr.w;                              /* clip w, kept before the divide */

    /* vdiv Q, vf0w, vf2w  +  vwaitq  +  vmulq.xy : perspective divide on x,y */
    scr.x = scr.x / scr.w;
    scr.y = scr.y / scr.w;

    /* The target subtracts a w-bias staged through qmtc2; here the bias is
       0.0f, so this is a no-op. (The sibling func_001E2800 uses 1.0f.)       */
    scr.w = scr.w - 0.0f;

    /* second vdiv / vwaitq / vmulq.z: perspective divide on z */
    scr.z = scr.z / scr.w;

    /* depth-fade: clamp(vf23.z + vf23.w * w, 0.0f, vf23.x) */
    depth = VF23_z + VF23_w * scr.w;
    if (depth > VF23_x) depth = VF23_x;
    if (depth < 0.0f)   depth = 0.0f;
    scr.w = depth;

    /* vftoi4.xyzw + sqc2 into the shared scratchpad projected-point slot */
    SPR_PROJECTED[0] = vu0_ftoi4(scr.x);
    SPR_PROJECTED[1] = vu0_ftoi4(scr.y);
    SPR_PROJECTED[2] = vu0_ftoi4(scr.z);
    SPR_PROJECTED[3] = vu0_ftoi4(scr.w);    /* written here, consumed elsewhere */

    /* --- screen-space size: shrinks with distance ------------------------- */
    scale = 8.0f * (256.0f / w);            /* == 2048/w, in 12.4 screen units */

    /* local copy of the corner-offset table (the target does this with three
       128-bit lq/sq pairs) */
    for (i = 0; i < 12; i++) {
        corner[i] = D_00253530[i];
    }

    /* --- build the VIF/GIF packet ---------------------------------------- */
    pkt = D_00275670->cur;
    *(short *)(pkt + 0x00) = 0x14;          /* engine packet descriptor       */
    *(char  *)(pkt + 0x03) = 0x10;
    *(int   *)(pkt + 0x04) = 0;
    *(int   *)(pkt + 0x10) = 0;             /* VIF nops ...                   */
    *(int   *)(pkt + 0x14) = 0;
    *(int   *)(pkt + 0x18) = 0;
    *(int   *)(pkt + 0x1C) = 0x50000013;    /* VIFcode DIRECT, 19 quadwords   */
    /* GIFtag: NLOOP=6, EOP=1, PRE=1, NREG=3, PACKED;
       PRIM = triangle strip + gouraud + texture + alpha blend;
       REGS = RGBAQ, ST, XYZ2. */
    *(long long *)(pkt + 0x20) = 0x302EC00000008006LL;
    *(long long *)(pkt + 0x28) = 0x421LL;
    D_00275670->cur = pkt + 0x150;          /* 0x30 header + 6*0x30 vertices  */

    vtx = pkt + 0x30;
    for (i = 0; i < 6; i++) {
        float jitter_s, jitter_t;
        float r0, r1;
        int sx, sy;

        /* Two independent uniform jitters, scaled by `scale`. 2^-31 turns the
           31-bit LCG output into [0,1); each is then mapped to +-half-range. */
        r0 = (float)func_00122BB8() * 4.6566129e-10f;
        jitter_s = scale * (r0 * 3.90625e-4f - 1.953125e-4f);

        r1 = (float)func_00122BB8() * 4.6566129e-10f;
        jitter_t = scale * (r1 * 8.9287e-4f - 4.46435e-4f);

        /* RGBAQ: only the first vertex is opaque. */
        if (i == 0) {
            *(unsigned int *)(vtx + 0x00) = rgba_centre[0];
            *(unsigned int *)(vtx + 0x04) = rgba_centre[1];
            *(unsigned int *)(vtx + 0x08) = rgba_centre[2];
            *(unsigned int *)(vtx + 0x0C) = rgba_centre[3];
        } else {
            *(unsigned int *)(vtx + 0x00) = rgba_edge[0];
            *(unsigned int *)(vtx + 0x04) = rgba_edge[1];
            *(unsigned int *)(vtx + 0x08) = rgba_edge[2];
            *(unsigned int *)(vtx + 0x0C) = rgba_edge[3];
        }

        /* XYZ2: projected centre + scaled corner offset, all in 12.4 fixed. */
        sx = float_to_int((float)SPR_PROJECTED[0] + scale * corner[i * 2 + 0]);
        sy = float_to_int((float)SPR_PROJECTED[1] + scale * corner[i * 2 + 1]);
        *(int *)(vtx + 0x20) = sx;
        *(int *)(vtx + 0x24) = sy;
        *(int *)(vtx + 0x28) = SPR_PROJECTED[2];   /* shared z for the strip  */
        *(int *)(vtx + 0x2C) = 0;

        /* ST: texture coords taken straight from the vertex's own screen
           position (12.4 -> /8192 in x, /4096 in y) plus the random jitter.
           This frame-buffer feedback is what produces the distortion. */
        *(float *)(vtx + 0x10) = jitter_s + (-3.5f    + (float)sx * 1.220703125e-4f);
        *(float *)(vtx + 0x14) = jitter_t + (-7.5625f + (float)sy * 2.44140625e-4f);
        *(int   *)(vtx + 0x18) = 0x3F800000;       /* Q = 1.0f                */

        vtx += 0x30;
    }
}
