// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00824490 (splat/link name 00824450;
// overlay code is linked 0x40 below where it runs), 0x158 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Covers the splat pieces 00824450, 00824490 (the later piece is absorbed at link time,
// tools/overlay/fill_overlay.py).
// Role: sub-state +5 (called as 0x824490 from 0x824320): 0 starts script
// 0x827D90 when func_001B1EA0(0, D_00810350, area copied from 0x828220, 4)
// == 1; 1 at script end sets +0x2E = 0xFFFF, +0x40 = D_0028A5D8, clip 0,
// func_001C47A0(0x23, 1), func_001C4760(8, 1), D_008107E9 = 1,
// func_001FAE70(0), then steps at 0.5 into talk +0xE. Otherwise
// func_001BA580(self, +0xD) and func_001C64F0 1.0.
typedef struct { float v[4][4]; } Quad __attribute__((aligned(16)));
extern Quad D_overlay_AREA04_00828220;
extern char D_00810350[];
extern unsigned char D_008107E9;
extern int D_0028A5D8;
extern char D_overlay_AREA04_00827D90[];
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001C47A0(int a, int b);
extern void func_001C4760(int a, int b);
extern void func_001FAE70(int a);
extern void func_001BA580(unsigned char *self, int a1);
extern short func_001C64F0(unsigned char *self, float step);

void func_overlay_AREA04_00824450(unsigned char *self) {
    Quad area = D_overlay_AREA04_00828220;
    unsigned char *blk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (func_001B1EA0(0, D_00810350, &area, 4) == 1) {
            func_001BA1A0(blk, D_overlay_AREA04_00827D90);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            self[5] = 0;
            *(int *)(self + 0x40) = D_0028A5D8;
            func_001C67E0(self, 0, 0.0f, 0.0f);
            func_001C47A0(0x23, 1);
            func_001C4760(8, 1);
            D_008107E9 = 1;
            func_001FAE70(0);
        }
        func_001BA580(self, self[0xD]);
        *(short *)(blk + 0xE) = func_001C64F0(self, 0.5f);
        return;
    }
    func_001BA580(self, self[0xD]);
    func_001C64F0(self, 1.0f);
}
