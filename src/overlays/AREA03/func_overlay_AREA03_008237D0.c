// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA03 overlay, runtime 0x00823810 (splat/link name 008237D0; overlay code
//  is linked 0x40 below where it runs), 0x114 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: sub 1 deferred group 0x826BC0[3]. State 0: 0x823780. State 1:
//  +5 0: on +0xB bit 2 D_00810781 = 1 and +5 = 1; +5 1: state 2 once
//  D_00810781 == 0xFF. While D_00810801 == 0: func_001F1180(self), and the
//  +0x4C method when func_001B17A0 is nonzero. States 2, 3 and above:
//  func_001B1190(+0x9A) and func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810781[];
extern unsigned char D_00810801[];
extern int func_overlay_AREA03_00823780(unsigned char *self);
extern void func_001F1180(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001B1190(int id);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA03_008237D0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        func_overlay_AREA03_00823780(self);
        return;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                D_00810781[0] = 1;
                self[5]++;
            }
            break;
        case 1:
            if (D_00810781[0] == 0xFF) {
                self[4]++;
            }
            break;
        }
        if (D_00810801[0] == 0) {
            func_001F1180(self);
            if (func_001B17A0(self) != 0) {
                (*(ActorFn *)(self + 0x4C))(self);
            }
        }
        break;
    case 2:
    case 3:
    default:
        func_001B1190(self[0x9A]);
        func_001AFC10(self);
        break;
    }
}
