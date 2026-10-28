// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.09% NEARMISS).
// Fixes read from the original instructions:
// - D_00275670 is a table of channel write cursors (char **); the channel slot is
//   taken as &D_00275670[idx] and its cursor read as slot[4], spelled exactly like
//   the byte-matched twin func_001D6580 (same packet header, larger block).
// - The packet body pointer (block + 0x10) is computed before the cursor advance
//   and the GIFtag constant is spelled as 0x8001 | (0x30004000 << 32), as in the
//   twin. Together these restore the original register colouring and constant
//   order (not isolated which single change was decisive).
//
// DMA/GIF packet builder. Indexes a channel table at D_00275670 by idx to reach
// a per-channel write cursor (slot[4]) into a packet buffer. It stamps a small
// header on the cursor (byte +3 = 0x10, word +4 = 0, short +0 = 5), then advances
// the cursor by 0x60 and fills the just-claimed 0x60 block: a 128-bit zero clear
// at +0x10, a DMA/source-chain tag word 0x50000004 at +0x1C, a 64-bit GIFtag at
// +0x20 (0x30004000_00008001 with the register/NREG field reg|6 shifted into
// bits 47+), the GS reg-list 0x441 at +0x28, then three 4-vector payloads copied
// from a3 (4 words at +0x30), a1 (3 words at +0x40, +0x4C zeroed) and a2
// (3 words at +0x50, +0x5C zeroed). Returns the packet body pointer
// (claimed block + 0x10). `reg` is the fifth argument register.
typedef unsigned __int128 u128;
extern char **D_00275670;

char *func_001D64A0(int idx, int *a1, int *a2, int *a3, int reg) {
    char **slot;
    char *blk;
    char *ret;

    slot = (char **)(&D_00275670[idx]);
    *((char *)(slot[4] + 3)) = 0x10;
    *((int *)(slot[4] + 4)) = 0;
    *((short *)(slot[4] + 0)) = 5;
    blk = slot[4];
    ret = blk + 0x10;
    slot[4] = blk + 0x60;
    *((u128 *)ret) = 0;
    *((int *)(blk + 0x1C)) = 0x50000004;
    *((long long *)(blk + 0x20)) = (((long long)((int)(reg | 6))) << 0x2F) | (0x8001 | (((long long)0x30004000) << 0x20));
    *((long long *)(blk + 0x28)) = 0x441;
    *(int *)(blk + 0x30) = a3[0];
    *(int *)(blk + 0x34) = a3[1];
    *(int *)(blk + 0x38) = a3[2];
    *(int *)(blk + 0x3C) = a3[3];
    *(int *)(blk + 0x40) = a1[0];
    *(int *)(blk + 0x44) = a1[1];
    *(int *)(blk + 0x48) = a1[2];
    *(int *)(blk + 0x4C) = 0;
    *(int *)(blk + 0x50) = a2[0];
    *(int *)(blk + 0x54) = a2[1];
    *(int *)(blk + 0x58) = a2[2];
    *(int *)(blk + 0x5C) = 0;
    return ret;
}
