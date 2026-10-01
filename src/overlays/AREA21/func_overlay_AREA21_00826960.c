// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008269A0 (splat/link name 00826960; overlay code
//  is linked 0x40 below where it runs), 0x48 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: script callback (script 0x82BEC0, op09 record 0x82C380):
//  D_008106C0 = func_001B6660(group 0x82A5A0), D_0081080E = 0x10,
//  func_001FABB0(), func_001FA790(0, 0x11); returns 1.
extern int D_008106C0[];
extern unsigned char D_0081080E[];
extern char D_overlay_AREA21_0082A5A0[];
extern int func_001B6660(void *group);
extern void func_001FABB0(void);
extern void func_001FA790(int a0, int a1);

int func_overlay_AREA21_00826960(void) {
    D_008106C0[0] = func_001B6660(D_overlay_AREA21_0082A5A0);
    D_0081080E[0] = 0x10;
    func_001FABB0();
    func_001FA790(0, 0x11);
    return 1;
}
