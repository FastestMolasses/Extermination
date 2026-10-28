// NEARMISS func_001C7900  (vram 0x001C7900, 0x2F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906); matching wave 2026-10-28 (inline VU0 asm,
// user-approved 2026-10-28 for the VU0 class). Remaining differences:
// Saved-register rotation: the original keeps the entry cursor in s0, slot_idx in s1,
// vu_addr in s2 and mtx in s3; this C gets mtx in s0 and the others shifted (declaration
// order does not move it), with the knock-on temporaries. Was 20.11%.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
typedef unsigned int u128 __attribute__((mode(TI)));

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; } Mtx4;

extern char *D_00275670;

extern Mtx4 D_70003400;
extern Mtx4 D_70003440;
extern Vec4 D_70003480;
extern Vec4 D_70003490;
extern Vec4 D_700034A0;
extern Vec4 D_700034B0;
extern Mtx4 D_70003AC0;

extern void func_001D88B0(void *pos, void *out_a, void *out_b, int token);

/* VU0: out (4 rows) = a (4 rows) times b, each output row
   a.x * b.r0 + a.y * b.r1 + a.z * b.r2 + a.w * b.r3. */
static inline void vu0_mul_mtx(void *out0, const void *a0, const void *b0)
{
    void *out = out0;
    const void *a = a0;
    const void *b = b0;

    asm("lqc2 vf4, 0x0(%0)" : : "r"(b));
    asm("lqc2 vf5, 0x10(%0)" : : "r"(b));
    asm("lqc2 vf6, 0x20(%0)" : : "r"(b));
    asm("lqc2 vf7, 0x30(%0)" : : "r"(b));
    asm("lqc2 vf8, 0x0(%0)" : : "r"(a));
    asm("vmulax.xyzw ACC, vf4, vf8x");
    asm("vmadday.xyzw ACC, vf5, vf8y");
    asm("vmaddaz.xyzw ACC, vf6, vf8z");
    asm("vmaddw.xyzw vf12, vf7, vf8w");
    asm("sqc2 vf12, 0x0(%0)" : : "r"(out) : "memory");
    asm("lqc2 vf9, 0x10(%0)" : : "r"(a));
    asm("vmulax.xyzw ACC, vf4, vf9x");
    asm("vmadday.xyzw ACC, vf5, vf9y");
    asm("vmaddaz.xyzw ACC, vf6, vf9z");
    asm("vmaddw.xyzw vf13, vf7, vf9w");
    asm("sqc2 vf13, 0x10(%0)" : : "r"(out) : "memory");
    asm("lqc2 vf10, 0x20(%0)" : : "r"(a));
    asm("vmulax.xyzw ACC, vf4, vf10x");
    asm("vmadday.xyzw ACC, vf5, vf10y");
    asm("vmaddaz.xyzw ACC, vf6, vf10z");
    asm("vmaddw.xyzw vf14, vf7, vf10w");
    asm("sqc2 vf14, 0x20(%0)" : : "r"(out) : "memory");
    asm("lqc2 vf11, 0x30(%0)" : : "r"(a));
    asm("vmulax.xyzw ACC, vf4, vf11x");
    asm("vmadday.xyzw ACC, vf5, vf11y");
    asm("vmaddaz.xyzw ACC, vf6, vf11z");
    asm("vmaddw.xyzw vf15, vf7, vf11w");
    asm("sqc2 vf15, 0x30(%0)" : : "r"(out) : "memory");
}

/* VU0: out.xyz = v.xyz / |v.xyz| (square root and divide through Q),
   out.w = 0. */
static inline void vu0_normalize(Vec4 *out0, const Vec4 *v0)
{
    Vec4 *out = out0;
    const Vec4 *v = v0;

    asm("lqc2 vf4, 0x0(%0)" : : "r"(v));
    asm("vmul.xyz vf5, vf4, vf4");
    asm("vaddy.x vf5, vf5, vf5y");
    asm("vaddz.x vf5, vf5, vf5z");
    asm("vsqrt Q, vf5x");
    asm("vwaitq");
    asm("vaddq.x vf5, vf0, Q");
    asm("vdiv Q, vf0w, vf5x");
    asm("vsub.xyzw vf6, vf0, vf0");
    asm("vwaitq");
    asm("vmulq.xyz vf6, vf4, Q");
    asm("sqc2 vf6, 0x0(%0)" : : "r"(out) : "memory");
}

/* Uploads an object's matrices for VU1: DMA chunk 1 (qwc 5) flushes, sets
   STCYCL 1/1 and unpacks the four rows of the scratchpad matrix at
   0x70003440 (built by func_001D88B0 from the object position) to VU
   address vu_addr; chunk 2 (qwc 9) unpacks eight rows to VU address 0:
   mtx times the camera matrix at 0x70003AC0, then the normalized basis
   (rows 0..2 of mtx at unit length, w = 0, plus mtx's translation row,
   staged at 0x70003480) times the scratchpad matrix at 0x70003400.
   Returns the channel cursor as it was on entry. */
void *func_001C7900(const Mtx4 *mtx, int arg1, int vu_addr, int slot_idx)
{
    void *first = *(char **)(D_00275670 + 0x10 + slot_idx * 4);
    char *ch;
    char *p;
    char *q;

    func_001D88B0((void *)&mtx->r[3], &D_70003400, &D_70003440, arg1);

    ch = D_00275670 + slot_idx * 4;
    *(char *)(*(char **)(ch + 0x10) + 3) = 0x10;
    *(int *)(*(char **)(ch + 0x10) + 4) = 0;
    *(short *)(*(char **)(ch + 0x10) + 0) = 5;
    p = *(char **)(ch + 0x10);
    *(char **)(ch + 0x10) = p + 0x60;
    *(u128 *)(p + 0x10) = 0;
    *(int *)(p + 0x14) = 0x11000000;
    *(int *)(p + 0x18) = 0x01000101;
    *(int *)(p + 0x1C) = 0x6C040000 | vu_addr;
    {
        u128 *src = (u128 *)&D_70003440;
        ((u128 *)(p + 0x20))[0] = src[0];
        ((u128 *)(p + 0x20))[1] = src[1];
        ((u128 *)(p + 0x20))[2] = src[2];
        ((u128 *)(p + 0x20))[3] = src[3];
    }

    ch = D_00275670 + slot_idx * 4;
    *(char *)(*(char **)(ch + 0x10) + 3) = 0x10;
    *(int *)(*(char **)(ch + 0x10) + 4) = 0;
    *(short *)(*(char **)(ch + 0x10) + 0) = 9;
    p = *(char **)(ch + 0x10);
    *(char **)(ch + 0x10) = p + 0xA0;
    q = p + 0x10;
    *(u128 *)q = 0;
    *(int *)(q + 0x8) = 0x01000101;
    *(int *)(q + 0xC) = 0x6C080000;
    q += 0x10;

    vu0_mul_mtx(q, mtx, &D_70003AC0);

    vu0_normalize(&D_70003480, &mtx->r[0]);
    vu0_normalize(&D_70003490, &mtx->r[1]);
    vu0_normalize(&D_700034A0, &mtx->r[2]);
    *(u128 *)&D_700034B0 = *(u128 *)&mtx->r[3];

    vu0_mul_mtx(q + 0x40, &D_70003480, &D_70003400);

    return first;
}
