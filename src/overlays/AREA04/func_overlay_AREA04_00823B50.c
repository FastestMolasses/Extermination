// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00823B90 (splat/link name 00823B50;
// overlay code is linked 0x40 below where it runs), 0x344 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0: with D_00810845 bit 5 set or func_001BA1C0(self, 0xC)
// true, D_00810764 = 0xFF and state 3; otherwise starts script 0x8275A0
// (plus func_001FABB0) when D_008107E4 is 0, else 0x8278D0, and advances.
// State 1 by D_008107E4: 0 (func_001B1E20(2, 0xF0) while D_70003B8D and
// talk +0xC == 1 and D_70003B84 == 0xAA) at script end sets +0x2E =
// 0xFFFF, places the player at (440.1, 14.9, 356.4) facing pi
// (func_001B6F80), starts 0x8278D0, func_001B6660(0x8268D0),
// func_001FB0B0(0xF); 1/2 sub-state +5: 0 bit 5 -> +0 = 2, D_00810764 =
// 0xFF, state 3; else player +0xA4 <= 16 and func_001B1EA0(0, player +
// 0xB0, 0x827CD0, 4) -> sub 1, D_008107E4 = 2, func_001FABB0,
// D_70003B84 = 0; 1 at script end places the player at (440.4, 14.9,
// 114.4) facing 0, +0x2E = 0xFFFF, state 3, func_001C4760(6, 1),
// func_001FAE70(0). State 3 func_001AFC10.
extern char D_008102B0[];
extern unsigned char D_00810845;
extern unsigned char D_00810764;
extern unsigned char D_008107E4;
extern unsigned char D_70003B8D;
extern unsigned short D_70003B84;
extern float D_700038A0[4];
extern char D_overlay_AREA04_008275A0[];
extern char D_overlay_AREA04_008278D0[];
extern char D_overlay_AREA04_008268D0[];
extern char D_overlay_AREA04_00827CD0[];
extern int func_001BA1C0(unsigned char *self, int n);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FABB0(void);
extern void func_001B1E20(int a, int b);
extern void func_001B6F80(float *pos, float yaw);
extern void func_001B6660(char *p);
extern void func_001FB0B0(int n);
extern int func_001B1EA0(int a, void *pos, void *box, int n);
extern void func_001C4760(int a, int b);
extern void func_001FAE70(int a);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00823B50(unsigned char *self) {
    char *pl = D_008102B0;
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if ((D_00810845 & 0x20) || func_001BA1C0(self, 0xC) != 0) {
            D_00810764 = 0xFF;
            self[4] = 3;
        } else {
            if (D_008107E4 == 0) {
                func_001BA1A0(blk, D_overlay_AREA04_008275A0);
                func_001FABB0();
            } else {
                func_001BA1A0(blk, D_overlay_AREA04_008278D0);
            }
            self[4] = self[4] + 1;
        }
        break;
    case 1:
        switch (D_008107E4) {
        case 0:
            if (D_70003B8D != 0) {
                if ((signed char)blk[0xC] == 1 && D_70003B84 == 0xAA) {
                    func_001B1E20(2, 0xF0);
                }
            }
            if (func_001BA1F0(self) != 0) {
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                *(float *)0x700038A0 = 440.1f;
                *(float *)0x700038A4 = 14.9f;
                *(float *)0x700038A8 = 356.4f;
                *(float *)0x700038AC = 1.0f;
                func_001B6F80(D_700038A0, 3.1415927f);
                func_001BA1A0(blk, D_overlay_AREA04_008278D0);
                func_001B6660(D_overlay_AREA04_008268D0);
                func_001FB0B0(0xF);
            }
            break;
        case 1:
        case 2:
            switch (self[5]) {
            case 0:
                if (D_00810845 & 0x20) {
                    self[0] = 2;
                    D_00810764 = 0xFF;
                    self[4] = 3;
                } else if (*(float *)(pl + 0xA4) <= 16.0f) {
                    if (func_001B1EA0(0, pl + 0xB0, D_overlay_AREA04_00827CD0, 4) != 0) {
                        self[5] = self[5] + 1;
                        D_008107E4 = 2;
                        func_001FABB0();
                        D_70003B84 = 0;
                    }
                }
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    *(float *)0x700038A0 = 440.4f;
                    *(float *)0x700038A4 = 14.9f;
                    *(float *)0x700038A8 = 114.4f;
                    *(float *)0x700038AC = 1.0f;
                    func_001B6F80(D_700038A0, 0.0f);
                    *(unsigned short *)(self + 0x2E) = 0xFFFF;
                    self[4] = 3;
                    func_001C4760(6, 1);
                    func_001FAE70(0);
                }
                break;
            }
            break;
        }
        break;
    case 2:
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
