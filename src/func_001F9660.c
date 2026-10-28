// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.23% NEARMISS).
// Fixes read from the original instructions:
// - The region test is a switch with one case (0x1500) and a default, not an
//   if/else: the original branches to the case when equal, with the case's first
//   compare in the delay slot, then jumps to the default block. An if/else gives
//   the inverted branch with the else block first.

// Copies 0x2F0 bytes into a local buffer, then sets buf[3] based on a region key
// ((D_00810700<<8)+D_00810701) and the `key` arg, dispatching func_001F91C0(buf).
// No key match returns without calling. Single shared call site; inner switch
// cases written in reversed source order (mwcc reverses to ascending dispatch).
extern void block_copy(void *dst, void *src, int len);
extern void func_001F91C0(void *buf);
extern unsigned char D_00810700;
extern unsigned char D_00810701;

void func_001F9660(void *src, int key) {
    unsigned char buf[0x2F0];

    block_copy(buf, src, 0x2F0);
    switch ((D_00810700 << 8) + D_00810701) {
    case 0x1500:
        switch (key) {
        case 0x9D:
            buf[3] = 0xA;
            break;
        case 0xA0:
            buf[3] = 0xB;
            break;
        default:
            return;
        }
        break;
    default:
        switch (key) {
        case 0x9C:
            buf[3] = 0x2;
            break;
        case 0x9D:
            buf[3] = 0x9;
            break;
        case 0x6E:
            buf[3] = 0x1;
            break;
        case 0x72:
            buf[3] = 0xC;
            break;
        default:
            return;
        }
    }
    func_001F91C0(buf);
}
