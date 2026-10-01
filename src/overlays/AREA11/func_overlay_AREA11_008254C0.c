// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00825500 (splat/link name 008254C0; overlay code is
// linked 0x40 below where it runs), 0xF4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Covers the splat pieces 008254C0, 00825500 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-behaviour at runtime 0x825500 (D_00810813 0/1). +5 0: with
// the player's y (D_00810354) in 260..280 and the player inside the area
// 0x82ABE0 (func_001B1EA0), starts script 0x8294C0 and +5 = 1. +5 1: at the
// script end D_00810813 = 0x10, func_001C4760(1, 1), +5 = 0.
extern unsigned char D_00810813;
extern float D_00810354;
extern char D_00810350[];
extern char D_overlay_AREA11_0082ABE0[];
extern char D_overlay_AREA11_008294C0[];
extern int func_001B1EA0(int a0, void *pos, void *poly, int n);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int a0, int a1);

void func_overlay_AREA11_008254C0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_00810354 >= 260.0f && D_00810354 <= 280.0f &&
            func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA11_0082ABE0, 4) != 0) {
            func_001BA1A0(talk, D_overlay_AREA11_008294C0);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            D_00810813 = 0x10;
            func_001C4760(1, 1);
            self[5] = 0;
        }
        break;
    }
}
