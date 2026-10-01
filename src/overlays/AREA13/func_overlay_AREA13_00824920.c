// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00824960 (splat/link name 00824920; overlay code
//  is linked 0x40 below where it runs), 0x88 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 00824920, 00824960 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: [44]'s step 4 (0x824960): +5 0 starts script 0x82C110 once D_00810833
//  != 0; +5 1 calls func_001FAE70(0) and sets state 3 when it ends.
extern unsigned char D_00810833;
extern char D_overlay_AREA13_0082C110[];
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FAE70(int a);

void func_overlay_AREA13_00824920(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_00810833 != 0) {
            func_001BA1A0(talk, D_overlay_AREA13_0082C110);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            func_001FAE70(0);
            self[4] = 3;
        }
        break;
    }
}
