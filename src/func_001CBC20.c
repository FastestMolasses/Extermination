// NEARMISS func_001CBC20  (vram 0x001CBC20, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) (was 62.15%).
// Matching wave 2026-10-27: the four command nodes are one static inline helper (dma_node,
// family spelling: slot = &D_00275670[ch], cursor slot[4], node pointer taken before the
// advance); the colour is a 64-bit local holding the sign-extended 32-bit RGBA; alpha is a
// u8 clamped with a ternary. Same size as the original (0x1F0), no out-of-line helper emitted.
// Remaining differences: register colouring only: the else-arm cursor slot and the RGBA
// value swap t7/t8, the RGBA sign extension is scheduled inside the arg7 arm instead of
// beside the final stores, and the vertex pointer (packet + 0x10) lands in the clamp
// branch's delay slot instead of the last store. Declaration order (8 orders swept) and
// statement order do not change it. 2026-10-28 (wave wX2): the else arm queues its
// D_00250FC0 node before setting the default colour (the original's order).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// GS/DMA sprite-quad packet builder (8 args: a0-a3 + t0-t3). Writes deferred command
// nodes into the per-channel packet ring slot &D_00275670[arg0] (cursor slot[4], 16-byte
// nodes {byte+3=cmd, word+4=list ptr, half+0=count}): node {0x30, D_002511C0, 3}, then
// {0x30, D_00250FC0, 8} on both arms of arg7. If arg7 != 0 the RGBA is
// (arg7[4]<<24)|*(int *)arg7 with alpha arg7[7], else 0x80808080 and alpha 0. Then an
// inline 0x60-byte packet: cmd node {0x10, 0, 5}, qword zero (+0x10), VIF 0x50000004
// (+0x1C), GIFtag 0x64000000_00008001 (+0x20), reglist 0x413413 (+0x28), zero (+0x30),
// rgba sign-extended (+0x38 and +0x50), packed corner vertices:
// (arg1<<4)|(arg2<<20)|0xFFFFFF<<32 (+0x40), (arg5<<4)|(arg6<<20) (+0x48),
// ((arg1+arg3)<<4)|((arg2+(arg4>>1))<<20)|0xFFFFFF<<32 (+0x58). Finally the alpha,
// clamped to at most 0xF, is OR-ed <<16 into the two vertex words at +0x40/+0x58.

typedef unsigned char u8;
typedef unsigned long long u64;
typedef long long s64;
typedef int s128 __attribute__((mode(TI)));

extern char D_002511C0[0x100];
extern char D_00250FC0[0x100];
extern char **D_00275670;


static inline char *dma_node(int ch, int cmd, char *list, int count, int size) {
    char **slot;
    char *p;

    slot = (char **)(&D_00275670[ch]);
    *(u8 *)(slot[4] + 3) = cmd;
    *(char **)(slot[4] + 4) = list;
    *(short *)(slot[4] + 0) = count;
    p = slot[4];
    slot[4] = p + size;
    return p;
}

void func_001CBC20(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, u8 *arg7) {
    char *p;
    char *q;
    int rgba;
    u8 alpha;
    int x;
    s64 v40;
    s64 v48;
    s64 v58;

    dma_node(arg0, 0x30, D_002511C0, 3, 0x10);

    if (arg7 != 0) {
        dma_node(arg0, 0x30, D_00250FC0, 8, 0x10);
        rgba = (arg7[4] << 24) | *(int *)arg7;
        alpha = arg7[7];
    } else {
        dma_node(arg0, 0x30, D_00250FC0, 8, 0x10);
        rgba = 0x80808080;
        alpha = 0;
    }

    v48 = (arg5 << 4) | (arg6 << 20);
    v58 = (s64)(((arg1 + arg3) << 4) | ((arg2 + (arg4 >> 1)) << 20)) | ((u64)0xFFFFFF << 32);
    v40 = (s64)((arg1 << 4) | (arg2 << 20)) | ((u64)0xFFFFFF << 32);

    p = dma_node(arg0, 0x10, 0, 5, 0x60);
    ((s128 *)p)[1] = 0;
    *(int *)(p + 0x1C) = 0x50000004;
    *(u64 *)(p + 0x20) = (u64)0x8001 | ((u64)0x64000000 << 32);
    *(u64 *)(p + 0x28) = 0x413413;
    *(u64 *)(p + 0x30) = 0;
    *(s64 *)(p + 0x38) = rgba;
    *(s64 *)(p + 0x40) = v40;
    *(s64 *)(p + 0x48) = v48;
    *(s64 *)(p + 0x50) = rgba;
    *(s64 *)(p + 0x58) = v58;
    q = p + 0x10;
    alpha = (alpha > 0xF) ? 0xF : alpha;
    x = alpha << 16;
    *(u64 *)(q + 0x30) = *(u64 *)(q + 0x30) | x;
    *(u64 *)(q + 0x48) = *(u64 *)(q + 0x48) | x;
}
