// NEARMISS func_00207E40  (vram 0x00207E40, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 5.42% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef int   __attribute__((mode(TI))) u128;

extern char *D_00275670;        /* per-frame render context */

void func_00207E40(int slot, int x0, int y0, int width, int height,
                   u32 rgba, u64 tex0)
{
    int  *cursor_slot = (int *)(D_00275670 + 4 * slot + 0x10);
    u8   *p           = (u8 *)(u32)*cursor_slot;
    u32   tw          = (u32)(tex0 >> 26) & 0xF;    /* log2 texture width  */
    u32   th          = (u32)(tex0 >> 30) & 0xF;    /* log2 texture height */
    u32   uv_far      = ((1u << tw) << 4) | ((1u << th) << 20);
    u32   xyz1        = (u32)x0 | (u32)((y0 + 8 * height) << 16);
    u32   xyz2        = (u32)(x0 + 16 * width) | (u32)(y0 << 16);

    /* DMAtag: CNT, 7 quadwords follow. */
    p[3]                 = 0x10;
    *(u32 *)(p + 0x04)   = 0;
    *(u16 *)(p + 0x00)   = 7;
    *cursor_slot         = (int)(u32)(p + 0x80);

    /* VIFcodes: three NOPs then DIRECT for 6 quadwords. */
    *(u128 *)(p + 0x10)  = 0;
    *(u32  *)(p + 0x1C)  = 0x50000006;

    /* GIFtag: NLOOP 1, EOP, REGLIST of 10 registers. */
    *(u64 *)(p + 0x20)   = 0xA400000000008001ULL;
    *(u64 *)(p + 0x28)   = 0x0000008413413680ULL;

    *(u64 *)(p + 0x30)   = 0x156;               /* PRIM   */
    *(u64 *)(p + 0x38)   = 5;                   /* TEX1_1 */
    *(u64 *)(p + 0x40)   = tex0;                /* TEX0_1 */

    *(u64 *)(p + 0x48)   = 0;                   /* UV    v1 */
    *(u64 *)(p + 0x50)   = (u64)rgba;           /* RGBAQ v1 */
    *(u64 *)(p + 0x58)   = (u64)(long long)(int)xyz1 | (0x00FFFFFFULL << 32);

    *(u64 *)(p + 0x60)   = (u64)(long long)(int)uv_far;   /* UV    v2 */
    *(u64 *)(p + 0x68)   = (u64)rgba;                     /* RGBAQ v2 */
    *(u64 *)(p + 0x70)   = (u64)(long long)(int)xyz2 | (0x00FFFFFFULL << 32);

    *(u64 *)(p + 0x78)   = 0;                   /* TEX1_1 reset */
}
