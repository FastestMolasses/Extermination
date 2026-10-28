// NEARMISS func_001CE860  (vram 0x001CE860, 0x678 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (was 90.67%; matching wave 2026-10-28).
// Remaining differences: the store of the width scale to the scratchpad word 0x7000368C
// is followed in the original by a float register copy before the two multiplies (a
// second variable holding the same value); one join of the "i != 0" block carries a
// label nop the candidate lacks; the 64-bit tex word is stored one slot earlier; and the
// second projection's address bases (screen slot, point, raw temp) are coloured
// differently.
// Inline VU0 asm (user-approved 2026-10-28 for the VU0 class, incl. the float-to-GPR
// move before a VU0 transfer), one instruction per statement; mwcc inserts the hazard
// nop after the move itself (no explicit nop is written).
// Fixes found: the clip test, the camera projection (xy / w, w - 3, z / (w - 3), fog clamp,
// 12.4 conversion) are inline VU0 code; 3.0f is a float moved to t0 for the VU0 transfer;
// the screen matrix copy is four 128-bit row copies through pointers (source declared
// first); the normal scratch at 0x70003630 is a float / int union; the width-scale word
// 0x7000368C is an unsized extern (not gp-relative); the packet record cursor starts at
// the packet and then steps past the header; the clip test reads the point directly.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// The textured ribbon (strip) packet of the cable / trail nodes (see src/func_001CE860.c).

typedef struct { float x, y, z, w; } Vec4;
typedef struct { unsigned int x, y, z, w; } UVec4;
typedef int u128 __attribute__((mode(TI)));

extern Vec4 *func_001CD370(int n);
extern int func_001CCF70(Vec4 *points);
extern char *func_001CB5F0(char *chain, int id, int qwc);
extern void func_001CB900(char *chain, int id, int mode);
extern void func_001026A0(float *dst, Vec4 *m, float *v);
extern float func_0011E748(float);
extern int float_to_int(float);
extern char *D_00275670;
extern char D_0028F700[];

extern Vec4 D_70003AC0[4];
typedef struct { Vec4 r[4]; } Mtx;
extern Mtx D_70003A40;
extern Mtx D_70003400;
extern union { float f[4]; int i[4]; } D_70003630;
extern float D_7000368C[];

void func_001CE860(int chain_no, int mode, Vec4 *points, float *colour, int n,
                   unsigned long long tex, float width)
{
    int i;
    int side;
    char *rec;
    char *prev;
    int adc;
    int key;
    int qwc;
    UVec4 scr[2];
    Vec4 raw;
    float inv_w[2];
    Vec4 *m;
    char *fog;
    Vec4 *p;
    UVec4 *sp;
    Vec4 *rp;
    float t;
    float k;
    float three;
    int flags;
    char *pkt;
    u128 *src;
    u128 *dst;

    m = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
    side = 0;
    t = 0.0f;
    qwc = n * 5 + 3;
    fog = D_00275670 + 0xA0;
    asm("lqc2 vf23, 0x0(%0)" : : "r"(fog));
    m = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(m));
    dst = (u128 *)&D_70003400;
    src = (u128 *)&D_70003A40;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    D_70003400.r[3].x = 0.0f;
    D_70003400.r[3].y = 0.0f;
    D_70003400.r[3].z = 0.0f;
    D_70003400.r[3].w = 1.0f;
    key = func_001CCF70(points);

    pkt = func_001CB5F0(D_0028F700 + (chain_no << 15) + 0x4D3EC0, key, qwc + 1);
    *(u128 *)pkt = 0;
    *(unsigned int *)(pkt + 0xC) = qwc | 0x50000000;
    ((unsigned long long *)pkt)[2] = 0x1000000000000001ULL;
    ((unsigned long long *)pkt)[3] = 0xE;
    ((unsigned long long *)pkt)[4] = tex;
    ((unsigned long long *)pkt)[5] = 6;
    ((unsigned long long *)pkt)[6] = (long long)n | 0x502E400000008000ULL;
    ((unsigned long long *)pkt)[7] = 0x42421;
    rec = pkt;
    rec += 0x40;

    for (i = 0; i < n; i++) {
        asm("lqc2 vf1, 0x0(%0)" : : "r"(&points[i]));
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
        asm("cfc2.ni %0, vi18" : "=r"(flags));
        if (flags & 0x3F) {
            adc = 0x8000;
        } else {
            adc = 0;
        }

        sp = &scr[side];
        p = &points[i];
        three = 3.0f;
        asm("lqc2 vf1, 0x0(%0)" : : "r"(p));
        asm("vmulax.xyzw ACC, vf28, vf1x");
        asm("vmadday.xyzw ACC, vf29, vf1y");
        asm("vmaddaz.xyzw ACC, vf30, vf1z");
        asm("vmaddw.xyzw vf2, vf31, vf0w");
        asm("vdiv Q, vf0w, vf2w");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(&raw));
        asm("vwaitq");
        asm("vmulq.xy vf2, vf2, Q");
        asm("mfc1 $8, %0" : : "f"(three) : "$8");
        asm("qmtc2.ni $8, vf3");
        asm("vsubx.w vf2, vf2, vf3x");
        asm("vdiv Q, vf0w, vf2w");
        asm("vwaitq");
        asm("vmulq.z vf2, vf2, Q");
        asm("vmulaz.w ACC, vf0, vf23z");
        asm("vmaddw.w vf2, vf23, vf2w");
        asm("vminix.w vf2, vf2, vf23x");
        asm("vmaxx.w vf2, vf2, vf0x");
        asm("vftoi4.xyzw vf2, vf2");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(sp));
        inv_w[side] = 1.0f / raw.w;

        ((int *)rec)[0] = float_to_int(colour[0]);
        ((int *)rec)[1] = float_to_int(colour[1]);
        ((int *)rec)[2] = float_to_int(colour[2]);
        ((int *)rec)[3] = float_to_int(colour[3]);
        ((float *)rec)[4] = t;
        ((float *)rec)[5] = 0.0f;
        ((float *)rec)[6] = 1.0f;
        ((unsigned int *)rec)[8] = scr[side].x;
        ((unsigned int *)rec)[9] = scr[side].y;
        ((unsigned int *)rec)[10] = scr[side].z;
        ((unsigned int *)rec)[11] = adc | scr[side].w;
        ((float *)rec)[12] = t;
        ((float *)rec)[13] = 1.0f;
        ((float *)rec)[14] = 1.0f;
        ((unsigned int *)rec)[16] = scr[side].x;
        ((unsigned int *)rec)[17] = scr[side].y;
        ((unsigned int *)rec)[18] = scr[side].z;
        ((unsigned int *)rec)[19] = adc | scr[side].w;

        if (i != 0) {
            D_70003630.f[1] = (float)scr[side].x - (float)scr[1 - side].x;
            D_70003630.f[0] = -((float)scr[side].y - (float)scr[1 - side].y);
            k = width * (1.0f / func_0011E748(D_70003630.f[0] * D_70003630.f[0]
                                               + D_70003630.f[1] * D_70003630.f[1]));
            D_7000368C[0] = k;
            D_70003630.f[0] *= k;
            D_70003630.f[1] *= k;
            D_70003630.f[2] = inv_w[side];
            D_70003630.f[3] = 1.0f;
            func_001026A0(D_70003630.f, D_70003400.r, D_70003630.f);
            D_70003630.i[0] = float_to_int(16.0f * D_70003630.f[0] * inv_w[side]);
            D_70003630.i[1] = float_to_int(16.0f * D_70003630.f[1] * inv_w[side]);
            ((int *)prev)[8] += D_70003630.i[0];
            ((int *)prev)[9] += D_70003630.i[1];
            ((int *)prev)[16] -= D_70003630.i[0];
            ((int *)prev)[17] -= D_70003630.i[1];
            if (i == n - 1) {
                ((int *)rec)[8] += D_70003630.i[0];
                ((int *)rec)[9] += D_70003630.i[1];
                ((int *)rec)[16] -= D_70003630.i[0];
                ((int *)rec)[17] -= D_70003630.i[1];
            }
        }
        side = 1 - side;
        prev = rec;
        t = 1.0f - t;
        rec += 0x50;
    }
    func_001CB900(D_0028F700 + (chain_no << 15) + 0x4D3EC0, key, mode);
}
