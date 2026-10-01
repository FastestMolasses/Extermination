// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00823E80 (splat/link name 00823E40; overlay code is
// linked 0x40 below where it runs), 0x168 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: state 0: once func_001B0FD0 is clear, func_001C6380, state 1 and
// +0 = 1. State 1 while func_001BA1C0(self, 0x39) is clear: +5 0 starts
// script 0x828FC0 and func_001FABB0, +5 = 1; +5 1 at the script end sets
// +0x2E = 0xFFFF, D_00810811 = 0xFF, func_001C4760(0, 1), func_001FAE70(0),
// +5 = 2 and func_001AEE10(4, 0); +5 2 idles. Then func_001B1B70 and the
// +0x4C method. States 2/3: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810811;
extern char D_overlay_AREA11_00828FC0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001FABB0(void);
extern void func_001AEE10(int a0, int a1);
extern void func_001C4760(int a0, int a1);
extern void func_001FAE70(int a0);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA11_00823E40(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        if (func_001BA1C0(self, 0x39) == 0) {
            switch (self[5]) {
            case 0:
                func_001BA1A0(talk, D_overlay_AREA11_00828FC0);
                func_001FABB0();
                self[5] = 1;
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    *(unsigned short *)(self + 0x2E) = 0xFFFF;
                    D_00810811 = 0xFF;
                    func_001C4760(0, 1);
                    func_001FAE70(0);
                    self[5] = 2;
                    func_001AEE10(4, 0);
                }
                break;
            case 2:
                break;
            }
        }
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
