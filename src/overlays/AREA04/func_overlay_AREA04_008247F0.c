// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00824830 (splat/link name 008247F0;
// overlay code is linked 0x40 below where it runs), 0xFC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Covers the splat pieces 008247F0, 00824830 (the later piece is absorbed at link time,
// tools/overlay/fill_overlay.py).
// Role: sub-state +5 (called as 0x824830): 0 starts script 0x828450 when
// D_008107EA is 0, else stores func_001B6660(0x826790) at +0x240, calls
// func_001B6660(0x8267F0) and goes to 2; 1 at script end +0x2E = 0xFFFF,
// the same two func_001B6660 calls, D_008107EA = 0x10, func_001FAE70(0),
// sub 2.
extern unsigned char D_008107EA;
extern char D_overlay_AREA04_00828450[];
extern char D_overlay_AREA04_00826790[];
extern char D_overlay_AREA04_008267F0[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void *func_001B6660(char *p);
extern void func_001FAE70(int a);

void func_overlay_AREA04_008247F0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    void **fx = (void **)(self + 0x240);
    switch (self[5]) {
    case 0:
        if (D_008107EA == 0) {
            self[5] = 1;
            func_001BA1A0(blk, D_overlay_AREA04_00828450);
        } else {
            self[5] = 2;
            *fx = func_001B6660(D_overlay_AREA04_00826790);
            func_001B6660(D_overlay_AREA04_008267F0);
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            *fx = func_001B6660(D_overlay_AREA04_00826790);
            func_001B6660(D_overlay_AREA04_008267F0);
            D_008107EA = 0x10;
            func_001FAE70(0);
            self[5] = 2;
        }
        break;
    case 2:
        break;
    }
}
