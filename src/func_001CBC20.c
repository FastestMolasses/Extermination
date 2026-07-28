// NEARMISS func_001CBC20  (vram 0x001CBC20, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 62.15% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// temp-register-coloring + list-scheduler slot permutation (same class as committed NEARMISS siblings func_001CC3B0/func_001CC8A0 in this GS-packet family). Body/structure fully recovered and several target tells reproduced exactly: andi+signed-slti alpha clamp (alpha as int with in-place 'alpha &=...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// GS/DMA sprite-quad packet builder (8 args: a0-a3 + t0-t3). Writes deferred command
// nodes into the per-channel packet ring D_00275670[arg0] (+0x10 cursor, 16-byte nodes
// {byte+3=cmd, word+4=list ptr, half+0=count}): node {0x30, D_002511C0, 3}, then
// {0x30, D_00250FC0, 8} on both arms of arg7. If arg7 != 0 the RGBA is
// (arg7[4]<<24)|*(int*)arg7 with alpha arg7[7], else 0x80808080/0. Then an inline
// 0x60-byte packet: cmd node {0x10, 0, 5}, qword zero, VIF 0x50000004 (+0x1C), GIFtag
// 0x64000000_00008001 (+0x20), reglist 0x413413 (+0x28), rgba sign-extended (+0x38 and
// +0x50), packed corner vertices in 12.4/color format: (arg1<<4)|(arg2<<20)|0xFFFFFF<<32
// (+0x40), (arg5<<4)|(arg6<<20) (+0x48), ((arg1+arg3)<<4)|((arg2+(arg4>>1))<<20)|
// 0xFFFFFF<<32 (+0x58). Finally the alpha (masked to 8 bits, clamped to max 0xF) is
// OR-ed <<16 into the two vertex words at +0x30/+0x48 of the vertex block (blk+0x10).
typedef unsigned char u8;
typedef unsigned long long u64;
typedef int s128 __attribute__((mode(TI)));

extern char D_002511C0[0x100];
extern char D_00250FC0[0x100];
extern char *D_00275670;

void func_001CBC20(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, char *arg7) {
    char *page;
    char *blk;
    char *q;
    int rgba;
    int alpha;
    int x;
    u64 v40;
    u64 v48;
    u64 v58;

    page = D_00275670 + arg0 * 4;
    blk = *(char **)(page + 0x10);
    *(u8 *)(blk + 3) = 0x30;
    blk = *(char **)(page + 0x10);
    *(char **)(blk + 4) = D_002511C0;
    blk = *(char **)(page + 0x10);
    *(short *)(blk + 0) = 3;
    blk = *(char **)(page + 0x10);
    *(char **)(page + 0x10) = blk + 0x10;

    if (arg7 != 0) {
        page = D_00275670 + arg0 * 4;
        blk = *(char **)(page + 0x10);
        *(u8 *)(blk + 3) = 0x30;
        blk = *(char **)(page + 0x10);
        *(char **)(blk + 4) = D_00250FC0;
        blk = *(char **)(page + 0x10);
        *(short *)(blk + 0) = 8;
        blk = *(char **)(page + 0x10);
        *(char **)(page + 0x10) = blk + 0x10;
        rgba = (*(u8 *)(arg7 + 4) << 24) | *(int *)arg7;
        alpha = *(u8 *)(arg7 + 7);
    } else {
        rgba = 0x80808080;
        alpha = 0;
        page = D_00275670 + arg0 * 4;
        blk = *(char **)(page + 0x10);
        *(u8 *)(blk + 3) = 0x30;
        blk = *(char **)(page + 0x10);
        *(char **)(blk + 4) = D_00250FC0;
        blk = *(char **)(page + 0x10);
        *(short *)(blk + 0) = 8;
        blk = *(char **)(page + 0x10);
        *(char **)(page + 0x10) = blk + 0x10;
    }

    v48 = (u64)((arg5 << 4) | (arg6 << 20));
    v58 = (u64)(((arg1 + arg3) << 4) | ((arg2 + (arg4 >> 1)) << 20)) | ((u64)0xFFFFFF << 32);
    v40 = (u64)((arg1 << 4) | (arg2 << 20)) | ((u64)0xFFFFFF << 32);

    page = D_00275670 + arg0 * 4;
    blk = *(char **)(page + 0x10);
    *(u8 *)(blk + 3) = 0x10;
    blk = *(char **)(page + 0x10);
    *(int *)(blk + 4) = 0;
    blk = *(char **)(page + 0x10);
    *(short *)(blk + 0) = 5;
    blk = *(char **)(page + 0x10);
    *(char **)(page + 0x10) = blk + 0x60;
    ((s128 *)blk)[1] = 0;
    *(int *)(blk + 0x1C) = 0x50000004;
    *(u64 *)(blk + 0x20) = (u64)0x8001 | ((u64)0x64000000 << 32);
    *(u64 *)(blk + 0x28) = 0x413413;
    *(u64 *)(blk + 0x30) = 0;
    *(u64 *)(blk + 0x38) = (u64)rgba;
    *(u64 *)(blk + 0x40) = v40;
    *(u64 *)(blk + 0x48) = v48;
    *(u64 *)(blk + 0x50) = (u64)rgba;
    *(u64 *)(blk + 0x58) = v58;
    q = blk + 0x10;
    alpha = alpha & 0xFF;
    if (alpha >= 0x10) {
        alpha = 0xF;
    }
    x = (alpha & 0xFF) << 16;
    *(u64 *)(q + 0x30) = *(u64 *)(q + 0x30) | x;
    *(u64 *)(q + 0x48) = *(u64 *)(q + 0x48) | x;
}
