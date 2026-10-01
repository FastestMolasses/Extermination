// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00827250 (splat/link name 00827210; overlay code
//  is linked 0x40 below where it runs), 0x6C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: script callback (script 0x82CBD0, op09 record 0x82CDD0): D_00810858
//  = D_008104D0 = 100, D_0081085C = D_008104D8 = 0,
//  func_0015C700 / func_0015C750 / func_001B81D0(D_008102B0), D_00810856 =
//  0xFF; returns 1.
extern float D_00810850[];
extern float D_008104D0[];
extern unsigned char D_00810856[];
extern char D_008102B0[];
extern void func_0015C700(void *p);
extern void func_0015C750(void *p);
extern void func_001B81D0(void *p);

int func_overlay_AREA21_00827210(void) {
    D_00810850[2] = 100.0f;
    D_008104D0[0] = 100.0f;
    D_00810850[3] = 0.0f;
    D_008104D0[2] = 0.0f;
    func_0015C700(D_008102B0);
    func_0015C750(D_008102B0);
    func_001B81D0(D_008102B0);
    D_00810856[0] = 0xFF;
    return 1;
}
