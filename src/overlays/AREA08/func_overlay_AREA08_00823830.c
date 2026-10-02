// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x00823870 (splat/link name 00823830; overlay code
//  is linked 0x40 below where it runs), 0x250 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Role: door, sub 0 / 1 placement [6] (room move e7 / e6). State 0:
//  func_001BB520, +0x28 = +0x2A = 0, +0 = 1. State 1 by D_008107EF: 0 -> +5 0
//  calls 0x8237B0 at spawn entry 3 and opens through 0x823630 (+5 1), +5 1
//  closes at the script end (+5 = +0xB = 0); set -> the door steps
//  func_001BB560 / 001BB7C0 / 001BC150 / 001BB7F0 (+5 0..3). Then
//  func_001C6380, the +0x4C method and within 20 units of D_00810350 +1 = 1
//  (func_001B1DE0 when +2 bit 7). States 2 / 3 func_001AFC10.
extern unsigned char D_00810702;
extern unsigned char D_008107EF;
extern float D_00810350[];
extern void func_001BB520(unsigned char *self);
extern int func_001BB560(unsigned char *self, unsigned char *blk, int a2);
extern int func_001BB7C0(unsigned char *self);
extern void func_001BC150(unsigned char *self);
extern int func_001BB7F0(unsigned char *self);
extern int func_001BA1F0(unsigned char *self);
extern void func_overlay_AREA08_008237B0(unsigned char *self);
extern int func_overlay_AREA08_00823630(unsigned char *self, unsigned char *blk);
extern void func_001C6380(unsigned char *self);
extern float func_0011E748(float a);
extern void func_001B1DE0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA08_00823830(unsigned char *self) {
    unsigned char *blk;
    float dx, dy, dz;
    blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BB520(self);
        *(short *)(self + 0x28) = 0;
        *(short *)(self + 0x2A) = 0;
        self[0] = 1;
        break;
    case 1:
        switch (D_008107EF) {
        case 0:
            switch (self[5]) {
            case 0:
                if (D_00810702 == 3) {
                    func_overlay_AREA08_008237B0(self);
                }
                if (func_overlay_AREA08_00823630(self, blk) != 0) {
                    self[5] = self[5] + 1;
                }
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    self[5] = 0;
                    self[0xB] = 0;
                }
                break;
            }
            break;
        default:
            switch (self[5]) {
            case 0:
                if (func_001BB560(self, blk, 0) != 0) {
                    self[5] = self[5] + 1;
                }
                break;
            case 1:
                if (func_001BB7C0(self) != 0) {
                    self[5] = self[5] + 1;
                }
                break;
            case 2:
                func_001BC150(self);
                self[5] = self[5] + 1;
                break;
            case 3:
                if (func_001BB7F0(self) != 0) {
                    self[5] = 0;
                }
                break;
            }
        }
        func_001C6380(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        dx = D_00810350[0] - *(float *)(self + 0xB0);
        dy = D_00810350[1] - *(float *)(self + 0xB4);
        dz = D_00810350[2] - *(float *)(self + 0xB8);
        if (func_0011E748(dx * dx + dy * dy + dz * dz) <= 20.0f) {
            self[1] = 1;
            if (self[2] & 0x80) {
                func_001B1DE0(self);
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
