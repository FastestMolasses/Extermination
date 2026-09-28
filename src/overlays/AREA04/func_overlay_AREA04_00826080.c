// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x008260C0 (splat/link name 00826080;
// overlay code is linked 0x40 below where it runs), 0x4F8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0 (func_001B0FD0 == 0): at (380, 14.9) with +0xC8 = 0 when
// D_0081083B, else +0x30 = 0x82C640; model refresh, state 4, +5 = 0, +0 =
// 1. State 4: +2 = 4 (D_0081083B) or 0x84; +0xB bit 2 sets the word at
// 0x82C724 = 0xF3 and starts script 0x82C650; at script end +5/+0xB = 0;
// func_001B1B70 and the +0x4C method. State 1: +0x28 = 700 then counts
// down, moving +0xB0 (and +0xB4, +0xC8) by fixed steps in bands of +0xB0
// (499/497/495/428/426/424/395; at or below 395 it snaps to 380, 14.9, 0);
// at 0 state 0x64. State 0x64 resets to (380, 14.9, 0), state 4. States
// 1/0x64 end with func_001B17A0 and the +0x4C method; others
// func_001AFC10.
extern unsigned char D_0081083B;
extern int D_overlay_AREA04_0082C724;
extern char D_overlay_AREA04_0082C640[];
extern char D_overlay_AREA04_0082C650[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *m);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00826080(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    float x;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            if (D_0081083B != 0) {
                *(float *)(self + 0xB0) = 380.0f;
                *(float *)(self + 0xB4) = 14.9f;
                *(float *)(self + 0xC8) = 0.0f;
            } else {
                *(char **)(self + 0x30) = D_overlay_AREA04_0082C640;
            }
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
            self[4] = 4;
            self[5] = 0;
            self[0] = 1;
        }
        break;
    case 4:
        switch (self[5]) {
        case 0:
            if (D_0081083B != 0) {
                self[2] = 4;
            } else {
                self[2] = 0x84;
            }
            if (self[0xB] & 4) {
                D_overlay_AREA04_0082C724 = 0xF3;
                func_001BA1A0(blk, D_overlay_AREA04_0082C650);
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
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 1:
        switch (self[5]) {
        case 0:
            self[5] = 1;
            *(short *)(self + 0x28) = 0x2BC;
            break;
        case 1:
            *(short *)(self + 0x28) -= 1;
            if (*(short *)(self + 0x28) > 0) {
                x = *(float *)(self + 0xB0);
                if (x > 499.0f) {
                    *(float *)(self + 0xB0) = x - 0.2f;
                } else if (x > 497.0f) {
                    *(float *)(self + 0xB0) = x - 0.2f;
                    *(float *)(self + 0xB4) -= 0.08944256f;
                    *(float *)(self + 0xC8) += 0.023182336f;
                } else if (x > 495.0f) {
                    *(float *)(self + 0xB0) = x - 0.17888552f;
                    *(float *)(self + 0xB4) -= 0.08944256f;
                    *(float *)(self + 0xC8) += 0.023182336f;
                } else if (x > 428.0f) {
                    *(float *)(self + 0xB0) = x - 0.17888552f;
                    *(float *)(self + 0xB4) -= 0.08944256f;
                } else if (x > 426.0f) {
                    *(float *)(self + 0xB0) = x - 0.17888552f;
                    *(float *)(self + 0xB4) -= 0.08944256f;
                    *(float *)(self + 0xC8) -= 0.023182336f;
                } else if (x > 424.0f) {
                    *(float *)(self + 0xB0) = x - 0.2f;
                    *(float *)(self + 0xC8) -= 0.023182336f;
                } else if (x > 395.0f) {
                    *(float *)(self + 0xB0) = x - 0.2f;
                } else {
                    *(float *)(self + 0xB0) = 380.0f;
                    *(float *)(self + 0xB4) = 14.9f;
                    *(float *)(self + 0xC8) = 0.0f;
                }
                func_001C6380(self);
                func_001A2370(self, self + 0xD0);
            } else {
                self[4] = 0x64;
            }
            break;
        }
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 0x64:
        *(float *)(self + 0xB0) = 380.0f;
        *(float *)(self + 0xB4) = 14.9f;
        *(float *)(self + 0xC8) = 0.0f;
        self[4] = 4;
        self[5] = 0;
        func_001C6380(self);
        func_001A2370(self, self + 0xD0);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
