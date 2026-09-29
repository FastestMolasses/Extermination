// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x008236B0 (splat/link name 00823670; overlay code is
// linked 0x40 below where it runs), 0xC8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00823670, 008236B0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-state +5: 0 starts script 0x826E70 on the talk block (+0x1F0), +5
//  = 1, D_00810702 = 1; 1 at script end (func_001BA1F0) calls
//  func_001C4760(0xD, 1), sets +0x2E = 0xFFFF, D_008107FA = 1, +5 = 0 and
//  func_001FAE70(0). Then func_001C64F0(1.0), func_001B17A0, func_001C68C0 and
//  the +0x4C method. Called as 0x8236B0 from 0x8235A0.
extern unsigned char D_00810702;
extern unsigned char D_008107FA;
extern char D_overlay_AREA15_00826E70[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int a0, int a1);
extern void func_001FAE70(int a);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);

void func_overlay_AREA15_00823670(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        func_001BA1A0(blk, D_overlay_AREA15_00826E70);
        self[5] = 1;
        D_00810702 = 1;
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            func_001C4760(0xD, 1);
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            D_008107FA = 1;
            self[5] = 0;
            func_001FAE70(0);
        }
        break;
    }
    func_001C64F0(self, 1.0f);
    func_001B17A0(self);
    func_001C68C0(self);
    (*(void (**)(unsigned char *))(self + 0x4C))(self);
}
