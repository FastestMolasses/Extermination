// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x008256D0 (splat/link name 00825690; overlay code is
// linked 0x40 below where it runs), 0xD0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Covers the splat pieces 00825690, 008256D0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-behaviour at runtime 0x8256D0 (D_00810813 0x20). +5 0: with
// the player's y >= 285 and the player inside the area 0x82AC60, starts
// script 0x829CC0 and +5 = 1. +5 1: at the script end D_00810813 = 0xFF and
// +5 = 0.
extern unsigned char D_00810813;
extern float D_00810354;
extern char D_00810350[];
extern char D_overlay_AREA11_0082AC60[];
extern char D_overlay_AREA11_00829CC0[];
extern int func_001B1EA0(int a0, void *pos, void *poly, int n);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);

void func_overlay_AREA11_00825690(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_00810354 >= 285.0f &&
            func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA11_0082AC60, 4) != 0) {
            func_001BA1A0(talk, D_overlay_AREA11_00829CC0);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            D_00810813 = 0xFF;
            self[5] = 0;
        }
        break;
    }
}
