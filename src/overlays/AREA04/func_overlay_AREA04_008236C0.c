// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00823700 (splat/link name 008236C0;
// overlay code is linked 0x40 below where it runs), 0x220 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: door-shaped lifecycle (the boot func_001BC350 pattern). State 0
// func_001BB520, +0 = 1. State 1, sub-state +5: 0 tests the bit
// D_00810841[D_00810700] & (1 << +0x34): set -> func_001BB560(self, blk, 0)
// then sub 2; clear -> 0x823580 then sub 1; 1 func_001BB7C0 -> +0xB = 0,
// sub 0; 2 func_001BB7C0 -> sub 3; 3 func_001BC150 -> sub 4; 4
// func_001BB7F0 -> sub 0. Then func_001C6380, the +0x4C method, and within
// 20 units of D_00810350 +1 = 1 (func_001B1DE0 when +2 bit 7). States 2/3
// func_001AFC10.
extern unsigned char D_00810700;
extern unsigned char D_00810841[];
extern float D_00810350[];
extern void func_001BB520(unsigned char *self, unsigned char *blk);
extern int func_001BB560(unsigned char *self, unsigned char *blk, int a2);
extern int func_overlay_AREA04_00823580(unsigned char *self, unsigned char *blk);
extern int func_001BB7C0(unsigned char *self, unsigned char *blk);
extern void func_001BC150(unsigned char *self, unsigned char *blk);
extern int func_001BB7F0(unsigned char *self, unsigned char *blk);
extern void func_001C6380(unsigned char *self);
extern float func_0011E748(float a);
extern void func_001B1DE0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_008236C0(unsigned char *self) {
    unsigned char *blk;
    float dx, dy, dz;
    blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BB520(self, blk);
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810841[D_00810700] & (1U << *(short *)(self + 0x34))) {
                if (func_001BB560(self, blk, 0) != 0) {
                    self[5] = 2;
                }
            } else {
                if (func_overlay_AREA04_00823580(self, blk) != 0) {
                    self[5] = self[5] + 1;
                }
            }
            break;
        case 1:
            if (func_001BB7C0(self, blk) != 0) {
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        case 2:
            if (func_001BB7C0(self, blk) != 0) {
                self[5] = self[5] + 1;
            }
            break;
        case 3:
            func_001BC150(self, blk);
            self[5] = self[5] + 1;
            break;
        case 4:
            if (func_001BB7F0(self, blk) != 0) {
                self[5] = 0;
            }
            break;
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
