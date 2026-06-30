// NEARMISS func_001D6BA0  (vram 0x001D6BA0, 0xEC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.34% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation cascade (buffer base $t0 vs $a3) + scheduling order of the +0x4 header store vs the 0x40 GS-register pack. 6-arg 64-bit GS register packing fully recovered (sq zero via __int128, dsll32/dsra32 shift fields, OR grouping all match). Coloring/scheduling permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS: 90.34% (mwcc 2.3.3); 19.34% (991202). Body/logic fully recovered;
// sole residual is a register-allocation cascade (target keeps the buffer base
// in $t0, mwcc colors it into $a3) plus scheduling order of the +0x4 header
// store relative to the 0x40 GS-register pack -- a coloring/scheduling
// permutation, not a clean-store nop. Permuter territory.
//
// Appends a 0x50-byte GIF/GS register-write block into the ring buffer pointed
// to by D_00275670[arg0][4] (the bump cursor). Header: byte +3 = 0x10, word
// +4 = 0, half +0 = 4. Cursor advanced by 0x50. The block body (base+0x10):
// quadword +0x10 = 0 (sq); GIFtag word +0x1C = 0x50000003; +0x20 = the A+D
// GIFtag dword 0x10000000_00008002; +0x28 = 0xE; +0x30 = 0; +0x38 = 0x3F;
// +0x40 = a packed GS register built from six fields:
//   (arg1>>8)            -> bits  0+   (sign-extended 32->64)
//   ((1<<arg2)>>6)<<14   -> bits 14+
//   arg2<<26             -> bits 26+
//   arg3<<30             -> bits 30+
//   t1p<<34              -> bits 34+
//   t0p<<35              -> bits 35+
// +0x48 = 6. Returns the block body pointer (cursor +0x10).
typedef unsigned __int128 u128;
extern char **D_00275670;

char *func_001D6BA0(int arg0, int arg1, int arg2, int arg3, int t0p, int t1p) {
    char **t3;
    char *t1;

    t3 = (char **)(&D_00275670[arg0]);
    *(char *)(t3[4] + 3) = 0x10;
    *(int *)(t3[4] + 4) = 0;
    *(short *)(t3[4] + 0) = 4;
    t1 = t3[4];
    t3[4] = t1 + 0x50;
    *(u128 *)(t1 + 0x10) = 0;
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
    return t1 + 0x10;
}
