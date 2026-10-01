// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00826310 (splat/link name 008262D0; overlay code
//  is linked 0x40 below where it runs), 0x1C0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 008262D0, 00826310 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8262D0 (+3 == 0). State 0: D_00810856 = 0xFF; state 3
//  if func_001BA1C0(self, 0x35), else state 1 and +0 = 1. State 1: +5 0: ORs 1
//  into D_0081080D when D_00810702 == 1 and 2 when it is 5; +5 1 once
//  D_0081080D == 3. +5 1: script 0x82B5E0 when D_00810702 is 6 or 2. +5 2: at
//  the script end D_008106C0 = func_001B6660(group 0x82A540), +0x2E = 0xFFFF,
//  D_00810856 = 0, func_001FB0B0(0xC), state 3. func_001B17A0 each frame.
#define B8(a) (*(unsigned char *)(a))
extern int D_008106C0[];
extern unsigned char D_00810702[];
extern unsigned char D_0081080D[];
extern char D_overlay_AREA21_0082B5E0[];
extern char D_overlay_AREA21_0082A540[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern int func_001B6660(void *group);
extern void func_001FB0B0(int a);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_008262D0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        B8(0x810856) = 0xFF;
        if (func_001BA1C0(self, 0x35) != 0) {
            self[4] = 3;
        } else {
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810702[0] == 1) {
                B8(0x81080D) |= 1;
            }
            if (D_00810702[0] == 5) {
                B8(0x81080D) |= 2;
            }
            if (D_0081080D[0] == 3) {
                self[5] = 1;
            }
            break;
        case 1:
            if (D_00810702[0] == 6 || D_00810702[0] == 2) {
                func_001BA1A0(talk, D_overlay_AREA21_0082B5E0);
                self[5] = 2;
            }
            break;
        case 2:
            if (func_001BA1F0(self) != 0) {
                D_008106C0[0] = func_001B6660(D_overlay_AREA21_0082A540);
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                B8(0x810856) = 0;
                func_001FB0B0(0xC);
                self[4] = 3;
            }
            break;
        }
        func_001B17A0(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
