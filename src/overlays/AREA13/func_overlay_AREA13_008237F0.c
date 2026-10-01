// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823830 (splat/link name 008237F0; overlay code
//  is linked 0x40 below where it runs), 0x104 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 008237F0, 00823830 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-state of 0x823700 (D_008107F1 == 0). +5 0: when D_00810354 <= 170
//  and D_00810702 == 2: func_001AEDB0(0), func_001EFD20(7, scratch
//  0x700038A0), script 0x82A360 on the talk block (+0x1F0), +5 = 1,
//  D_00810771 = 1. +5 1: when the script ends (func_001BA1F0):
//  func_001C4760(9, 1), func_001C4760(0x4F, 1), +0x2E = 0xFFFF, D_008107F1 =
//  1, +5 = 0, func_001FAE70(0).
extern float D_700038A0[4];
extern float D_00810354;
extern char D_overlay_AREA13_0082A360[];
extern void func_001AEDB0(int a);
extern void func_001EFD20(int a, float *pos);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int id, int on);
extern void func_001FAE70(int a);

void func_overlay_AREA13_008237F0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_00810354 <= 170.0f && *(unsigned char *)0x810702 == 2) {
            func_001AEDB0(0);
            func_001EFD20(7, D_700038A0);
            func_001BA1A0(talk, D_overlay_AREA13_0082A360);
            self[5] = 1;
            *(unsigned char *)0x810771 = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            func_001C4760(9, 1);
            func_001C4760(0x4F, 1);
            *(unsigned short *)(self + 0x2E) = 0xFFFF;
            *(unsigned char *)0x8107F1 = 1;
            self[5] = 0;
            func_001FAE70(0);
        }
        break;
    }
}
