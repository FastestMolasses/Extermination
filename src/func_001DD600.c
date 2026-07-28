// NEARMISS func_001DD600  (vram 0x001DD600, 0x1A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 51.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// temp-register-coloring + list-scheduler slot permutation (sibling class of committed NEARMISS func_001DD2F0, 69.53%). Body/structure fully recovered — every opcode/offset/shape now matches the target: single-base grouped lq/sq struct copy of D_00253450, per-store cursor reloads, the two-addiu dst...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// GS/DMA packet builder: copies the 8-entry float-pair table D_00253450 (0x40 bytes,
// single struct lq/sq copy) onto the stack, then acquires the write cursor of the
// per-channel packet ring D_00275670[arg0] (+0x10 field), writes a 16-byte command
// node {byte+3=cmd 0x10, word+4=0, half+0=count 7} and advances the cursor 0x80 for an
// inline packet: qword 0 zeroed, +0x1C=VIF 0x50000006 (DIRECT), GIFtag
// 0xA4000000_00008001 (+0x20), reglist 0x44_44444410 (+0x28), 0x144 (+0x30),
// zero-extended arg2 (+0x38). Then emits 8 packed screen-XY vertex dwords at +0x40:
// X = arg1[0] + float_to_int(16*(0.8*tbl[i].x)), Y = arg1[1] + float_to_int(16*(0.5*tbl[i].y)),
// staged through scratchpad words 0x70003620/0x70003624, stored as
// (X | Y<<16) sign-extended | 0xFFFFFF<<32.
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int s128 __attribute__((mode(TI)));
typedef struct { s128 q0, q1, q2, q3; } Tbl;

extern int float_to_int(float x);
extern Tbl D_00253450;
extern char *D_00275670;

void func_001DD600(int arg0, int *arg1, u32 arg2) {
    Tbl tbl;
    u64 wide;
    char *page;
    char *blk;
    int n;
    char *dst;
    float *src;

    tbl = D_00253450;
    src = (float *)&tbl;

    wide = (u32)arg2;

    page = D_00275670 + arg0 * 4;
    blk = *(char **)(page + 0x10);
    *(u8 *)(blk + 3) = 0x10;
    blk = *(char **)(page + 0x10);
    *(int *)(blk + 4) = 0;
    blk = *(char **)(page + 0x10);
    *(short *)(blk + 0) = 7;
    blk = *(char **)(page + 0x10);
    dst = (char *)((u64 *)(blk + 0x10) + 6);
    *(char **)(page + 0x10) = blk + 0x80;
    ((s128 *)blk)[1] = 0;
    *(int *)(blk + 0x1C) = 0x50000006;
    *(u64 *)(blk + 0x20) = (u64)0x8001 | ((u64)0xA4000000 << 32);
    *(u64 *)(blk + 0x28) = (u64)0x44444410 | ((u64)0x44 << 32);
    *(u64 *)(blk + 0x30) = 0x144;
    *(u64 *)(blk + 0x38) = wide;

    n = 0;
    do {
        *(int *)0x70003620 = arg1[0] + float_to_int(16.0f * (0.8f * src[0]));
        *(int *)0x70003624 = arg1[1] + float_to_int(16.0f * (0.5f * src[1]));
        n += 1;
        *(u64 *)dst = (u64)(*(int *)0x70003620 | (*(int *)0x70003624 << 16)) | ((u64)0xFFFFFF << 32);
        src += 2;
        dst += 8;
    } while (n < 8);
}
