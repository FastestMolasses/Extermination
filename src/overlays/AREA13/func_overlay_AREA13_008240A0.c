// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008240E0 (splat/link name 008240A0; overlay code
//  is linked 0x40 below where it runs), 0x7C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Covers the splat pieces 008240A0, 008240E0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: area test (called by 0x824390): the shape of 0x823FE0 with the area
//  0x82E140.
extern float D_00810350[];
extern char D_overlay_AREA13_0082E140[];
extern int func_001B1EA0(int a, void *b, void *c, int d);

int func_overlay_AREA13_008240A0(void) {
    if (*(unsigned char *)0x810774 != 0xFF &&
        func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA13_0082E140, 4) != 0 &&
        !(*(float *)0x810354 <= 210.0f)) {
        return 1;
    }
    return 0;
}
