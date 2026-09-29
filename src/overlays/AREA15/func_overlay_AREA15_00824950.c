// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824990 (splat/link name 00824950; overlay code is
// linked 0x40 below where it runs), 0x1B0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: state 0: state 3 unless func_001BA1C0(self, 0x30); +0xD 0x50 ->
//  func_001B10B0(.., 0x52), func_001C63E0(self, 5), func_001CA6F0(self, 2),
//  +0x58 = D_0028A5DC, +0x30 = 0x829260; 0x5A -> func_001B10B0(.., 0x5B),
//  func_001C63E0(self, 2), func_001CA6F0(self, 2), +0x58 = D_0028A600, +0x30 =
//  0x829270; func_001BA8E0, state 1, +0 = 1. State 1: state 3 when
//  func_001BA1C0(self, 0x2A) is set, else by D_008107FF 0 -> 0x824B40, 1 ->
//  0x824C90. States 2/3 func_001BA540 and func_001AFC10.
extern unsigned char D_008107FF;
extern int D_0028A5DC;
extern int D_0028A600;
extern char D_overlay_AREA15_00829260[];
extern char D_overlay_AREA15_00829270[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA15_00824B40(unsigned char *self);
extern void func_overlay_AREA15_00824C90(unsigned char *self);

void func_overlay_AREA15_00824950(unsigned char *self) {
    int id;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x30) == 0) {
            self[4] = 3;
            break;
        }
        id = self[0xD];
        if (id == 0x50) {
            func_001B10B0(self, (unsigned char)id, 0x52);
            func_001C63E0(self, 5);
            func_001CA6F0(self, 2);
            *(int *)(self + 0x58) = D_0028A5DC;
            *(char **)(self + 0x30) = D_overlay_AREA15_00829260;
        }
        id = self[0xD];
        if (id == 0x5A) {
            func_001B10B0(self, (unsigned char)id, 0x5B);
            func_001C63E0(self, 2);
            func_001CA6F0(self, 2);
            *(int *)(self + 0x58) = D_0028A600;
            *(char **)(self + 0x30) = D_overlay_AREA15_00829270;
        }
        func_001BA8E0(self, self[0xD]);
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        if (func_001BA1C0(self, 0x2A) != 0) {
            self[4] = 3;
            break;
        }
        switch (D_008107FF) {
        case 0:
            func_overlay_AREA15_00824B40(self);
            break;
        case 1:
            func_overlay_AREA15_00824C90(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
