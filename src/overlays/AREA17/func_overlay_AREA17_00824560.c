// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008245A0 (splat/link name 00824560; overlay code
//  is linked 0x40 below where it runs), 0x158 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [33] (flag 0x2E). State 1: +5 0 waits for flag 0x2D
//  (+5 1, skipping the frame's method); +5 1: when func_001B1EA0(0,
//  D_00810350, area 0x827530, 5) is 1, func_001FB0B0(0), D_00810786 = 1, +5
//  2. Then the +0x4C method when func_001B17A0. States 2 / 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
typedef struct {
    float v[20];
} Area __attribute__((aligned(16)));
extern Area D_overlay_AREA17_00827530;
extern float D_00810350[];
extern unsigned char D_00810786;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001FB0B0(int a);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00824560(unsigned char *self) {
    Area area = D_overlay_AREA17_00827530;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[0] = 1;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (func_001BA1C0(self, 0x2D) != 0) {
                self[5] = 1;
                return;
            }
            break;
        case 1:
            if (func_001B1EA0(0, D_00810350, &area, 5) == 1) {
                func_001FB0B0(0);
                D_00810786 = 1;
                self[5] = 2;
            }
            break;
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
