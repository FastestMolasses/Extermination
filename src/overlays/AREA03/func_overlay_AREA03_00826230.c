// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x00826270 (splat/link name 00826230; overlay code
//  is linked 0x40 below where it runs), 0xB8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: script callback (script 0x8282D0, op09 record 0x828750). With
//  D_00275CA0 set: D_00810702 = 3 and D_00810354, D_008105D4 and D_008105E4
//  each + 104.5; otherwise D_00810702 = 2 and each - 104.5. Returns 1. (The
//  sub 0 spawn entries 2 and 3 differ only in y, by 104.5.)
extern int D_00275CA0;
extern unsigned char D_00810702[];
extern float D_00810350[];
extern float D_008105D0[];

int func_overlay_AREA03_00826230(void) {
    if (D_00275CA0 != 0) {
        D_00810350[1] += 104.5f;
        D_008105D0[1] += 104.5f;
        D_008105D0[5] += 104.5f;
        D_00810702[0] = 3;
    } else {
        D_00810350[1] -= 104.5f;
        D_008105D0[1] -= 104.5f;
        D_008105D0[5] -= 104.5f;
        D_00810702[0] = 2;
    }
    return 1;
}
