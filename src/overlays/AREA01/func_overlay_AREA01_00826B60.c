// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00826BA0 (splat/link name 00826B60; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: sub-1 placement owner; script 0x82BAD0 once D_00810794 == 1.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810794;
extern unsigned char D_00810814;
extern char D_overlay_AREA01_0082BAD0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern int func_001C4760(int a0, int a1);
extern void func_001FABB0(void);
extern void func_001FB0B0(int a0);
extern unsigned char func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA01_00826B60(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    int v;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            v = D_00810794;
            if (v != 0xFF && v == 1) {
                func_001BA1A0(talk, D_overlay_AREA01_0082BAD0);
                self[5] = 1;
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                D_00810814 = 0xFF;
                func_001C4760(0x1B, 1);
                func_001FABB0();
                func_001FB0B0(0x14);
                self[5] = 2;
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
