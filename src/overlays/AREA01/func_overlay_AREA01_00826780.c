// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x008267C0 (splat/link name 00826780; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: sub-0 placement owner (O1 suspension fixture); quad tests 0x82CCA0,
//  script 0x82B590.
typedef void (*ActorFn)(unsigned char *);
extern char D_00810350[];
extern char D_overlay_AREA01_0082CCA0[];
extern char D_overlay_AREA01_0082B590[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B1EA0(int a, void *b, void *c, int d);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA01_00826780(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[0] = 1;
        }
        break;
    case 1:
        if (func_001BA1C0(self, 0xF) == 0) {
            if (func_001BA1C0(self, 7) != 0) {
                switch (self[5]) {
                case 0:
                    /* the (void *) cast orders the argument set-up */
                    if (func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA01_0082CCA0, 4) != 0 &&
                        func_001B1EA0(2, D_00810350, (void *)D_overlay_AREA01_0082CCA0, 4) != 0) {
                        func_001BA1A0(talk, D_overlay_AREA01_0082B590);
                        self[5] = 1;
                    }
                    break;
                case 1:
                    if (func_001BA1F0(self) != 0) {
                        self[5] = 2;
                    }
                    break;
                case 2:
                    break;
                }
            }
        }
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
