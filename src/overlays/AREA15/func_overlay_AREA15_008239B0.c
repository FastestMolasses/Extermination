// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x008239F0 (splat/link name 008239B0; overlay code is
// linked 0x40 below where it runs), 0x144 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 008239B0, 008239F0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x8239F0, D_008107FB 0) for +0xD 0x54: sub-state 0 waits
//  until D_00810354 >= 310 and func_001B1EA0(0, D_00810350, 0x827C80, 6) == 1,
//  then +5 = 1, D_0081077B = 1 and script 0x827400; 1 at script end calls
//  func_001FAE70(0), D_008107FB = 1, +0x2E = 0xFFFF, +5 = 0, +0x28 = 900.
//  While D_70003B92 == 0: func_001C64F0(1.0), func_001B17A0, func_001C68C0,
//  the +0x4C method and 0x823E40.
extern float D_00810350[];
extern unsigned char D_0081077B;
extern unsigned char D_008107FB;
extern unsigned char D_70003B92;
extern char D_overlay_AREA15_00827C80[];
extern char D_overlay_AREA15_00827400[];
extern int func_001B1EA0(int a, void *pos, void *box, int n);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FAE70(int a);
extern void func_001C64F0(unsigned char *self, float step);
extern int func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_overlay_AREA15_00823E40(unsigned char *self);

void func_overlay_AREA15_008239B0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    if (self[0xD] == 0x54) {
        switch (self[5]) {
        case 0:
            if (!(D_00810350[1] < 310.0f)) {
                if (func_001B1EA0(0, D_00810350, D_overlay_AREA15_00827C80, 6) == 1) {
                    self[5] = 1;
                    D_0081077B = 1;
                    func_001BA1A0(blk, D_overlay_AREA15_00827400);
                }
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001FAE70(0);
                D_008107FB = 1;
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                self[5] = 0;
                *(short *)(self + 0x28) = 900;
            }
            break;
        }
    }
    if (D_70003B92 == 0) {
        func_001C64F0(self, 1.0f);
        func_001B17A0(self);
        func_001C68C0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        func_overlay_AREA15_00823E40(self);
    }
}
