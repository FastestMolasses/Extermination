// NEARMISS func_001DD600  (vram 0x001DD600, 0x1A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.52% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Remaining differences: register colouring and scheduling only (same class as the
// sibling func_001DD2F0). The original keeps arg0 and arg1 live in their argument
// registers through the template copy (the index shift and the arg1 move into a saved
// register are scheduled after the first three quadword loads), so every temporary in
// the function sits two registers higher; this C homes both parameters at entry.
// Insensitive to statement order, declaration order, an inline header helper and
// -O3/-O4,s; a 9-minute 2.3.3 permuter run found only non-C junk. Every opcode,
// offset and constant otherwise matches. Wave 2026-10-27 fixes: the template is an
// 8-entry float-pair table copied as a struct; the packet-family `char **` slot
// spelling (cursor slot[4]); arg2 stored directly; the loop is a do-while with the
// counter cleared up front (a for loop drops to 51%); scratch Y word read before X.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// GS/DMA packet builder: copies the 8-entry float-pair table D_00253450 (0x40 bytes)
// onto the stack, appends a command node to the packet ring D_00275670[arg0] (cursor
// at slot[4]: byte +3 = 0x10, word +4 = 0, half +0 = count 7), advances the cursor
// 0x80, writes an inline packet (qword zeroed, +0x1C = VIF DIRECT 0x50000006, GIFtag
// 0xA4000000_00008001 at +0x20, register list 0x44_44444410 at +0x28, 0x144 at +0x30,
// zero-extended arg2 at +0x38), then emits 8 packed screen-XY dwords at +0x40:
// X = arg1[0] + float_to_int(16*(0.8*tbl[i].x)), Y = arg1[1] + float_to_int(16*(0.5*tbl[i].y)),
// staged through scratchpad words 0x70003620 / 0x70003624 and stored as
// (X | Y<<16) sign-extended | 0xFFFFFF<<32.
typedef unsigned char u8;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef int s128 __attribute__((mode(TI)));
typedef struct { float x, y; } Pt;
typedef struct { Pt p[8]; } Tbl;

extern int float_to_int(float x);
extern Tbl D_00253450;
extern char **D_00275670;

void func_001DD600(int arg0, int *arg1, u32 arg2) {
    Tbl tbl;
    int n;
    u64 *dst;
    Pt *src;
    char **t3;
    char *t1;
    int z;

    tbl = D_00253450;
    src = tbl.p;
    t3 = (char **)(&D_00275670[arg0]);
    n = 0;

    *(char *)(t3[4] + 3) = 0x10;
    *(int *)(t3[4] + 4) = 0;
    *(short *)(t3[4] + 0) = 7;
    t1 = t3[4];
    dst = (u64 *)(t1 + 0x10) + 6;
    t3[4] = t1 + 0x80;
    *(s128 *)(t1 + 0x10) = 0;
    *(int *)(t1 + 0x1C) = 0x50000006;
    *(u64 *)(t1 + 0x20) = (u64)0x8001 | ((u64)0xA4000000 << 32);
    *(u64 *)(t1 + 0x28) = 0x44444410ULL | ((u64)0x44 << 32);
    *(u64 *)(t1 + 0x30) = 0x144;
    *(u64 *)(t1 + 0x38) = arg2;

    do {
        *(volatile int *)0x70003620 = arg1[0] + float_to_int(16.0f * (0.8f * src->x));
        *(volatile int *)0x70003624 = arg1[1] + float_to_int(16.0f * (0.5f * src->y));
        src++;
        n++;
        z = *(volatile int *)0x70003624;
        *dst = (s64)(*(volatile int *)0x70003620 | (z << 16)) | ((u64)0xFFFFFF << 32);
        dst++;
    } while (n < 8);
}
