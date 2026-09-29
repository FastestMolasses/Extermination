// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824240 (splat/link name 00824200; overlay code is
// linked 0x40 below where it runs), 0x110 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00824200, 00824240 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x824240) sub-state +5: 0 when func_001BA1C0(self, 0x23) is
//  set starts script 0x827D70, func_001CA6F0(self, 2), func_001FABB0,
//  func_001FBC50, +5 = 1; 1 at script end sets +0x2E = 0xFF, +5 = 0, +0x40 =
//  D_0028A624, func_001C67E0(self, 0, 20.0, 0.0), D_008107FC = 1,
//  func_001C47A0(0x25, 1), func_001FAE70(0), then (every frame of +5 1)
//  func_001BA580(self, +0xD) and talk block +0xE = func_001C64F0(self, 0.5).
extern unsigned char D_008107FC;
extern int D_0028A624;
extern char D_overlay_AREA15_00827D70[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001FABB0(void);
extern void func_001FBC50(void);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001C47A0(int a0, int a1);
extern void func_001FAE70(int a);
extern void func_001BA580(unsigned char *self, int a1);
extern int func_001C64F0(unsigned char *self, float step);

void func_overlay_AREA15_00824200(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (func_001BA1C0(self, 0x23) != 0) {
            func_001BA1A0(blk, D_overlay_AREA15_00827D70);
            func_001CA6F0(self, 2);
            func_001FABB0();
            func_001FBC50();
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            *(short *)(self + 0x2E) = 0xFF;
            self[5] = 0;
            *(int *)(self + 0x40) = D_0028A624;
            func_001C67E0(self, 0, 20.0f, 0.0f);
            D_008107FC = 1;
            func_001C47A0(0x25, 1);
            func_001FAE70(0);
        }
        func_001BA580(self, self[0xD]);
        *(short *)(blk + 0xE) = func_001C64F0(self, 0.5f);
        break;
    }
}
