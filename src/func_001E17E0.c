// NEARMISS func_001E17E0  (vram 0x001E17E0, 0x2EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.29% via mwcc 2.3.3 (mwcps2-2.3.3-000906); matching wave 2026-10-28 (inline VU0 asm,
// user-approved 2026-10-28 for the VU0 class). Remaining differences:
// Register colouring only: the outer and inner loop counters, the channel base and
// the adc flag sit in swapped temporaries (v0/v1, t-registers); the instruction
// stream and scheduling otherwise match. Was 13.59% (scalar emulation of the VU0 code).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;
typedef unsigned int u128 __attribute__((mode(TI)));

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 c0, c1, c2, c3; } Mtx4;

extern char *D_00275670;
extern Vec4  D_70003AC0[4];
extern Mtx4 *func_001CD370(int which);

/* Clip p on VU0 against the matrix rows held in VU0 registers 24..27:
   transform (implicit w = 1), clip x, y, z against w, wait out the flag
   pipeline and return the six clip flags (0 == fully inside). */
static inline int vu0_clip(const Vec4 *p)
{
    int c;

    asm("lqc2 vf1, 0x0(%0)" : : "r"(p));
    asm("vmulax.xyzw ACC, vf24, vf1x");
    asm("vmadday.xyzw ACC, vf25, vf1y");
    asm("vmaddaz.xyzw ACC, vf26, vf1z");
    asm("vmaddw.xyzw vf2, vf27, vf0w");
    asm("vclipw.xyz vf2, vf2w");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("cfc2.ni %0, vi18" : "=r"(c));
    return c & 0x3F;
}

/* Project p on VU0 with the camera rows in VU0 registers 28..31 and the fog
   ramp in VU0 register 23 (live-in, loaded by the caller's caller): clip
   position (also parked in a stack quadword the original never reads back),
   Q = 1 / w, xyz scaled by Q, fog = clamp(fog.z + fog.w * w, 0, fog.x) in
   the w lane, all four lanes to 12.4 fixed point, stored as one XYZF2
   quadword at out. */
static inline void vu0_project(const Vec4 *p, void *out)
{
    Vec4 clip;
    Vec4 *c = &clip;
    void *o = out;

    asm("lqc2 vf1, 0x0(%0)" : : "r"(p));
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c) : "memory");
    asm("vwaitq");
    asm("vmulq.xyz vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(o) : "memory");
}

/* Builds the water-grid strip packets for display-list channel chan: 16
   rows of 16 column pairs on a 93.75-unit grid at height 10. Each row is
   a DMA CNT tag (34 qwords), a VIF DIRECT, a GIFtag (triangle strip,
   XYZF2 x 2) and 32 projected vertices; a vertex whose own clip flags, or
   the previous one's, are set gets the ADC bit (no-draw). Returns the
   channel cursor as it was on entry. */
char *func_001E17E0(int chan)
{
    char *first = *(char **)(D_00275670 + 0x10 + chan * 4);
    const Mtx4 *clip_mtx = func_001CD370(2);
    const Vec4 *cam;
    int i, j;
    float y;

    asm("lqc2 vf24, 0x0(%0)" : : "r"(clip_mtx));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(clip_mtx));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(clip_mtx));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(clip_mtx));
    cam = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(cam));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(cam));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(cam));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(cam));
    y = 10.0f;

    for (i = -8; i < 8; i++) {
        float z0 = 93.75f * (float)i - 46.875f;
        float z1 = 93.75f * (float)(i + 1) - 46.875f;
        char *ch;
        char *pkt;
        char *q;
        int   adc;

        /* DMA tag (qwc 34, CNT), then VIF NOPs + DIRECT 33 and the GIFtag */
        ch = D_00275670 + chan * 4;
        *(char *)(*(char **)(ch + 0x10) + 3) = 0x10;
        *(int *)(*(char **)(ch + 0x10) + 4) = 0;
        *(short *)(*(char **)(ch + 0x10) + 0) = 0x22;
        pkt = *(char **)(ch + 0x10);
        q = pkt + 0x10;
        *(char **)(ch + 0x10) = pkt + 0x230;
        *(u128 *)q = 0;
        *(int *)(q + 0xC) = 0x50000021;
        *(long long *)(q + 0x10) = (long long)0x8010 | ((long long)0x20024000 << 32);
        *(long long *)(q + 0x18) = 0x44;
        q += 0x20;
        adc = 0;

        for (j = -8; j < 8; j++) {
            float xj = 93.75f * (float)j - 46.875f;
            Vec4  p;

            adc >>= 1;
            p.x = xj; p.y = y; p.z = z0; p.w = 1.0f;
            if (vu0_clip(&p) != 0) {
                adc |= 4;
            }
            vu0_project(&p, q);
            if (adc != 0) {
                *(int *)(q + 0xC) = 0xFFFF;
            }

            p.x = xj; p.y = y; p.z = z1; p.w = 1.0f;
            adc >>= 1;
            if (vu0_clip(&p) != 0) {
                adc |= 4;
            }
            vu0_project(&p, q + 0x10);
            if (adc != 0) {
                *(int *)(q + 0x1C) = 0xFFFF;
            }

            q += 0x20;
        }
    }

    return first;
}
