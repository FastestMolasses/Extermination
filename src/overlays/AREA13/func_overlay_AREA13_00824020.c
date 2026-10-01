// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00824060 (splat/link name 00824020; overlay code
//  is linked 0x40 below where it runs), 0x7C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 00824020, 00824060 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: area test (called by 0x824390): the shape of 0x823FE0 with the area
//  0x82E180.
extern float D_00810350[];
extern char D_overlay_AREA13_0082E180[];
extern int func_001B1EA0(int a, void *b, void *c, int d);

int func_overlay_AREA13_00824020(void) {
    if (*(unsigned char *)0x810774 != 0xFF &&
        func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA13_0082E180, 4) != 0 &&
        !(*(float *)0x810354 <= 210.0f)) {
        return 1;
    }
    return 0;
}
