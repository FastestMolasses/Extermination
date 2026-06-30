// NEARMISS func_001D7510  (vram 0x001D7510, 0xD0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 57.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling near-miss (57.7%, mwcc233): logic fully recovered, identical instruction multiset (the sq quadword-zero matches once typed as mode(TI)). Residual is heavy register-coloring/scheduling divergence of the dsll32/dsra32 64-bit sign-extend temps (target keeps arg2<<34 in...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 57.7% (mwcc233). Per-channel GIFtag/DMA-tag builder: indexes the
// channel record at D_00275670 + idx*4, reads/advances its write pointer (+0x10)
// by 0x50, zeroes a quadword, then writes the GS A+D packet (0x50000003 DMA tag,
// the GIFtag dwords, etc.) and packs the final dword from
// (arg1<<35)|(arg2<<34)|(chip>>8)|const, where chip = (*(int*)(base+0x9C)!=0)?0x70000:0
// (movn select). Body/structure correct; residual is regalloc-order/scheduling of
// the dsll32/dsra32 sign-extend temps and the buf-reload interleave -- permuter territory.
typedef int s128 __attribute__((mode(TI)));
extern char *D_00275670;

void func_001D7510(int idx, long arg1, long arg2) {
    char *ch;
    char *buf;
    int chip;

    ch = D_00275670;
    chip = (*(int *)(ch + 0x9C) != 0) ? 0x70000 : 0;
    ch = ch + (idx << 2);
    buf = *(char **)(ch + 0x10);

    buf[3] = 0x10;
    buf = *(char **)(ch + 0x10);
    *(int *)(buf + 4) = 0;
    buf = *(char **)(ch + 0x10);
    *(short *)(buf + 0) = 4;
    buf = *(char **)(ch + 0x10);
    *(char **)(ch + 0x10) = buf + 0x50;

    *(s128 *)(buf + 0x10) = 0;
    *(int *)(buf + 0x1C) = 0x50000003;
    *(long *)(buf + 0x20) = 0x8002L | ((long)0x10000000 << 32);
    *(long *)(buf + 0x28) = 0xE;
    *(long *)(buf + 0x30) = 0;
    *(long *)(buf + 0x38) = 0x3F;
    *(long *)(buf + 0x40) =
        ((long)(int)arg1 << 35) |
        (((long)(int)arg2 << 34) |
         ((long)(int)(chip >> 8) |
          (0x24020000L | ((long)2 << 32))));
    *(long *)(buf + 0x48) = 6;
}
