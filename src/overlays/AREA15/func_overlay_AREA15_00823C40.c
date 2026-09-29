// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00823C80 (splat/link name 00823C40; overlay code is
// linked 0x40 below where it runs), 0x1B4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Covers the splat pieces 00823C40, 00823C80 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: (runtime 0x823C80, D_008107FB 2..4) for +0xD 0x54, else state 3.
//  Sub-state +5: 0 starts script 0x8277C0, +0x2E = 0; 1 at script end calls
//  func_001C47A0(6, 1), func_001C4760(0xE, 1), +0x2E = 0xFFFF, +5 = 2,
//  D_70003B8D = 3; 2 with +6: 0 calls func_001FABB0, func_001FBC50,
//  func_001AEDE0(0xFF, 0), +0x28 = 60, +6 = 1; 1 counts +0x28 down to +5 = 3;
//  3 sets D_008107FB = 0xFF, D_008106B8 = 1, D_008106B5 = 0xF, D_008106B6/B7 =
//  0, state 3 and D_00810730/31/32/34/36 = 0x82/0x81/0x82/0x81/0x81.
extern unsigned char D_008107FB;
extern unsigned char D_008106B5[];
extern unsigned char D_008106B6;
extern unsigned char D_008106B7;
extern unsigned char D_008106B8;
extern unsigned char D_00810730[];
extern unsigned char D_00810731;
extern unsigned char D_00810732;
extern unsigned char D_00810734;
extern unsigned char D_00810736;
extern unsigned char D_70003B8D;
extern char D_overlay_AREA15_008277C0[];
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C47A0(int a0, int a1);
extern void func_001C4760(int a0, int a1);
extern void func_001FABB0(void);
extern void func_001FBC50(void);
extern void func_001AEDE0(int a0, int a1);

void func_overlay_AREA15_00823C40(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    if (self[0xD] == 0x54) {
        switch (self[5]) {
        case 0:
            func_001BA1A0(blk, D_overlay_AREA15_008277C0);
            self[5] = 1;
            *(unsigned short *)(self + 0x2E) = 0;
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001C47A0(6, 1);
                func_001C4760(0xE, 1);
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                self[5] = 2;
                D_70003B8D = 3;
            }
            break;
        case 2:
            switch (self[6]) {
            case 0:
                func_001FABB0();
                func_001FBC50();
                func_001AEDE0(0xFF, 0);
                *(short *)(self + 0x28) = 60;
                self[6] = 1;
                break;
            case 1:
                (*(short *)(self + 0x28))--;
                if (*(short *)(self + 0x28) == 0) {
                    self[5] = 3;
                }
                break;
            }
            break;
        case 3:
            D_008107FB = 0xFF;
            /* the extern-array form keeps these byte stores in source order */
            D_008106B5[3] = 1;
            D_008106B5[0] = 0xF;
            D_008106B5[1] = 0;
            D_008106B5[2] = 0;
            self[4] = 3;
            D_00810730[0] = 0x82;
            D_00810730[1] = 0x81;
            D_00810730[2] = 0x82;
            D_00810730[4] = 0x81;
            D_00810730[6] = 0x81;
            break;
        }
    } else {
        self[4] = 3;
    }
}
