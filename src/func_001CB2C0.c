// NEARMISS func_001CB2C0  (vram 0x001CB2C0, 0x9C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.72% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// instruction-scheduling permutation: target emits `addiu s1,a2,0x10` before `sw zero,0x10(a2)`; mwcc's list scheduler emits the store first regardless of source form (tried store-through-s1 and full s1-relative rebasing). Permuter/scheduling-class wall, not the clean-store nop case.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS (mwcc 2.3.3 = 94.9%, pinned 991202 = 63.4%). Logic fully recovered.
// Builds a GS/DMA packet in the slot table. base = *(arg0+0x90). slot = the
// gp-relative table D_00275670 indexed by arg2 (word stride). The current
// packet pointer lives at slot[4] (offset 0x10): writes a GIFtag-ish header
// (byte +3 = 0x10 NLOOP/EOP, word +4 = 0, half +0 = 3), captures p = slot[4],
// advances slot[4] by 0x40, then fills the next 0x40 region: zeroes +0x10/+0x14,
// writes 0x01000404 at +0x18 and (arg1 | 0x6C020000) at +0x1C (a GS register
// address fused with arg1), and finally issues two func_00102948 calls
// (s1+0x10, base+0x40) and (s1+0x20, base+0x50) where s1 = p+0x10. func_00102948
// takes 2 args (a2/a3 at the call sites are loop leftovers). D_00275670 needs
// -sdatathreshold >=4 so 233 reaches it via %gp_rel.
//
// WALL: the sole 2.3.3 residual is a 2-instruction schedule swap -- the target
// emits `addiu s1, a2, 0x10` (the call-arg base) BEFORE `sw zero, 0x10(a2)`,
// while mwcc deterministically emits the store first. Tried writing the zero
// store through s1 and rebasing all four field stores to s1; mwcc's list
// scheduler always picks store-first. Pure scheduling/permuter-class wall.
extern int *D_00275670;
extern void func_00102948(int a, int b);

void func_001CB2C0(int arg0, int arg1, int arg2) {
    int base;
    int *slot;
    int p;
    int s1;

    base = *(int *)(arg0 + 0x90);
    slot = (int *)((char *)D_00275670 + arg2 * 4);
    *(unsigned char *)(slot[4] + 3) = 0x10;
    *(int *)(slot[4] + 4) = 0;
    *(short *)(slot[4] + 0) = 3;
    p = slot[4];
    slot[4] = p + 0x40;
    s1 = p + 0x10;
    *(int *)(p + 0x10) = 0;
    *(int *)(p + 0x14) = 0;
    *(int *)(p + 0x18) = 0x01000404;
    *(int *)(p + 0x1C) = arg1 | 0x6C020000;
    func_00102948(s1 + 0x10, base + 0x40);
    func_00102948(s1 + 0x20, base + 0x50);
}
