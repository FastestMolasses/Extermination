// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825350 (splat/link name 00825310; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: sub-0 placement owner (the control-room NPC record [36] per the route
// capture, docs/SECOND_LEVEL_ROUTE.md; the old 'crank/drawbridge' label was wrong); dispatches
//  the talk sub-states 0x8254B0/0x825590/0x825670 on D_008107D9.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081075A;
extern unsigned char D_008107D9;
extern int D_0028A5C4;
extern char D_overlay_AREA01_0082A7A0[];
extern int func_001B10B0(unsigned char *self, int a1, int a2);
extern void func_001C63E0(unsigned char *self, short a1);
extern void func_001BA8E0(unsigned char *self, int type);
extern void func_001BA580(unsigned char *self, unsigned char a1);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_001C68C0(unsigned char *self);
extern unsigned char func_001B17A0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA01_008254B0(unsigned char *self);
extern void func_overlay_AREA01_00825590(unsigned char *self);
extern void func_overlay_AREA01_00825670(unsigned char *self);

void func_overlay_AREA01_00825310(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (D_0081075A != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x4A);
        func_001C63E0(self, 1);
        func_001BA8E0(self, self[0xD]);
        self[4] = 1;
        self[0] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA01_0082A7A0;
        *(int *)(self + 0x58) = D_0028A5C4;
        break;
    case 1:
        switch (D_008107D9) {
        case 0:
            func_overlay_AREA01_008254B0(self);
            break;
        case 0x80:
            func_overlay_AREA01_00825590(self);
            break;
        case 0x81:
            func_overlay_AREA01_00825670(self);
            break;
        }
        func_001BA580(self, self[0xD]);
        func_001C64F0(self, 1.0f);
        func_001C68C0(self);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
        break;
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
