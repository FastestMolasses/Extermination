// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823A40 (splat/link name 00823A00; overlay code
//  is linked 0x40 below where it runs), 0x174 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: sub 0 placement [4]. State 0: state 3 when func_001BA1C0(self, 0x1A)
//  is set, else func_001B1020(self, +0xD, -1, 0), +0 = 1. State 1 by +5: 0
//  starts script 0x82A770 (+5 = 1, +0x28 = 0) when D_00810354 <= 170 and
//  func_001B1EA0(0, D_00810350, 0x82E100, 4) is set; 1 sets state 3 when the
//  script ends. Then +1 = 1, func_001C6380 and the +0x4C method. States 2/3
//  func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern float D_00810354;
extern float D_00810350[];
extern char D_overlay_AREA13_0082E100[];
extern char D_overlay_AREA13_0082A770[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B1020(unsigned char *self, int id, int a2, int a3);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00823A00(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x1A) != 0) {
            self[4] = 3;
            break;
        }
        func_001B1020(self, self[0xD], -1, 0);
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810354 <= 170.0f &&
                func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA13_0082E100, 4) != 0) {
                func_001BA1A0(talk, D_overlay_AREA13_0082A770);
                self[5] = 1;
                *(short *)(self + 0x28) = 0;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[4] = 3;
            }
            break;
        case 2:
            break;
        }
        self[1] = 1;
        func_001C6380(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
