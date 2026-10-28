// NEARMISS func_001D75E0  (vram 0x001D75E0, 0x49C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.53% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4); matching wave 2026-10-28.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
// Remaining differences:
// the bottom edge lands in a0 in the original and v1 here (the scissor compare
// temporaries swap with it). Fixes so far: scratchpad words are per-word externs
// (// SPAD:, volatile, unsized arrays); left and top load the screen word before
// the float_to_int call; 1.0 stores; packet pointer block as in func_001DB480;
// returns the packet + 0x10 pointer; locals declared bottom, left, right, top.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003600 0x70003604 0x70003608 0x7000360C 0x70003670 0x70003674 0x70003678 0x7000367C 0x70003A38 0x70003A3C
//
// Builds a screen-aligned sprite packet: scales w, h by a reference projection
// (16, 16, 480) through the matrix at render context + 0x2340, projects pos with
// the camera matrix into the 12.4 screen quad (0x70003600), rejects points behind
// the camera or boxes outside the 512x224 window, clips the UVs to the window and
// appends a VIF DIRECT + GIF PACKED sprite (TEX0, RGBAQ, two ST/XYZF2 pairs) to
// the context list idx.

typedef int u128 __attribute__((mode(TI)));

typedef struct Vec4 {
    float x, y, z, w;
} Vec4;

extern char *D_00275670;

extern void func_001026A0(void *dst, void *m, void *v);
extern int  float_to_int(float f);

extern Vec4 D_70003AC0[4];
extern volatile int D_70003600[];
extern volatile int D_70003604[];
extern volatile int D_70003608[];
extern volatile int D_7000360C[];
extern volatile float D_70003670[];
extern volatile float D_70003674[];
extern volatile float D_70003678[];
extern volatile float D_7000367C[];
extern volatile float D_70003A38[];
extern volatile float D_70003A3C[];

char *func_001D75E0(int idx, Vec4 *pos, unsigned long long tex0,
                    unsigned int rgba, float w, float h)
{
    Vec4  ref;
    Vec4  clip;
    int   bottom, left, right, top;
    Vec4 *m;
    volatile int *s;
    Vec4 *c;
    char *pkt;
    char *dl;
    char *ret;

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

    m = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(m));
    s = D_70003600;
    c = &clip;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(pos));
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c));
    asm("vwaitq");
    asm("vmulq.xyz vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(s));

    if (clip.w < 0.0f) {
        return 0;
    }

    left = D_70003600[0];
    left -= float_to_int(w);
    right = D_70003600[0] + float_to_int(w);
    top = D_70003604[0];
    top -= float_to_int(h);
    bottom = D_70003604[0] + float_to_int(h);

    if (right < 0x7000 || left > 0x9000 || bottom < 0x7900 || top > 0x8700) {
        return 0;
    }

    D_70003670[0] = 0.0f;
    D_70003674[0] = 1.0f;
    D_70003678[0] = 1.0f;
    D_7000367C[0] = 0.0f;
    D_70003A38[0] = 2.0f * w;
    D_70003A3C[0] = 2.0f * h;

    if (left < 0x7000) {
        D_70003670[0] += (float)(0x7000 - left) / D_70003A38[0];
        left = 0x7000;
    }
    if (right > 0x9000) {
        D_70003678[0] -= (float)(right - 0x9000) / D_70003A38[0];
        right = 0x9000;
    }
    if (top < 0x7900) {
        D_70003674[0] -= (float)(0x7900 - top) / D_70003A3C[0];
        top = 0x7900;
    }
    if (bottom > 0x8700) {
        D_7000367C[0] += (float)(bottom - 0x8700) / D_70003A3C[0];
        bottom = 0x8700;
    }

    {
        char *ctx = D_00275670 + idx * 4;
        *(char *)(*(char **)(ctx + 0x10) + 3) = 0x10;
        *(int *)(*(char **)(ctx + 0x10) + 4) = 0;
        *(short *)(*(char **)(ctx + 0x10) + 0) = 8;
        pkt = *(char **)(ctx + 0x10);
        dl = pkt + 0x10;
        *(char **)(ctx + 0x10) = pkt + 0x90;
    }
    *(u128 *)(pkt + 0x10) = 0;
    *(int *)(pkt + 0x1C) = 0x50000007;
    ret = dl;
    *(long long *)(pkt + 0x20) = (long long)0x602B4000 << 32 | 0x8001;
    dl += 0x20;
    *(long long *)(pkt + 0x28) = 0x424216;

    *(unsigned long long *)(dl + 0x00) = tex0;
    *(int *)(dl + 0x10) = (rgba >>  0) & 0xFF;
    *(int *)(dl + 0x14) = (rgba >>  8) & 0xFF;
    *(int *)(dl + 0x18) = (rgba >> 16) & 0xFF;
    *(int *)(dl + 0x1C) = (rgba >> 24) & 0xFF;
    *(float *)(dl + 0x20) = D_70003670[0];
    *(float *)(dl + 0x24) = D_70003674[0];
    *(float *)(dl + 0x28) = 1.0f;
    *(int *)(dl + 0x30) = left;
    *(int *)(dl + 0x34) = top;
    *(int *)(dl + 0x38) = D_70003608[0];
    *(int *)(dl + 0x3C) = D_7000360C[0];
    *(float *)(dl + 0x40) = D_70003678[0];
    *(float *)(dl + 0x44) = D_7000367C[0];
    *(float *)(dl + 0x48) = 1.0f;
    *(int *)(dl + 0x50) = right;
    *(int *)(dl + 0x54) = bottom;
    *(int *)(dl + 0x58) = D_70003608[0];
    *(int *)(dl + 0x5C) = D_7000360C[0];
    return ret;
}
