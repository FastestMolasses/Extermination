// NEARMISS func_001DD2F0  (vram 0x001DD2F0, 0x30C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.53% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered: two near-identical GS/DMA quad-strip particle builders. Each copies a template GIFtag+register-list blob (D_002533D0=0x48 bytes, D_00253420=0x38 bytes) onto the stack, acquires a write cursor from the page-table array D_00275670[arg0] (+0x10 field, matching the fun...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef int s128 __attribute__((mode(TI)));

extern int float_to_int(float x);
extern char D_002533D0[0x48];
extern char D_00253420[0x38];
extern char *D_00275670;

void func_001DD2F0(int arg0, int *arg1, u32 arg2) {
    char tbl0[0x48];
    char tbl1[0x38];
    u64 wide;
    char *page;
    char *blk;
    char *src;
    char *dst;
    int n;

    {
        s128 q0 = *(s128 *)&D_002533D0[0x0];
        s128 q1 = *(s128 *)&D_002533D0[0x10];
        s128 q2 = *(s128 *)&D_002533D0[0x20];
        s128 q3 = *(s128 *)&D_002533D0[0x30];
        *(s128 *)&tbl0[0x0] = q0;
        *(s128 *)&tbl0[0x10] = q1;
        *(s128 *)&tbl0[0x20] = q2;
        *(s128 *)&tbl0[0x30] = q3;
        *(s128 *)&tbl0[0x40] = *(s128 *)&D_002533D0[0x40];
    }

    {
        s128 r0 = *(s128 *)&D_00253420[0x0];
        s128 r1 = *(s128 *)&D_00253420[0x10];
        s64 r2 = *(s64 *)&D_00253420[0x20];
        *(s128 *)&tbl1[0x0] = r0;
        *(s128 *)&tbl1[0x10] = r1;
        *(s64 *)&tbl1[0x20] = r2;
    }

    wide = (u32)arg2;

    page = D_00275670 + arg0 * 4;
    blk = *(char **)(page + 0x10);
    *(u8 *)(blk + 3) = 0x10;
    blk = *(char **)(page + 0x10);
    *(int *)(blk + 4) = 0;
    blk = *(char **)(page + 0x10);
    *(short *)(blk + 0) = 8;
    blk = *(char **)(page + 0x10);
    { char *t = blk + 0x10; dst = t + 0x30; }
    *(char **)(page + 0x10) = blk + 0x90;
    *(s128 *)(blk + 0x10) = 0;
    *(int *)(blk + 0x1C) = 0x50000007;
    *(u64 *)(blk + 0x20) = 0x800100000000ULL | 0xC4000000ULL;
    *(u64 *)(blk + 0x28) = 0x4444000000000000ULL | 0x44444410ULL;
    *(u64 *)(blk + 0x30) = 0x144;
    *(u64 *)(blk + 0x38) = wide;

    src = tbl0;
    n = 0;
    do {
        int qx = *(int *)(arg1 + 0) + float_to_int(16.0f * (0.8f * *(float *)(src + 0)));
        *(int *)0x70003620 = qx;
        n += 1;
        {
            int qz = *(int *)(arg1 + 1) + float_to_int(16.0f * (0.5f * *(float *)(src + 4)));
            *(int *)0x70003624 = qz;
        }
        src += 8;
        *(s64 *)dst = (*(int *)0x70003620 | (*(int *)0x70003624 << 16)) | 0xFFFFFF00000000ULL;
        dst += 8;
    } while (n < 0xA);

    page = D_00275670 + arg0 * 4;
    blk = *(char **)(page + 0x10);
    *(u8 *)(blk + 3) = 0x10;
    blk = *(char **)(page + 0x10);
    *(int *)(blk + 4) = 0;
    blk = *(char **)(page + 0x10);
    *(short *)(blk + 0) = 6;
    blk = *(char **)(page + 0x10);
    { char *t = blk + 0x10; dst = t + 0x30; }
    *(char **)(page + 0x10) = blk + 0x70;
    *(s128 *)(blk + 0x10) = 0;
    *(int *)(blk + 0x1C) = 0x50000005;
    *(u64 *)(blk + 0x20) = 0x800100000000ULL | 0x74000000ULL;
    *(u64 *)(blk + 0x28) = 0x4444410ULL;
    *(u64 *)(blk + 0x30) = 0x142;
    *(u64 *)(blk + 0x38) = wide;

    src = tbl1;
    n = 0;
    do {
        int qx = *(int *)(arg1 + 0) + float_to_int(16.0f * (0.8f * *(float *)(src + 0)));
        *(int *)0x70003620 = qx;
        n += 1;
        {
            int qz = *(int *)(arg1 + 1) + float_to_int(16.0f * (0.5f * *(float *)(src + 4)));
            *(int *)0x70003624 = qz;
        }
        src += 8;
        *(s64 *)dst = (*(int *)0x70003620 | (*(int *)0x70003624 << 16)) | 0xFFFFFF00000000ULL;
        dst += 8;
    } while (n < 5);
}
