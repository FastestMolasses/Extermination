// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00823D70 (splat/link name 00823D30; overlay
// code is linked 0x40 below where it runs), 0x2B0 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Role: behaviour by kind +0xD with state +4. 0: if D_00810761 is set and
//  the kind is 14, +4 = 3; otherwise (+5 = 1 when D_00810761 is set)
//  func_001B0FD0, +4 = 1, +0 = 1. 1: kind 14 with +0xB0 < -270: +4 = 3 when
//  D_008107E1 bit 2 is set, then func_001B1B70, func_001C6380 and the +0x4C
//  callback. Kind 14 with +0xB0 > 170: +5 0 animates and waits for bit 3
//  (+5 = 1, +0x28 = 5); +5 1 counts +0x28 down, at 0 +4 = 3 and
//  func_0019C6F0(0x1D, 1), (0x1E, 1), then func_001B1B70. Kind 15: +5 0
//  waits for bit 3, +5 1 animates. Kind 16: the same with bit 2.
//  2, 3: func_001AFC10.
// Matching: the flag bytes are literal addresses (their lui fills branch
//  slots); each block that holds a +5 switch ends in `return;` (with `break`
//  only, mwcc threads the switch exits past the join the original keeps).
typedef void (*ActorFn)(unsigned char *);
extern int func_0019C6F0(int a0, int a1);
extern int func_001B0FD0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA02_00823D30(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (*(unsigned char *)0x00810761 != 0) {
            if (self[0xD] == 0xE) {
                self[4] = 3;
                break;
            }
            self[5] = 1;
        }
        func_001B0FD0(self);
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        if (self[0xD] == 0xE && *(float *)(self + 0xB0) < -270.0f) {
            if (*(unsigned char *)0x008107E1 & 4) {
                self[4] = 3;
            }
            func_001B1B70(self);
            func_001C6380(self);
            (*(ActorFn *)(self + 0x4C))(self);
        } else if (self[0xD] == 0xE && *(float *)(self + 0xB0) > 170.0f) {
            switch (self[5]) {
            case 0:
                if (*(unsigned char *)0x008107E1 & 8) {
                    self[5] = 1;
                    *(short *)(self + 0x28) = 5;
                }
                func_001B1B70(self);
                func_001C6380(self);
                (*(ActorFn *)(self + 0x4C))(self);
                break;
            case 1:
                if (--*(short *)(self + 0x28) == 0) {
                    self[4] = 3;
                    func_0019C6F0(0x1D, 1);
                    func_0019C6F0(0x1E, 1);
                }
                func_001B1B70(self);
                break;
            }
            return;
        } else if (self[0xD] == 0xF) {
            switch (self[5]) {
            case 0:
                if (*(unsigned char *)0x008107E1 & 8) {
                    self[5] = 1;
                }
                break;
            case 1:
                func_001B1B70(self);
                func_001C6380(self);
                (*(ActorFn *)(self + 0x4C))(self);
                break;
            }
            return;
        } else if (self[0xD] == 0x10) {
            switch (self[5]) {
            case 0:
                if (*(unsigned char *)0x008107E1 & 4) {
                    self[5] = 1;
                }
                break;
            case 1:
                func_001B1B70(self);
                func_001C6380(self);
                (*(ActorFn *)(self + 0x4C))(self);
                break;
            }
            return;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
