// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 76.87% NEARMISS).
// Fixes read from the original instructions:
// - The three patched state bytes D_00810700/01/02 are volatile: the original
//   stores each one and then re-reads D_00810700 (the index) and D_00810701
//   (the value) for the indexed table store, instead of folding the constants.
//   The earlier "const-prop-era wall" verdict was wrong.
//
// Saves the 0x640-byte global state block at 0x70000640 -> backup buffer
// (D_00810700), runs func_001AF2C0, restores it to 0x70000000, then patches the
// first three bytes of the block (D_00810700 = 0xB, +1 = 0, +2 = 0), writes
// D_00810701 (=0) into the table at D_00810730 indexed by D_00810700 (=0xB),
// and finally copies the patched block back to 0x70000640. block_copy(dst,src,
// len).
extern void block_copy(unsigned char *dst, unsigned char *src, int len);
extern void func_001AF2C0(void);
extern volatile unsigned char D_00810700;
extern volatile unsigned char D_00810701;
extern volatile unsigned char D_00810702;
extern unsigned char D_00810730;

void func_001AF220(void) {
    block_copy((unsigned char *)0x70000640, (unsigned char *)&D_00810700, 0x640);
    func_001AF2C0();
    block_copy((unsigned char *)0x70000000, (unsigned char *)&D_00810700, 0x640);
    D_00810700 = 0xB;
    D_00810701 = 0;
    D_00810702 = 0;
    (&D_00810730)[D_00810700] = D_00810701;
    block_copy((unsigned char *)&D_00810700, (unsigned char *)0x70000640, 0x640);
}
