// NEARMISS func_001DD2F0  (vram 0x001DD2F0, 0x30C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.51% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Remaining differences: register colouring and scheduling only. The original keeps
// arg0 and arg1 live in their argument registers through the template copies (its copy
// temporaries start two registers higher, and the index shift and the arg1 move into a
// saved register are scheduled late, next to the header stores); this C homes both
// parameters at entry. Every opcode, offset and constant otherwise matches. Wave
// 2026-10-27 fixes: the templates are 0x50 / 0x28-byte tables of ten / five float
// pairs (struct copies, not 0x48 / 0x38 blobs); the packet-family `char **` slot
// spelling; separate counter/pointer locals per loop (the original swaps their saved
// registers); arg2 stored directly (no 64-bit local); the scratch Y word read first.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Two GS quad-strip particle builders. Each appends a command node to the packet ring
// D_00275670[arg0] (cursor at slot[4]: byte +3 = 0x10, word +4 = 0, half +0 = count
// 8 / 6), advances the cursor 0x90 / 0x70, writes an inline packet (qword zeroed,
// +0x1C = VIF DIRECT 0x50000007 / 0x50000005, GIFtag +0x20, register list +0x28,
// 0x144 / 0x142 at +0x30, zero-extended arg2 at +0x38), then emits 10 / 5 packed
// screen-XY dwords: X = arg1[0] + float_to_int(16*(0.8*tbl[i].x)),
// Y = arg1[1] + float_to_int(16*(0.5*tbl[i].y)), staged through scratchpad words
// 0x70003620 / 0x70003624 and stored as (X | Y<<16) sign-extended | 0xFFFFFF<<32.
typedef unsigned char u8;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef int s128 __attribute__((mode(TI)));
typedef struct { float x, y; } Pt;
typedef struct { Pt p[10]; } Tbl0;
typedef struct { Pt p[5]; } Tbl1;

extern int float_to_int(float x);
extern Tbl0 D_002533D0;
extern Tbl1 D_00253420;
extern char **D_00275670;

void func_001DD2F0(int arg0, int *arg1, u32 arg2) {
    Tbl0 tbl0;
    Tbl1 tbl1;
    int n;
    u64 *dst;
    Pt *src;
    Pt *src2;
    int n2;
    char **t3;
    char *t1;
    int z;

    tbl0 = D_002533D0;
    tbl1 = D_00253420;

    t3 = (char **)(&D_00275670[arg0]);
    *(char *)(t3[4] + 3) = 0x10;
    *(int *)(t3[4] + 4) = 0;
    *(short *)(t3[4] + 0) = 8;
    t1 = t3[4];
    dst = (u64 *)(t1 + 0x10) + 6;
    t3[4] = t1 + 0x90;
    *(s128 *)(t1 + 0x10) = 0;
    *(int *)(t1 + 0x1C) = 0x50000007;
    *(u64 *)(t1 + 0x20) = (u64)0x8001 | ((u64)0xC4000000 << 32);
    *(u64 *)(t1 + 0x28) = 0x44444410ULL | ((u64)0x4444 << 32);
    *(u64 *)(t1 + 0x30) = 0x144;
    *(u64 *)(t1 + 0x38) = arg2;

    src = tbl0.p;
    for (n = 0; n < 10; n++) {
        *(volatile int *)0x70003620 = arg1[0] + float_to_int(16.0f * (0.8f * src->x));
        *(volatile int *)0x70003624 = arg1[1] + float_to_int(16.0f * (0.5f * src->y));
        src++;
        z = *(volatile int *)0x70003624;
        *dst = (s64)(*(volatile int *)0x70003620 | (z << 16)) | ((u64)0xFFFFFF << 32);
        dst++;
    }

    t3 = (char **)(&D_00275670[arg0]);
    *(char *)(t3[4] + 3) = 0x10;
    *(int *)(t3[4] + 4) = 0;
    *(short *)(t3[4] + 0) = 6;
    t1 = t3[4];
    dst = (u64 *)(t1 + 0x10) + 6;
    t3[4] = t1 + 0x70;
    *(s128 *)(t1 + 0x10) = 0;
    *(int *)(t1 + 0x1C) = 0x50000005;
    *(u64 *)(t1 + 0x20) = (u64)0x8001 | ((u64)0x74000000 << 32);
    *(u64 *)(t1 + 0x28) = 0x4444410ULL;
    *(u64 *)(t1 + 0x30) = 0x142;
    *(u64 *)(t1 + 0x38) = arg2;

    src2 = tbl1.p;
    for (n2 = 0; n2 < 5; n2++) {
        *(volatile int *)0x70003620 = arg1[0] + float_to_int(16.0f * (0.8f * src2->x));
        *(volatile int *)0x70003624 = arg1[1] + float_to_int(16.0f * (0.5f * src2->y));
        src2++;
        z = *(volatile int *)0x70003624;
        *dst = (s64)(*(volatile int *)0x70003620 | (z << 16)) | ((u64)0xFFFFFF << 32);
        dst++;
    }
}
