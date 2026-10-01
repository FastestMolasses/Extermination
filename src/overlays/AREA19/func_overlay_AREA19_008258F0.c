// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00825930 (splat/link name 008258F0; overlay code
//  is linked 0x40 below where it runs), 0x174 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Covers the splat pieces 008258F0, 00825930 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-state of 0x8257C0 (D_008107F6 != 0). +5 0 on +0xB bit 2: +5 = 1,
//  script 0x82BD90 and func_001B6F00(self, (0, 0, 5.5, 1), pi). +5 1:
//  func_001C64F0; at D_008107F6 == 4 func_001E8B40(1), func_001FBD50(self,
//  0x8DF, 0, 1000.0) and D_008107F6 += 1; at 0xFF D_00810854 |= 4; +5 = 2
//  when the script ends.
extern float D_700038A0[];
extern unsigned char D_00810854;
extern char D_overlay_AREA19_0082BD90[];
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B6F00(unsigned char *self, void *v, float a);
extern void func_001C64F0(unsigned char *self, float step);
extern void func_001E8B40(int a);
extern void func_001FBD50(unsigned char *self, int id, int a2, float vol);

void func_overlay_AREA19_008258F0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (self[0xB] & 4) {
            self[5] = 1;
            func_001BA1A0(talk, D_overlay_AREA19_0082BD90);
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = 0.0f;
            *(float *)0x700038A8 = 5.5f;
            *(float *)0x700038AC = 1.0f;
            func_001B6F00(self, D_700038A0, 3.1415927f);
        }
        break;
    case 1:
        func_001C64F0(self, 1.0f);
        switch (*(unsigned char *)0x8107F6) {
        case 1:
        case 2:
            break;
        case 3:
            break;
        case 4:
            func_001E8B40(1);
            func_001FBD50(self, 0x8DF, 0, 1000.0f);
            *(unsigned char *)0x8107F6 += 1;
            break;
        case 0xFF:
            D_00810854 |= 4;
            break;
        }
        if (func_001BA1F0(self) != 0) {
            self[5] = 2;
        }
        break;
    }
}
