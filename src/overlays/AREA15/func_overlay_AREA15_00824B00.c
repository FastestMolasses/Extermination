// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824B40 (splat/link name 00824B00; overlay code is
// linked 0x40 below where it runs), 0x148 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00824B00, 00824B40 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x824B40, D_008107FF 0) +0xD 0x50 while D_0081077F != 1:
//  func_001C64F0(1.0), func_001C68C0, the +0x4C method, func_001B17A0. +0xD
//  0x5A: sub-state +5 0 waits for D_00810702 == 1 and starts script 0x828CA0
//  (+5 = 1); 1 at script end sets +5 = 0, +0x2E = 0xFFFF, func_001C47E0(0x2B,
//  1), func_001C4760(0x1C, 1), D_008107FF = 1, sets bit 2 of D_00810847,
//  func_001FB0B0(0); then func_001B17A0 and +1 = 1.
extern unsigned char D_0081077F;
extern unsigned char D_00810702;
extern unsigned char D_008107FF;
extern char D_overlay_AREA15_00828CA0[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C47E0(int a0, int a1);
extern void func_001C4760(int a0, int a1);
extern void func_001FB0B0(int n);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);

void func_overlay_AREA15_00824B00(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    if (self[0xD] == 0x50 && D_0081077F != 1) {
        func_001C64F0(self, 1.0f);
        func_001C68C0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        func_001B17A0(self);
    }
    if (self[0xD] == 0x5A) {
        switch (self[5]) {
        case 0:
            if (D_00810702 == 1) {
                func_001BA1A0(blk, D_overlay_AREA15_00828CA0);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[5] = 0;
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                func_001C47E0(0x2B, 1);
                func_001C4760(0x1C, 1);
                D_008107FF = 1;
                /* literal address: keeps the D_008107FF store ahead of this load */
                *(unsigned char *)0x00810847 |= 4;
                func_001FB0B0(0);
            }
            break;
        }
        func_001B17A0(self);
        self[1] = 1;
    }
}
