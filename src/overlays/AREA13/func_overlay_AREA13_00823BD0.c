// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823C10 (splat/link name 00823BD0; overlay code
//  is linked 0x40 below where it runs), 0x138 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 00823BD0, 00823C10 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: [5]'s handler (from 0x823BC0, +0xD 0). State 0: state 3 when
//  func_001BA1C0(self, 0x1B) is set, else +0x30 = 0x82B070, +0 = 1, state 1.
//  State 1: state 3 once D_00810C8B != 0; +5 0 starts script 0x82AB70 when
//  +0xB bit 2 is set; +5 1 calls func_001AEE10(4, 0) and clears +0xB / +5
//  when it ends; then func_001B17A0. States 2/3 func_001AFC10.
extern char D_overlay_AREA13_0082B070[];
extern char D_overlay_AREA13_0082AB70[];
extern unsigned char D_00810C8B;
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001AEE10(int a, int b);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00823BD0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x1B) != 0) {
            self[4] = 3;
            break;
        }
        *(void **)(self + 0x30) = D_overlay_AREA13_0082B070;
        self[0] = 1;
        self[4] = 1;
        break;
    case 1:
        if (D_00810C8B != 0) {
            self[4] = 3;
            break;
        }
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                func_001BA1A0(talk, D_overlay_AREA13_0082AB70);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001AEE10(4, 0);
                self[0xB] = 0;
                self[5] = 0;
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
