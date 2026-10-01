// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x008235A0 (splat/link name 00823560; overlay code
//  is linked 0x40 below where it runs), 0x1D8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: sub 1 placement [22]. State 0 (after func_001B0FD0): func_001C6380,
//  state 1, +0 = 1, +8 = 3, +0x30 = &D_00275900; +2 = 0x84 when D_00810781
//  is 0xFF. +5 0: script 0x827170 when D_00810781 == 1; on +0xB bit 2 with
//  D_00810781 == 0xFF script 0x827630, otherwise +0xB is cleared. +5 1: at
//  the script end, the first time (D_00810801 != 0xFF) func_001C47A0(0x2A, 1),
//  func_001C4760(0x12, 1), D_00810801 = 0xFF and +2 = 0x84; +0xB and +5 are
//  cleared. Then func_001B17A0 and the +0x4C method. States 2 / 3:
//  func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern int D_00275900;
extern unsigned char D_00810781[];
extern unsigned char D_00810801[];
extern char D_overlay_AREA03_00827170[];
extern char D_overlay_AREA03_00827630[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C47A0(int id, int on);
extern void func_001C4760(int id, int on);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA03_00823560(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[4] = 1;
        self[0] = 1;
        self[8] = 3;
        *(int **)(self + 0x30) = &D_00275900;
        if (D_00810781[0] == 0xFF) {
            self[2] = 0x84;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810781[0] == 1) {
                func_001BA1A0(talk, D_overlay_AREA03_00827170);
                self[5] = 1;
            }
            if ((self[0xB] & 4) && D_00810781[0] == 0xFF) {
                func_001BA1A0(talk, D_overlay_AREA03_00827630);
                self[5] = 1;
            } else {
                self[0xB] = 0;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                if (D_00810801[0] != 0xFF) {
                    func_001C47A0(0x2A, 1);
                    func_001C4760(0x12, 1);
                    D_00810801[0] = 0xFF;
                    self[2] = 0x84;
                    self[0xB] = 0;
                    self[5] = 0;
                } else {
                    self[0xB] = 0;
                    self[5] = 0;
                }
            }
            break;
        case 2:
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
