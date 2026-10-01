// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00827490 (splat/link name 00827450; overlay code is
// linked 0x40 below where it runs), 0x198 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: gun cable (same shape as AREA01 0x828850). State 0 (after
// func_001B0FD0): +0x34 = 1, +0 = 1; if func_001B11E0(+0x9A) the parent
// (+0x18) goes to state 2 and this to state 3. State 1: while +0x36 is 0
// it animates (func_001C6380, func_001B17A0, +0x4C method); when set,
// +0 = 2, parent state 2, parent +0x21C = 90, and it spawns class
// 0x80000045 (func_001EFE00): on success sound 0x426, +0x28 = 0, state 2;
// else state 3. State 2 counts +0x28 to 10 (sound 0x427 at 10), then
// func_001B1190(+0x9A), func_001B17A0 and the +0x4C method. State 3/other:
// func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern int func_001B0FD0(unsigned char *self);
extern int func_001B11E0(int id);
extern void func_001B1190(int id);
extern int func_001EFE00(int cls, unsigned char *obj);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001C6380(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA11_00827450(unsigned char *self) {
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
            break;
        }
        func_001C6380(self);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
        if (*(short *)(self + 0x28) < 10) {
            (*(short *)(self + 0x28))++;
            if (*(short *)(self + 0x28) == 10) {
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
