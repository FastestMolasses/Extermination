// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x008260F0 (splat/link name 008260B0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 goes to state 3 when func_001BA1C0(self, 0x2B) is set, else
//  func_001B10B0(self, +0xD, 0x6B) and func_001C63E0(self, 0); state 1
//  animates (func_001C64F0 at 1.0) unless D_00810803 is 0 or 1.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810803;
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B10B0(unsigned char *self, int a1, int a2);
extern void func_001C63E0(unsigned char *self, short a1);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_008260B0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x2B) != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x6B);
        func_001C63E0(self, 0);
        self[4] = 1;
        break;
    case 1:
        switch (D_00810803) {
        case 0:
        case 1:
            break;
        default:
            func_001C64F0(self, 1.0f);
            func_001B17A0(self);
            func_001C68C0(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
