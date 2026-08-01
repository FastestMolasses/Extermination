// NEARMISS func_001F8D30  (vram 0x001F8D30, 0x3D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 42.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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

/* --- byte-matched / documented helpers used here ------------------------- */
extern void  func_001029C0(Vec4 *m);                       /* m = identity   */
extern void  func_00102A60(Vec4 *dst, Vec4 *src, float a);  /* dst = src * rotZ(a) */
extern void  func_001CD390(Vec4 *dst, const Vec4 *normal);  /* basis from normal */
extern void  func_001026D0(Vec4 *dst, Vec4 *m, Vec4 *src);  /* dst = src * m  */
extern void  func_00102918(Vec4 *dst, Vec4 *src, const Vec4 *t); /* dst = src, row3 += t */
extern void  func_00102900(Vec4 *dst, const Vec4 *v, float s);   /* dst = v * s */
extern float func_0011E620(float y, float x);               /* atan2f        */
extern float func_0011DF78(float x);                        /* fabsf         */
extern int   func_00128250(float f);                        /* float -> int  */
/* Builds and submits the GS packet for `n` quads given world-space corners. */
extern void  func_001CE300(int n, const Vec4 *corners,
                           unsigned long long gs_state, unsigned int rgba);

/* EE scratchpad: the shared projected-point slots and the camera's
   world->screen matrix block. */
#define SPR_P0         ((volatile int *)0x70003600)  /* x,y,z 12.4, w depth */
#define SPR_P1         ((volatile int *)0x70003610)
#define SPR_P2         ((volatile int *)0x70003620)
#define SPR_SCREEN_MTX ((const Vec4 *)0x70003AC0)

/* vf23: globally reserved depth-fade constants (.x ceiling, .z bias, .w slope). */
extern float VF23_x, VF23_z, VF23_w;

extern int float_to_int(float f);
static int vu0_ftoi4(float f) { return float_to_int(f * 16.0f); }

/* VU0 row-vector * 4x4 with v.w forced to 1.0 (the vf0.w operand). */
static Vec4 vu0_transform(const Vec4 *m, Vec4 v) {
    Vec4 r;
    r.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z + m[3].x;
    r.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z + m[3].y;
    r.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z + m[3].z;
    r.w = m[0].w * v.x + m[1].w * v.y + m[2].w * v.z + m[3].w;
    return r;
}

/* Project a world point into the shared scratchpad slot: transform, divide
   x,y,z by w, run w through the depth-fade ramp, convert all four to 12.4. */
static void vu0_project(volatile int *out, Vec4 p) {
    Vec4 v = vu0_transform(SPR_SCREEN_MTX, p);
    float depth;

    v.x = v.x / v.w;
    v.y = v.y / v.w;
    v.z = v.z / v.w;

    depth = VF23_z + VF23_w * v.w;
    if (depth > VF23_x) depth = VF23_x;
    if (depth < 0.0f)   depth = 0.0f;

    out[0] = vu0_ftoi4(v.x);
    out[1] = vu0_ftoi4(v.y);
    out[2] = vu0_ftoi4(v.z);
    out[3] = vu0_ftoi4(depth);
}

void func_001F8D30(const float *owner, const Vec4 *contact, const Vec4 *normal,
                   Vec4 facing, const Vec4 *colour,
                   float half_w, float half_h, float fade_rng) {
    Vec4 basis[4];        /* sp+0x70: decal world matrix                     */
    Vec4 surf[4];         /* sp+0xB0: orientation built from `normal`        */
    Vec4 corner[4];       /* sp+0xF0: the four world-space corners           */
    Vec4 tinted;          /* sp+0x130: colour scaled by the fade factor      */
    float yaw, fade;
    unsigned int rgba;
    int dx01, dy12, dy01, dx12, cross;

    /* --- 1. build the decal's world matrix -------------------------------- */
    func_001029C0(basis);                                   /* identity      */
    yaw = func_0011E620(facing.z, facing.x);                /* atan2f        */
    func_00102A60(basis, basis, yaw);                       /* spin in-plane */
    func_001CD390(surf, normal);                            /* surface basis */
    func_001026D0(basis, surf, basis);                      /* basis *= surf */
    func_00102918(basis, basis, contact);                   /* translate     */

    /* --- 2. vertical fade -------------------------------------------------- */
    if (fade_rng == 0.0f) {
        fade = 1.0f;
    } else {
        fade = 1.0f - (func_0011DF78(owner[1] - contact->y) / fade_rng);
        if (fade > 1.0f) fade = 1.0f;
        if (fade < 0.1f) fade = 0.1f;
    }

    func_00102900(&tinted, colour, fade);
    rgba =  (unsigned int)func_00128250(tinted.x)
         | ((unsigned int)func_00128250(tinted.y) << 8)
         | ((unsigned int)func_00128250(tinted.z) << 16)
         | ((unsigned int)func_00128250(tinted.w) << 24);

    /* --- 3. corners of the quad, local Z = 0, into world space ------------- */
    corner[0].x = -half_w; corner[0].y = -half_h; corner[0].z = 0.0f; corner[0].w = 1.0f;
    corner[0] = vu0_transform(basis, corner[0]);
    corner[1].x =  half_w; corner[1].y = -half_h; corner[1].z = 0.0f; corner[1].w = 1.0f;
    corner[1] = vu0_transform(basis, corner[1]);
    corner[2].x = -half_w; corner[2].y =  half_h; corner[2].z = 0.0f; corner[2].w = 1.0f;
    corner[2] = vu0_transform(basis, corner[2]);
    corner[3].x =  half_w; corner[3].y =  half_h; corner[3].z = 0.0f; corner[3].w = 1.0f;
    corner[3] = vu0_transform(basis, corner[3]);

    /* --- 4. screen-space winding test (only the first three are needed) ---- */
    vu0_project(SPR_P0, corner[0]);
    vu0_project(SPR_P1, corner[1]);
    vu0_project(SPR_P2, corner[2]);

    dx01 = SPR_P1[0] - SPR_P0[0];      /* P1.x - P0.x */
    dy12 = SPR_P2[1] - SPR_P1[1];      /* P2.y - P1.y */
    dy01 = SPR_P1[1] - SPR_P0[1];      /* P1.y - P0.y */
    dx12 = SPR_P2[0] - SPR_P1[0];      /* P2.x - P1.x */
    /* The target issues these two as `mult` / `mult1` so they run on both EE
       integer-multiply pipelines; semantically it is a plain 2D cross product. */
    cross = (dx01 * dy12) - (dy01 * dx12);
    if (cross < 0) {
        return;                                 /* back-facing: draw nothing  */
    }

    /* --- 5. submit ---------------------------------------------------------
       NOTE: func_001CE300 re-projects the corners itself and overwrites the
       scratchpad slots used above with the unpacked colour channels. */
    func_001CE300(1, corner, 0x2004290511322469ULL, rgba);
}
