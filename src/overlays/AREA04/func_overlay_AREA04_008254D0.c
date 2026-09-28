// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00825510 (splat/link name 008254D0;
// overlay code is linked 0x40 below where it runs), 0x370 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: subtype +3 0x11 is a placed actor at (317, 14.9 or 55 when
// D_008107BD, 260); others get +0x30 = 0x82C1E0. State 0 func_001B0FD0,
// func_001C6380, func_001A2370(self, self + 0xD0), state 1, +0 = 1. State
// 1 for 0x11, sub-state +5: 0 waits for D_0081083D == 1; 1 starts script
// 0x82C060 (D_008107BD 0, sub 4) or 0x82C120 (sub 8) and falls into 4; 4/8
// at script end sub 0x10 and D_008107BD = 1 / 0, both refresh the model;
// 0x10 back to 0; then func_001B1B70, func_001C6380, the +0x4C method.
// Other subtypes draw func_001F4BF0 at (+0xB0, +0xB4, +0xB8 - 0.3) with
// color (0, 0x80, 0, 0x80) and run a script 0x82BD60 talk on +0xB bit 2.
// 2/3 func_001AFC10.
extern unsigned char D_008107BD;
extern unsigned char D_0081083D;
extern float D_700038A0[4];
extern int D_700038B0[4];
extern char D_overlay_AREA04_0082C1E0[];
extern char D_overlay_AREA04_0082C060[];
extern char D_overlay_AREA04_0082C120[];
extern char D_overlay_AREA04_0082BD60[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *m);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001F4BF0(float *pos, int *color);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_008254D0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (self[3] == 0x11) {
            if (D_008107BD == 0) {
                *(float *)(self + 0xB0) = 317.0f;
                *(float *)(self + 0xB4) = 14.9f;
                *(float *)(self + 0xB8) = 260.0f;
            } else {
                *(float *)(self + 0xB0) = 317.0f;
                *(float *)(self + 0xB4) = 55.0f;
                *(float *)(self + 0xB8) = 260.0f;
            }
        } else {
            *(char **)(self + 0x30) = D_overlay_AREA04_0082C1E0;
        }
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        if (self[3] == 0x11) {
            switch (self[5]) {
            case 0:
                if (D_0081083D == 1) {
                    self[5] = 1;
                }
                break;
            case 1:
                if (D_008107BD == 0) {
                    func_001BA1A0(blk, D_overlay_AREA04_0082C060);
                    self[5] = 4;
                } else {
                    func_001BA1A0(blk, D_overlay_AREA04_0082C120);
                    self[5] = 8;
                }
            case 4:
                if (func_001BA1F0(self) != 0) {
                    self[5] = 0x10;
                    D_008107BD = 1;
                }
                func_001C6380(self);
                func_001A2370(self, self + 0xD0);
                break;
            case 8:
                if (func_001BA1F0(self) != 0) {
                    self[5] = 0x10;
                    D_008107BD = 0;
                }
                func_001C6380(self);
                func_001A2370(self, self + 0xD0);
                break;
            case 0x10:
                self[5] = 0;
                break;
            }
            func_001B1B70(self);
            func_001C6380(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        } else {
            D_700038A0[0] = *(float *)(self + 0xB0);
            D_700038A0[1] = *(float *)(self + 0xB4);
            D_700038A0[2] = *(float *)(self + 0xB8) - 0.3f;
            D_700038A0[3] = 1.0f;
            D_700038B0[0] = 0;
            D_700038B0[1] = 0x80;
            D_700038B0[2] = 0;
            D_700038B0[3] = 0x80;
            func_001F4BF0(D_700038A0, D_700038B0);
            switch (self[5]) {
            case 0:
                if (self[0xB] & 4) {
                    func_001BA1A0(blk, D_overlay_AREA04_0082BD60);
                    self[5] = 1;
                }
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    self[0xB] = 0;
                    self[5] = 0;
                }
                break;
            }
            func_001B17A0(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
