// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 57.42% NEARMISS).
// Fixes read from the original instructions:
// - D_00275670 is a table of channel write cursors (char **); the slot is
//   &D_00275670[idx] and the cursor is read directly as slot[4] for each header
//   store, spelled like the byte-matched twin func_001D64A0.
// - The packet body pointer (block + 0x10) is taken into its own local before
//   the cursor advance and used both for the quadword clear and as the return
//   value; this restores the original scheduling of the header stores against
//   the constant setup (the residual was not a register-allocation wall).
// - The GIFtag dword is spelled 0x8001 | (0x50004000 << 32) OR'd with the
//   sign-extended (arg1 | 0x116) shifted into bits 47+, as in the twin.
//
// Builds a GIF/DMA packet into a channel's ring buffer. Through the channel's
// write cursor it writes a packet header: byte +3 = 0x10, word +4 = 0,
// halfword +0 = 7; then advances the cursor by 0x80. Into the just-claimed
// 0x80 block it stores: a zeroed quadword at +0x10, the word 0x50000006 at
// +0x1C, the 64-bit GIFtag at +0x20 (0x50004000_00008001 with arg1 | 0x116 in
// bits 47+), the register list 0x43431 at +0x28, then five source quadwords:
// arg4[0] -> +0x30, arg3[0] -> +0x40, arg2[0] -> +0x50, arg3[1] -> +0x60,
// arg2[1] -> +0x70. Returns the packet body pointer (block + 0x10).
typedef unsigned __int128 u128;
extern char **D_00275670;

char *func_001D7A80(int idx, int arg1, u128 *arg2, u128 *arg3, u128 *arg4) {
    char **slot;
    char *blk;
    char *ret;

    slot = (char **)(&D_00275670[idx]);
    *((char *)(slot[4] + 3)) = 0x10;
    *((int *)(slot[4] + 4)) = 0;
    *((short *)(slot[4] + 0)) = 7;
    blk = slot[4];
    ret = blk + 0x10;
    slot[4] = blk + 0x80;
    *((u128 *)ret) = 0;
    *((int *)(blk + 0x1C)) = 0x50000006;
    *((long long *)(blk + 0x20)) =
        (((long long)((int)(arg1 | 0x116))) << 0x2F) | (0x8001 | (((long long)0x50004000) << 0x20));
    *((long long *)(blk + 0x28)) = 0x43431;
    *(u128 *)(blk + 0x30) = *arg4;
    *(u128 *)(blk + 0x40) = *arg3;
    *(u128 *)(blk + 0x50) = *arg2;
    *(u128 *)(blk + 0x60) = arg3[1];
    *(u128 *)(blk + 0x70) = arg2[1];
    return ret;
}
