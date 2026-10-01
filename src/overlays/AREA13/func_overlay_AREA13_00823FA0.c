// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823FE0 (splat/link name 00823FA0; overlay code
//  is linked 0x40 below where it runs), 0x7C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: area test: returns 1 when D_00810774 != 0xFF, func_001B1EA0(0,
//  D_00810350, 0x82E1C0, 4) is set and D_00810354 > 210. No static caller
//  (area_overview.py).
extern float D_00810350[];
extern char D_overlay_AREA13_0082E1C0[];
extern int func_001B1EA0(int a, void *b, void *c, int d);

int func_overlay_AREA13_00823FA0(void) {
    if (*(unsigned char *)0x810774 != 0xFF &&
        func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA13_0082E1C0, 4) != 0 &&
        !(*(float *)0x810354 <= 210.0f)) {
        return 1;
    }
    return 0;
}
