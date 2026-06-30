// NEARMISS func_001D64A0  (vram 0x001D64A0, 0xDC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.09% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring (target colors address/const temporaries $t0-$t4; mwcc picks $a4-$a7) + materialization order of the +0x20 GIFtag doubleword vs the 0x441 const. All opcodes/widths (sq/sd/sw)/offsets/shift sequence match. Body 100% correct. Permuter-class, NOT clean-store nop. 233=90....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 90.1% (mwcc 2.3.3). DMA/GIF packet builder. Indexes a channel table
// at D_00275670 by idx (4-byte stride) to reach a per-channel context; field
// +0x10 of that context is the current write pointer into a packet buffer. It
// stamps a small header on the pointer (byte +3 = 0x10, word +4 = 0, short +0 =
// 5), then advances the channel's +0x10 cursor by 0x60 and fills the just-claimed
// 0x60 block: a 128-bit zero clear at +0x10, a DMA/source-chain tag word
// 0x50000004 at +0x1C, a 64-bit GIFtag at +0x20 (0x30004000_00008001 with the
// register/NREG field reg|6 shifted into bits 47+), the GS reg-list 0x441 at
// +0x28, then three 4-vector payloads copied from a3 (4 words at +0x30),
// a1 (3 words at +0x40, +0x4C zeroed) and a2 (3 words at +0x50, +0x5C zeroed).
// Returns the packet body pointer (claimed block + 0x10).
//
// NOTE: the GIFtag's register field is built from an EE register ($t0) that is
// live on entry but not one of a0-a3 -- modeled here as a 5th param `reg`
// (uninitialized-in-ABI read in the original). All stores, widths (sq/sd/sw),
// offsets and the shift sequence match. Residual is register-allocation coloring
// (target uses $t0-$t4 for the address/const temporaries; mwcc colors them
// $a4-$a7) plus the order the 0x20-doubleword value vs the 0x441 const are
// materialized -- regalloc/scheduling class -> permuter, not the clean-store nop.
// 233 (90.1%) beats 991202 (87.2%); pinned build is 233.
extern int D_00275670;

char *func_001D64A0(int idx, int *a1, int *a2, int *a3, int reg) {
    int *slot;
    char *p;

    slot = (int *)(D_00275670 + idx * 4);
    *(char *)(*(char **)(slot + 4) + 3) = 0x10;
    *(int *)(*(char **)(slot + 4) + 4) = 0;
    *(short *)(*(char **)(slot + 4) + 0) = 5;
    p = *(char **)(slot + 4);
    *(char **)(slot + 4) = p + 0x60;
    *(unsigned __int128 *)(p + 0x10) = 0;
    *(int *)(p + 0x1C) = 0x50000004;
    *(long long *)(p + 0x20) = ((long long)(reg | 6) << 47) | 0x3000400000008001LL;
    *(long long *)(p + 0x28) = 0x441;
    *(int *)(p + 0x30) = a3[0];
    *(int *)(p + 0x34) = a3[1];
    *(int *)(p + 0x38) = a3[2];
    *(int *)(p + 0x3C) = a3[3];
    *(int *)(p + 0x40) = a1[0];
    *(int *)(p + 0x44) = a1[1];
    *(int *)(p + 0x48) = a1[2];
    *(int *)(p + 0x4C) = 0;
    *(int *)(p + 0x50) = a2[0];
    *(int *)(p + 0x54) = a2[1];
    *(int *)(p + 0x58) = a2[2];
    *(int *)(p + 0x5C) = 0;
    return p + 0x10;
}
