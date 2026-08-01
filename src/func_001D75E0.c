// NEARMISS func_001D75E0  (vram 0x001D75E0, 0x49C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 56.26% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned __int128 uint128;

extern char *D_00275670;

extern void func_001026A0(void *dst, void *m, void *v);   /* dst = v * m (4x4) */
extern int  float_to_int(float f);

extern volatile float D_70003AC0[16];    /* world -> screen matrix (scratchpad) */

/* ---- EE scratchpad staging (0x70000000 is the SPR) ------------------------ */
/* Projected centre, 28.4 fixed point: x, y, z, fog. */
#define SCR       ((volatile int *)0x70003600)
/* Texture coordinates: (u0, v0) top-left, (u1, v1) bottom-right. */
#define TEX_U0    (*(volatile float *)0x70003670)
#define TEX_V0    (*(volatile float *)0x70003674)
#define TEX_U1    (*(volatile float *)0x70003678)
#define TEX_V1    (*(volatile float *)0x7000367C)
/* Full sprite extent in 28.4 units, used as the UV-clip divisor. */
#define FULL_W    (*(volatile float *)0x70003A38)
#define FULL_H    (*(volatile float *)0x70003A3C)

/* GS window in 2048.0-centred 28.4 coordinates: 512 x 224 pixels. */
#define WIN_LEFT   0x7000
#define WIN_RIGHT  0x9000
#define WIN_TOP    0x7900
#define WIN_BOTTOM 0x8700

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

/* vftoi4: float -> 28.4 fixed point, truncating toward zero. */
static int vu0_ftoi4(float f)
{
    return (int)(f * 16.0f);
}

/* NOT A REAL SYMBOL. This stands for the live contents of VU0 register vf23,
   which this function reads without ever loading it: the fog parameter quadword
   from D_00275670 + 0xA0, left behind by whichever of func_001CD520 /
   func_001CD940 ran last. A port must plumb those parameters in explicitly —
   there is no global at this name to link against. */
extern volatile float VU0_VF23[4];

void *func_001D75E0(int idx, Vec4 *pos, unsigned long long tex0,
                    unsigned int rgba, float w, float h)
{
    Vec4  ref;
    Vec4  cs;
    float clip_w;
    int   left, right, top, bottom;
    char *cursor;
    char *blk;

    /* 1. reference projection -> caller units to 28.4 screen units. */
    ref.x = 16.0f;
    ref.y = 16.0f;
    ref.z = 480.0f;
    ref.w = 1.0f;
    func_001026A0(&ref, D_00275670 + 0x2340, &ref);
    ref.x = ref.x / ref.w;
    ref.y = ref.y / ref.w;
    ref.x = ref.x - 2048.0f;
    ref.y = ref.y - 2048.0f;
    ref.x = 0.8f * ref.x;
    ref.y = 0.5f * ref.y;
    w = w * ref.x;
    h = h * ref.y;

    /* 2. project the sprite's world position (fog params live in vf23). */
    cs = vu0_transform_point(D_70003AC0, pos);
    clip_w = cs.w;
    cs.x /= cs.w;
    cs.y /= cs.w;
    cs.z /= cs.w;
    cs.w = VU0_VF23[2] + VU0_VF23[3] * cs.w;
    if (cs.w > VU0_VF23[0]) { cs.w = VU0_VF23[0]; }
    if (cs.w < 0.0f)        { cs.w = 0.0f; }
    SCR[0] = vu0_ftoi4(cs.x);
    SCR[1] = vu0_ftoi4(cs.y);
    SCR[2] = vu0_ftoi4(cs.z);
    SCR[3] = vu0_ftoi4(cs.w);

    /* 3. behind the camera? */
    if (clip_w < 0.0f) {
        return 0;
    }

    /* 4. the four screen edges. */
    left   = SCR[0] - float_to_int(w);
    right  = SCR[0] + float_to_int(w);
    top    = SCR[1] - float_to_int(h);
    bottom = SCR[1] + float_to_int(h);

    /* 5. scissor test. */
    if (right < WIN_LEFT)    { return 0; }
    if (left  > WIN_RIGHT)   { return 0; }
    if (bottom < WIN_TOP)    { return 0; }
    if (top   > WIN_BOTTOM)  { return 0; }

    /* 6. texture coordinates, adjusted for whatever the window clips off. */
    TEX_U0 = 0.0f;
    TEX_V0 = 1.0f;
    TEX_U1 = 1.0f;
    TEX_V1 = 0.0f;
    FULL_W = 2.0f * w;
    FULL_H = 2.0f * h;

    if (left < WIN_LEFT) {
        TEX_U0 += (float)(WIN_LEFT - left) / FULL_W;
        left = WIN_LEFT;
    }
    if (right > WIN_RIGHT) {
        TEX_U1 -= (float)(right - WIN_RIGHT) / FULL_W;
        right = WIN_RIGHT;
    }
    if (top < WIN_TOP) {
        TEX_V0 -= (float)(WIN_TOP - top) / FULL_H;
        top = WIN_TOP;
    }
    if (bottom > WIN_BOTTOM) {
        TEX_V1 += (float)(bottom - WIN_BOTTOM) / FULL_H;
        bottom = WIN_BOTTOM;
    }

    /* 7. append the VIF DIRECT + GIF PACKED sprite packet. */
    cursor = *(char **)(D_00275670 + 0x10 + idx * 4);

    *(char  *)(cursor + 3) = 0x10;              /* DMA CNT tag, QWC 8 ... */
    *(int   *)(cursor + 4) = 0;
    *(short *)(cursor + 0) = 8;                 /* ... written the house way */
    *(char **)(D_00275670 + 0x10 + idx * 4) = cursor + 0x90;

    blk = cursor + 0x10;
    *(uint128 *)(blk + 0x00) = 0;
    *(int *)(blk + 0x0C) = 0x50000007;                        /* VIF DIRECT, 7 qw */
    *(unsigned long long *)(blk + 0x10) = 0x602B400000008001ULL;  /* GIF tag */
    *(unsigned long long *)(blk + 0x18) = 0x424216ULL;            /* GIF REGS */

    *(unsigned long long *)(blk + 0x20) = tex0;               /* TEX0_1 */

    *(int *)(blk + 0x30) = (rgba >>  0) & 0xFF;               /* RGBAQ */
    *(int *)(blk + 0x34) = (rgba >>  8) & 0xFF;
    *(int *)(blk + 0x38) = (rgba >> 16) & 0xFF;
    *(int *)(blk + 0x3C) = (rgba >> 24) & 0xFF;

    *(float *)(blk + 0x40) = TEX_U0;                          /* ST (top-left) */
    *(float *)(blk + 0x44) = TEX_V0;
    *(float *)(blk + 0x48) = 1.0f;

    *(int *)(blk + 0x50) = left;                              /* XYZF2 (top-left) */
    *(int *)(blk + 0x54) = top;
    *(int *)(blk + 0x58) = SCR[2];
    *(int *)(blk + 0x5C) = SCR[3];

    *(float *)(blk + 0x60) = TEX_U1;                          /* ST (bottom-right) */
    *(float *)(blk + 0x64) = TEX_V1;
    *(float *)(blk + 0x68) = 1.0f;

    *(int *)(blk + 0x70) = right;                             /* XYZF2 (bot-right) */
    *(int *)(blk + 0x74) = bottom;
    *(int *)(blk + 0x78) = SCR[2];
    *(int *)(blk + 0x7C) = SCR[3];

    return blk;
}
