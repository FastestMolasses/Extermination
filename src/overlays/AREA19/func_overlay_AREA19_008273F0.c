// NEARMISS func_overlay_AREA19_008273F0 (94.91%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00827430 (splat/link name 008273F0; overlay code
//  is linked 0x40 below where it runs), 0x104 bytes.
// Role (read from the instructions): sub 1 placement [35]: follows the linked
//  object's (+0x1C) height: in state 1, when its +0xB4 differs from +0x2E4,
//  +0xB4 = that height + 7.9, +0x2E4 = that height, func_001C6380 and
//  func_001A2370; then func_001B1B70 and the +0x4C method. State 0 when
//  func_001B0FD0 returns 0: func_001C6380, +0x2E4 = the linked height,
//  func_001A2370.
// Divergence: the original computes the store address (self + 0x1F0) + 0xF4
//  as its own pointer before the compare (the first add fills the case-1
//  dispatch slot) and stores through it; mwcc 2.3.3 folds the store to self +
//  0x2E4. A pointer local keeps the two adds but schedules them after the
//  compare and loses the branch-likely; block pointers of several types were
//  tried.
typedef void (*ActorFn)(unsigned char *);
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *m);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_008273F0(unsigned char *self) {
    float y;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            *(float *)(self + 0x2E4) = *(float *)(*(unsigned char **)(self + 0x1C) + 0xB4);
            func_001A2370(self, self + 0xD0);
        }
        break;
    case 1:
        if (*(float *)(self + 0x2E4) != (y = *(float *)(*(unsigned char **)(self + 0x1C) + 0xB4))) {
            *(float *)(self + 0xB4) = 7.899994f + y;
            ((float *)(self + 0x1F0))[0x3D] = *(float *)(*(unsigned char **)(self + 0x1C) + 0xB4);
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
        }
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
