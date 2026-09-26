// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825950 (splat/link name 00825910; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: sub-0 deferred group 0x828A00 owner (O2 manager, model ids
//  0x47/0x4B); dispatches 0x825BE0/0x825D30/0x825EA0/0x825F00/0x825FC0 on
//  D_008107DF.
extern unsigned char *D_008106C0;
extern unsigned char D_008107DF;
extern unsigned char D_00810845;
extern int D_0028A5C4;
extern int func_001B10B0(unsigned char *self, int a1, int a2);
extern int func_0019C6F0(int a0, int a1);
extern void func_001C63E0(unsigned char *self, short a1);
extern void func_001CA6F0(void *a0, unsigned char a1);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA01_00825BE0(unsigned char *self);
extern void func_overlay_AREA01_00825D30(unsigned char *self);
extern void func_overlay_AREA01_00825EA0(unsigned char *self);
extern void func_overlay_AREA01_00825F00(unsigned char *self);
extern void func_overlay_AREA01_00825FC0(unsigned char *self);

void func_overlay_AREA01_00825910(unsigned char *self) {
    int id;
    switch (self[4]) {
    case 0:
        if (self[0xD] == 0x47) {
            D_008106C0 = self;
            func_001B10B0(self, self[0xD], 0x4A);
            self[4] = 1;
            *(float *)(self + 0x38) = 1.0f;
            self[0] = 1;
            *(short *)(self + 0x28) = 0;
            *(short *)(self + 0x2A) = 0;
            *(int *)(self + 0x58) = D_0028A5C4;
            func_0019C6F0(0x29, 0);
            self[0x240] = 0;
        }
        id = self[0xD];
        if (id == 0x4B) {
            if (D_00810845 & 0x20) {
                self[4] = 3;
                break;
            }
            func_001B10B0(self, (unsigned char)id, 0x4C);
            func_001C63E0(self, 0);
            self[0] = 1;
            func_001CA6F0(self, 0);
            *(int *)(self + 0x58) = 0;
            func_0019C6F0(0x29, 1);
            self[4] = 1;
        }
        break;
    case 1:
        if (self[0xD] == 0x47) {
            D_008106C0 = self;
            switch (D_008107DF) {
            case 0:
            case 1:
                func_overlay_AREA01_00825BE0(self);
                break;
            case 2:
            case 0x10:
                func_overlay_AREA01_00825D30(self);
                break;
            case 0x40:
                func_overlay_AREA01_00825EA0(self);
                break;
            case 0x80:
                func_overlay_AREA01_00825F00(self);
                break;
            case 0xFF:
                break;
            }
        }
        if (self[0xD] == 0x4B) {
            switch (D_008107DF) {
            case 0:
            case 1:
            case 2:
            case 0x10:
            case 0x40:
            case 0x80:
                break;
            case 0xFF:
                func_overlay_AREA01_00825FC0(self);
                break;
            }
        }
        break;
    case 2:
    case 3:
        if (self[0xD] == 0x47) {
            D_008106C0 = 0;
        }
        func_001AFC10(self);
        break;
    }
}
