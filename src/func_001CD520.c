// NEARMISS func_001CD520  (vram 0x001CD520, 0x420 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 21.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern char *D_00275670;
extern char  D_00251220[];          /* static GIF state packet chased in step 7 */
extern char  D_0028F700[];          /* render-bucket page tables, stride 0x8000 */

extern char *func_001CD370(int camera);
extern char *func_001CB5F0(int page_table, int z, int nquads);
extern void  func_001CB6B0(int page_table, int z, int kind, unsigned long long addr);
extern void  func_001CB900(int page_table, int z, int mode);

/* ---- EE scratchpad staging (0x70000000 is the SPR) ------------------------ */
/* Projected sprite centre, 28.4 fixed point. */
#define SCR_X   (*(volatile int *)0x70003600)
#define SCR_Y   (*(volatile int *)0x70003604)
#define SCR_Z   (*(volatile int *)0x70003608)
#define SCR_FOG (*(volatile int *)0x7000360C)
/* Half-extent: floats going in, 28.4 fixed point coming out. */
#define EXT_X_F (*(volatile float *)0x70003610)
#define EXT_Y_F (*(volatile float *)0x70003614)
#define EXT_W_F (*(volatile float *)0x70003618)
#define EXT_X   (*(volatile int   *)0x70003610)
#define EXT_Y   (*(volatile int   *)0x70003614)

extern volatile float D_70003AC0[16];   /* world -> screen matrix */
extern volatile float D_70003A40[16];   /* projection-only matrix */

typedef struct Vec4 {
    float x, y, z, w;
} Vec4;

/* vmulax/vmadday/vmaddaz/vmaddw ACC chain: row-vector point transform,
   with w taken from vf0.w == 1.0 rather than from the source vector. */
static Vec4 vu0_transform_point(const volatile float *m, const Vec4 *v)
{
    Vec4 r;
    r.x = m[0] * v->x + m[4] * v->y + m[ 8] * v->z + m[12];
    r.y = m[1] * v->x + m[5] * v->y + m[ 9] * v->z + m[13];
    r.z = m[2] * v->x + m[6] * v->y + m[10] * v->z + m[14];
    r.w = m[3] * v->x + m[7] * v->y + m[11] * v->z + m[15];
    return r;
}

/* vclipw.xyz + cfc2 $vi18: 1 if any of +/-x, +/-y, +/-z falls outside +/-w. */
static int vu0_clip_outside(const Vec4 *p)
{
    return p->x >  p->w || p->x < -p->w
        || p->y >  p->w || p->y < -p->w
        || p->z >  p->w || p->z < -p->w;
}

/* vftoi4: float -> 28.4 fixed point, truncating toward zero. */
static int vu0_ftoi4(float f)
{
    return (int)(f * 16.0f);
}

int func_001CD520(int bucket, int mode, Vec4 *world, unsigned long long giftag,
                  float w, float h, float zbias, unsigned int rgba)
{
    const volatile float *cull;
    const volatile float *fogp;
    Vec4 clip;
    Vec4 cs;
    Vec4 ext;
    float clip_w;
    int   fog8;
    char *prim;
    int   page_table;

    /* 1. frustum cull against the camera-0 cull matrix. */
    cull = (const volatile float *)func_001CD370(0);
    clip = vu0_transform_point(cull, world);
    if (vu0_clip_outside(&clip)) {
        return 0x00FFFFFF;
    }

    /* 2. world -> screen, perspective divide xy. */
    fogp = (const volatile float *)(D_00275670 + 0xA0);
    cs = vu0_transform_point(D_70003AC0, world);
    clip_w = cs.w;                       /* saved on the stack by the original */
    cs.x /= cs.w;
    cs.y /= cs.w;

    /* 3. depth bias, z divide, fog term, and the 28.4 conversion. */
    cs.w -= zbias;
    cs.z /= cs.w;
    cs.w = fogp[2] + fogp[3] * cs.w;     /* vmulaz.w / vmaddw.w */
    if (cs.w > fogp[0]) { cs.w = fogp[0]; }   /* vmini.w */
    if (cs.w < 0.0f)    { cs.w = 0.0f; }      /* vmax.w  against vf0.x == 0 */
    SCR_X   = vu0_ftoi4(cs.x);
    SCR_Y   = vu0_ftoi4(cs.y);
    SCR_Z   = vu0_ftoi4(cs.z);
    SCR_FOG = vu0_ftoi4(cs.w);

    /* 4. project the half-extent at the sprite's depth. */
    EXT_X_F = 0.5f * w;
    EXT_Y_F = 0.5f * h;
    EXT_W_F = clip_w;
    {
        float ex = EXT_X_F;
        float ey = EXT_Y_F;
        float ez = EXT_W_F;
        /* row 2's x,y are zeroed (vsub.xy $vf18,$vf18,$vf18): depth only feeds z,w. */
        ext.x = D_70003A40[0] * ex + D_70003A40[4] * ey + D_70003A40[12];
        ext.y = D_70003A40[1] * ex + D_70003A40[5] * ey + D_70003A40[13];
        ext.w = D_70003A40[3] * ex + D_70003A40[7] * ey
              + D_70003A40[11] * ez + D_70003A40[15];
    }
    EXT_X = vu0_ftoi4(ext.x / ext.w);
    EXT_Y = vu0_ftoi4(ext.y / ext.w);

    /* 5. depth-driven colour modulation. */
    if (mode != 0) {
        fog8 = SCR_FOG >> 4;
        if (fog8 >= 0x100) { fog8 = 0xFF; }
        if (fog8 < 0)      { fog8 = 0; }

        if (mode == 1) {
            /* alpha only */
            unsigned int a = (rgba >> 24) & 0xFF;
            rgba = (rgba & 0x00FFFFFFu) | (((a * fog8) >> 8) << 24);
        } else if (mode == 2 || mode == 3 || mode == 4) {
            /* RGB only; the GS fog word is pinned to full. */
            unsigned int r = (rgba >>  0) & 0xFF;
            unsigned int g = (rgba >>  8) & 0xFF;
            unsigned int b = (rgba >> 16) & 0xFF;
            r = (r * fog8) >> 8;
            g = (g * fog8) >> 8;      /* the original issues this one on `mult1` */
            b = (b * fog8) >> 8;
            SCR_FOG = 0xFF0;
            rgba = (rgba & 0xFF000000u) | r | (g << 8) | (b << 16);
        }
    }

    /* 6. allocate and fill the sprite primitive. */
    page_table = (int)(D_0028F700 + (bucket << 15) + 0x4D3EC0);
    prim = func_001CB5F0(page_table, SCR_Z, 6);

    *(unsigned long long *)(prim + 0x00) = giftag;
    *(int *)(prim + 0x10) = (rgba >>  0) & 0xFF;
    *(int *)(prim + 0x14) = (rgba >>  8) & 0xFF;
    *(int *)(prim + 0x18) = (rgba >> 16) & 0xFF;
    *(int *)(prim + 0x1C) = (rgba >> 24) & 0xFF;
    *(int *)(prim + 0x20) = 0;
    *(int *)(prim + 0x24) = 0;
    *(float *)(prim + 0x28) = 1.0f;

    *(int *)(prim + 0x30) = SCR_X + EXT_X;
    *(int *)(prim + 0x34) = SCR_Y + EXT_Y;
    *(int *)(prim + 0x38) = SCR_Z;
    *(int *)(prim + 0x3C) = SCR_FOG;
    *(float *)(prim + 0x40) = 1.0f;
    *(float *)(prim + 0x44) = 1.0f;
    *(float *)(prim + 0x48) = 1.0f;

    *(int *)(prim + 0x50) = SCR_X - EXT_X;
    *(int *)(prim + 0x54) = SCR_Y - EXT_Y;
    *(int *)(prim + 0x58) = SCR_Z;
    *(int *)(prim + 0x5C) = SCR_FOG;

    /* 7. trailing state + blend-mode command words. */
    func_001CB6B0(page_table, SCR_Z, 2, (unsigned long long)(unsigned int)D_00251220);
    func_001CB900(page_table, SCR_Z, mode);

    return SCR_Z;
}
