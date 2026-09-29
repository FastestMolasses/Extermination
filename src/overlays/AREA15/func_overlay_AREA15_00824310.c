// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824350 (splat/link name 00824310; overlay code is
// linked 0x40 below where it runs), 0x84 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00824310, 00824350 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x824350) sub-state +5: 0 starts script 0x8281F0, +5 = 1; 1
//  at script end sets D_008107FC = 2, func_001C4760(0xF, 1), +5 = 0.
extern unsigned char D_008107FC;
extern char D_overlay_AREA15_008281F0[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int a0, int a1);

void func_overlay_AREA15_00824310(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        func_001BA1A0(blk, D_overlay_AREA15_008281F0);
        self[5] = 1;
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            D_008107FC = 2;
            func_001C4760(0xF, 1);
            self[5] = 0;
        }
        break;
    }
}
