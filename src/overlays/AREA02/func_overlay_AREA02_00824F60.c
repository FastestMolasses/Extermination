// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA02 overlay, runtime 0x00824FA0 (splat/link name 00824F60; overlay
// code is linked 0x40 below where it runs), 0x154 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Role: state +4. 0: once func_001B0FD0 returns 0: func_001C6380, +8 = 3,
//  +0x30 = &D_002758E0, +4 = 1. 1: +0 = 2 if func_001BA1C0(self, 9) else 1;
//  +5 0 waits for +0xB bit 2 (+5 = 1, script 0x827670), +5 1 clears +0xB and
//  +5 at script end; then func_001B17A0 and the +0x4C callback.
//  2, 3: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern int D_002758E0;
extern char D_overlay_AREA02_00827670[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA02_00824F60(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    int st;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[8] = 3;
            *(int **)(self + 0x30) = &D_002758E0;
            self[4] = 1;
        }
        break;
    case 1:
        if (func_001BA1C0(self, 9)) {
            self[0] = 2;
        } else {
            self[0] = 1;
        }
        st = self[5];
        switch (st) {
        case 0:
            if (self[0xB] & 4) {
                self[5] = st + 1;
                func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00827670);
            }
            break;
        case 1:
            if (func_001BA1F0(self)) {
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
