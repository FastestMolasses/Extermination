// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.34% NEARMISS).
// Fixes read from the original instructions:
//  - the block body pointer (cursor + 0x10) is taken into its own local right
//    after the cursor is read, and that local is used both for the quadword
//    clear and as the return value; this alone fixes the buffer-base register
//    and the scheduling of the TEX0 pack against the header stores.
//
// Appends a 0x50-byte GIF/GS register-write block into the ring buffer pointed
// to by D_00275670[arg0][4] (the bump cursor). Header: byte +3 = 0x10, word
// +4 = 0, half +0 = 4. Cursor advanced by 0x50. The block body (base+0x10):
// quadword +0x10 = 0; GIFtag word +0x1C = 0x50000003; +0x20 = the A+D
// GIFtag dword 0x10000000_00008002; +0x28 = 0xE; +0x30 = 0; +0x38 = 0x3F;
// +0x40 = a packed GS TEX0 register value built from six fields:
//   (arg1>>8)            -> bits  0+   (TBP0, sign-extended 32->64)
//   ((1<<arg2)>>6)<<14   -> bits 14+   (TBW)
//   arg2<<26             -> bits 26+   (TW)
//   arg3<<30             -> bits 30+   (TH)
//   t1p<<34              -> bits 34+   (TCC)
//   t0p<<35              -> bits 35+   (TFX)
// +0x48 = 6 (register TEX0_1). Returns the block body pointer (cursor +0x10).
typedef unsigned __int128 u128;
extern char **D_00275670;

char *func_001D6BA0(int arg0, int arg1, int arg2, int arg3, int t0p, int t1p) {
    char **t3;
    char *t1;
    char *body;

    t3 = (char **)(&D_00275670[arg0]);
    *(char *)(t3[4] + 3) = 0x10;
    *(int *)(t3[4] + 4) = 0;
    *(short *)(t3[4] + 0) = 4;
    t1 = t3[4];
    body = t1 + 0x10;
    t3[4] = t1 + 0x50;
    *(u128 *)body = 0;
    *(int *)(t1 + 0x1C) = 0x50000003;
    *(long long *)(t1 + 0x20) = 0x8002LL | ((long long)0x10000000 << 0x20);
    *(long long *)(t1 + 0x28) = 0xE;
    *(long long *)(t1 + 0x30) = 0;
    *(long long *)(t1 + 0x38) = 0x3F;
    *(long long *)(t1 + 0x40) =
        ((long long)(int)t0p << 35)
        | (((long long)(int)t1p << 34)
        | (((long long)(int)arg3 << 30)
        | (((long long)(int)arg2 << 26)
        | ((long long)(int)(arg1 >> 8)
        | ((long long)(int)((1 << arg2) >> 6) << 14)))));
    *(long long *)(t1 + 0x48) = 6;
    return body;
}
