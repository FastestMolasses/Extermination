// NEARMISS func_001F15F0  (vram 0x001F15F0, 0x2D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4); matching wave 2026-10-28.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
// Remaining differences:
// in both projections the vertex-source and clip-temp pointers sit in swapped
// temporaries (v0/v1), the same three-operand projection residual as func_001CAAC0.
// Fixes so far: the history entry is a struct (ten positions, then ten fades at
// +0xA0); the loop is top-tested with the first pass skipping the wrap test; the
// pass counter increments right after samp -= 2; -sdatathreshold 4; local order.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Emits a trail of line segments for an actor: texture page from func_001CCF70
// (0xFFFFFF = not resident -> return), a 0x3E-quadword block from func_001CB5F0,
// then for each (node, sample) step back through the history D_00275B40 until it
// wraps to the start: scale the two colours by fade / 10 (func_00102900), convert
// them (func_00102990), project both history positions into XYZF2 words, age the
// fade by 1 (or set the ADC bit in the NEXT block when it is spent, a quirk kept
// as is), then close the block with func_001CB900(D_007635C0, page, 2).

typedef int u128 __attribute__((mode(TI)));

typedef struct { float x, y, z, w; } Vec4;

extern char *D_00275670;
struct hist {
    Vec4  pos[10];
    float fade[10];
};

extern struct hist **D_00275B40;
extern char  D_007635C0[];
extern Vec4  D_70003600;
extern Vec4  D_70003610;
extern Vec4  D_70003AC0[4];

extern int   func_001CCF70(char *pos);
extern char *func_001CB5F0(int pages, int page, int qwords);
extern void  func_001CB900(int pages, int page, int kind);
extern void  func_00102900(float *dst, float *v, float s);
extern void  func_00102990(int *dst, float *v);

void func_001F15F0(char *actor, int node, int samp)
{
    Vec4  clip0;
    Vec4  clip1;
    int   page;
    int   qw;
    int   node0;
    int   samp0;
    int   iter;
    char *blk;
    struct hist *hist;
    char *a;
    char *fog;
    Vec4 *m;
    float scale;
    float fade;
    Vec4 *src;
    char *dst;
    Vec4 *c;

    a = actor + 0x1F0;
    page = func_001CCF70(*(char **)a + 0x30);
    if (page == 0xFFFFFF) {
        return;
    }
    qw = 0x3E;
    node0 = node;
    samp0 = samp;
    fog = D_00275670 + 0xA0;
    asm("lqc2 vf23, 0x0(%0)" : : "r"(fog));
    m = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(m));
    blk = func_001CB5F0((int)D_007635C0, page, qw);
    *(u128 *)blk = 0;
    *(int *)(blk + 0xC) = (qw - 1) | 0x50000000;
    blk += 0x10;
    *(long long *)(blk + 0) = (long long)0x40264000 << 32 | 0x800F;
    *(long long *)(blk + 8) = 0x4141;
    blk += 0x10;

    iter = 0;
    while (1) {
        if (iter != 0 && node0 == node && samp0 == samp) {
            break;
        }
        hist = D_00275B40[node];
        scale = hist->fade[samp] / 10.0f;
        func_00102900((float *)&D_70003600, (float *)(a + 0x30), scale);
        func_00102900((float *)&D_70003610, (float *)(a + 0x40), scale);
        func_00102990((int *)blk, (float *)&D_70003600);
        func_00102990((int *)(blk + 0x20), (float *)&D_70003610);
        src = &hist->pos[samp];
        dst = blk + 0x10;
        c = &clip0;
        asm("lqc2 vf1, 0x0(%0)" : : "r"(src));
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
        asm("sqc2 vf2, 0x0(%0)" : : "r"(dst));
        src = &hist->pos[samp + 1];
        dst = blk + 0x30;
        c = &clip1;
        asm("lqc2 vf1, 0x0(%0)" : : "r"(src));
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
        asm("sqc2 vf2, 0x0(%0)" : : "r"(dst));
        fade = hist->fade[samp];
        blk += 0x40;
        if (fade > 0.0f) {
            hist->fade[samp] = fade - 1.0f;
        } else {
            *(int *)(blk + 0x1C) |= 0x8000;
            *(int *)(blk + 0x2C) |= 0x8000;
        }
        samp -= 2;
        iter++;
        if (samp < 0) {
            samp = 8;
            node -= 1;
            if (node < 0) {
                node = 2;
            }
        }
    }
    func_001CB900((int)D_007635C0, page, 2);
}
