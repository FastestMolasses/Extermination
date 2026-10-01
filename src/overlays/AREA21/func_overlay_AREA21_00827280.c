// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA21 overlay, runtime 0x008272C0 (splat/link name 00827280; overlay code
//  is linked 0x40 below where it runs), 0xFC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [47]. State 1: the +0x74 of the object at
//  *(D_00275B40 + 4) turns by -pi/30 a frame (func_001B1470) and the one at
//  +8 follows it at -pi/60; func_001C6380, then the +0x4C method when
//  func_001B17A0 is set.
typedef void (*ActorFn)(unsigned char *);
#define REC(o) (*(unsigned char **)(D_00275B40 + (o)))
extern char *D_00275B40;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern float func_001B1470(float a);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00827280(unsigned char *self) {
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        func_001C6380(self);
        break;
    case 1:
        *(float *)(REC(4) + 0x74) = func_001B1470(*(float *)(REC(4) + 0x74) - 0.10471976f);
        *(float *)(REC(8) + 0x74) = func_001B1470(*(float *)(REC(4) + 0x74) - 0.05235988f);
        func_001C6380(self);
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
