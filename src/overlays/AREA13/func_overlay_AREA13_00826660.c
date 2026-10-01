// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008266A0 (splat/link name 00826660; overlay code
//  is linked 0x40 below where it runs), 0x1AC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: deferred group 0x829D00 member (x4); the AREA01 0x828850 twin plus
//  one test: state 1 waits while D_00810702 < 8.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810702;
extern int func_001B0FD0(unsigned char *self);
extern int func_001B11E0(int id);
extern int func_001EFE00(int a, unsigned char *b);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern void func_001C6380(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001B1190(int a0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00826660(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            *(short *)(self + 0x34) = 1;
            self[0] = 1;
            if (func_001B11E0(self[0x9A]) != 0) {
                (*(unsigned char **)(self + 0x18))[4] = 2;
                self[4] = 3;
            }
        }
        break;
    case 1:
        if (D_00810702 < 8) {
            break;
        }
        if (*(short *)(self + 0x36) != 0) {
            self[0] = 2;
            (*(unsigned char **)(self + 0x18))[4] = 2;
            *(int *)(*(unsigned char **)(self + 0x18) + 0x21C) = 0x5A;
            if (func_001EFE00(0x80000045, self) != 0) {
                func_001FBD50(self, 0x426, 0, 300.0f);
                *(short *)(self + 0x28) = 0;
                self[4] = 2;
            } else {
                self[4] = 3;
            }
        } else {
            func_001C6380(self);
            func_001B17A0(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
        if (*(short *)(self + 0x28) < 0xA) {
            *(short *)(self + 0x28) += 1;
            if (*(short *)(self + 0x28) == 0xA) {
                func_001FBD50(self, 0x427, 0, 300.0f);
            }
        }
        func_001B1190(self[0x9A]);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
