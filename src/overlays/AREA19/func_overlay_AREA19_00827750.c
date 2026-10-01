// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00827790 (splat/link name 00827750; overlay code
//  is linked 0x40 below where it runs), 0x250 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 0 placement [10]: two script chains by D_0081081D. 0: script
//  0x82D990 when D_00810702 == 0xD, then script 0x82DA10 after 366 frames
//  (func_001831F0(2) every frame in between), then func_001C4760(0xC, 1) and
//  D_0081081D = 0x80. 0x80: script 0x82DD10 when the player is in the area
//  0x82E050; re-armed 600 frames after leaving it.
typedef struct { float v[16]; } Poly __attribute__((aligned(16)));
extern Poly D_overlay_AREA19_0082E050;
extern char D_overlay_AREA19_0082D990[];
extern char D_overlay_AREA19_0082DA10[];
extern char D_overlay_AREA19_0082DD10[];
extern float D_00810350[];
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001831F0(int mode);
extern void func_001C4760(int id, int on);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00827750(unsigned char *self) {
    Poly area = D_overlay_AREA19_0082E050;
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (*(unsigned char *)0x81081D) {
        case 0:
            switch (self[5]) {
            case 0:
                if (*(unsigned char *)0x810702 == 0xD) {
                    func_001BA1A0(talk, D_overlay_AREA19_0082D990);
                    self[5] = 1;
                    *(short *)(self + 0x28) = 0;
                }
                return;
            case 1:
                func_001BA1F0(self);
                func_001831F0(2);
                *(short *)(self + 0x28) += 1;
                if (*(short *)(self + 0x28) > 0x16D) {
                    func_001BA1A0(talk, D_overlay_AREA19_0082DA10);
                    self[5] = 2;
                }
                return;
            case 2:
                if (func_001BA1F0(self) != 0) {
                    func_001C4760(0xC, 1);
                    self[5] = 0;
                    *(unsigned char *)0x81081D = 0x80;
                }
                return;
            }
            break;
        case 0x80:
            switch (self[5]) {
            case 0:
                if (func_001B1EA0(0, D_00810350, &area, 4) == 1) {
                    func_001BA1A0(talk, D_overlay_AREA19_0082DD10);
                    self[5] = 1;
                }
                return;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    *(short *)(self + 0x2A) = 0;
                    self[5] = 2;
                }
                return;
            case 2:
                if (func_001B1EA0(0, D_00810350, &area, 4) == 0) {
                    self[5] = 3;
                }
                return;
            case 3:
                *(short *)(self + 0x2A) += 1;
                if (*(short *)(self + 0x2A) > 0x258) {
                    self[5] = 0;
                }
                return;
            }
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
