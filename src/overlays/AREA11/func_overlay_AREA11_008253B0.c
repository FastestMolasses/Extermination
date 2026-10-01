// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x008253F0 (splat/link name 008253B0; overlay code is
// linked 0x40 below where it runs), 0x10C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: state 0: inert (state 3) when func_001BA1C0(self, 0x3B), else +0
// = 1 and state 1. State 1 dispatches on D_00810813: 0 and 1 run 0x825500,
// 0x10 and 0x11 run 0x825600, 0x20 runs 0x8256D0. States 2/3:
// func_001AFC10.
extern unsigned char D_00810813;
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_overlay_AREA11_00825500(unsigned char *self);
extern void func_overlay_AREA11_00825600(unsigned char *self);
extern void func_overlay_AREA11_008256D0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA11_008253B0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x3B) != 0) {
            self[4] = 3;
            break;
        }
        self[0] = 1;
        self[4] = 1;
        break;
    case 1:
        switch (D_00810813) {
        case 0:
        case 1:
            func_overlay_AREA11_00825500(self);
            break;
        case 0x10:
        case 0x11:
            func_overlay_AREA11_00825600(self);
            break;
        case 0x20:
            func_overlay_AREA11_008256D0(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
