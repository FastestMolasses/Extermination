// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA14 overlay, runtime 0x00825FD0 (splat/link name 00825F90; overlay code
//  is linked 0x40 below where it runs), 0x1A8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA14; lane A03C).
// Role: sub 0 placement [6]. State 0 (after func_001B0FD0): func_001C6380,
//  +8 = 1, +0x30 = 0x8283A0, +0x2E = 0. State 1, +5 0: D_700038A0 = (221.6,
//  695.1, 1181, 1); +0 = 1 and func_00158590(self, 1, -2) when D_00810C8C is
//  set, else +0 = 2 and func_00158590(self, 0, -2); +0xB bit 2 starts script
//  0x828120 (+5 1). +5 1: +5 2 at the script end. Then func_001B17A0,
//  func_001C6380 and the +0x4C method. States 2 / 3: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern float D_700038A0[4];
extern unsigned char D_00810C8C[];
extern char D_overlay_AREA14_008283A0[];
extern char D_overlay_AREA14_00828120[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_00158590(unsigned char *self, int a1, int a2);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA14_00825F90(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[8] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA14_008283A0;
        *(short *)(self + 0x2E) = 0;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            D_700038A0[0] = 221.6f;
            D_700038A0[1] = 695.1f;
            D_700038A0[2] = 1181.0f;
            D_700038A0[3] = 1.0f;
            if (D_00810C8C[0] != 0) {
                self[0] = 1;
                func_00158590(self, 1, -2);
            } else {
                self[0] = 2;
                func_00158590(self, 0, -2);
            }
            if (self[0xB] & 4) {
                func_001BA1A0(talk, D_overlay_AREA14_00828120);
                self[5]++;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[5]++;
            }
            break;
        case 2:
            break;
        }
        func_001B17A0(self);
        func_001C6380(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
