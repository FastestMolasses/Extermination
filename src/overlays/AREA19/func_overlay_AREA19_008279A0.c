// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x008279E0 (splat/link name 008279A0; overlay code
//  is linked 0x40 below where it runs), 0x124 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 1 placement [34]: state 3 at once when func_001BA1C0(self, 0x46)
//  is set (func_0019C6F0(8, 1), func_001B6660(0x82A9C0)). +5 0: script
//  0x82E090 when D_00810702 == 1 and D_70003B8D != 4; +5 1:
//  func_001C4760(0xB, 1), D_0081081E = 0xFF and state 3 when it ends.
extern char D_overlay_AREA19_0082A9C0[];
extern char D_overlay_AREA19_0082E090[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_0019C6F0(int id, int on);
extern void *func_001B6660(void *p);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int id, int on);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_008279A0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x46) != 0) {
            func_0019C6F0(8, 1);
            func_001B6660(D_overlay_AREA19_0082A9C0);
            self[4] = 3;
            return;
        }
        self[4] = 1;
        self[0] = 1;
        return;
    case 1:
        switch (self[5]) {
        case 0:
            if (*(unsigned char *)0x810702 == 1 && *(unsigned char *)0x70003B8D != 4) {
                func_001BA1A0(talk, D_overlay_AREA19_0082E090);
                self[5] = 1;
            }
            return;
        case 1:
            if (func_001BA1F0(self) != 0) {
                func_001C4760(0xB, 1);
                *(unsigned char *)0x81081E = 0xFF;
                self[4] = 3;
            }
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
