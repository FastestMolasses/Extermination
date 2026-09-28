// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00824930 (splat/link name 008248F0;
// overlay code is linked 0x40 below where it runs), 0x10C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Covers the splat pieces 008248F0, 00824930 (the later piece is absorbed at link time,
// tools/overlay/fill_overlay.py).
// Role: sub-state +6 (called as 0x824930): 0 with an object at +0x240
// copies its +0xB0 (16 bytes, func_00121870) to +0x2E0 and starts script
// 0x8286D0; without one calls func_001B6660(0x827230),
// func_001EFD20(0x8000005B, 0x828B90), D_0081076A = D_008107EA = 0xFF, sub
// 2; 1 at script end sub 2; 2 D_008107EA = 0xFF, sub 3.
extern unsigned char D_0081076A;
extern unsigned char D_008107EA;
extern char D_overlay_AREA04_008286D0[];
extern char D_overlay_AREA04_00827230[];
extern char D_overlay_AREA04_00828B90[];
extern void func_00121870(void *dst, void *src, int n);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void *func_001B6660(char *p);
extern int func_001EFD20(int code, char *ptr);

void func_overlay_AREA04_008248F0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    char **fx = (char **)(self + 0x240);
    unsigned char *save = self + 0x2E0;
    switch (self[6]) {
    case 0:
        if (*fx != 0) {
            func_00121870(save, *fx + 0xB0, 0x10);
            func_001BA1A0(blk, D_overlay_AREA04_008286D0);
            self[6] = 1;
        } else {
            func_001B6660(D_overlay_AREA04_00827230);
            func_001EFD20(0x8000005B, D_overlay_AREA04_00828B90);
            D_0081076A = 0xFF;
            D_008107EA = 0xFF;
            self[6] = 2;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            self[6] = 2;
        }
        break;
    case 2:
        D_008107EA = 0xFF;
        self[6] = 3;
        break;
    case 3:
        break;
    }
}
