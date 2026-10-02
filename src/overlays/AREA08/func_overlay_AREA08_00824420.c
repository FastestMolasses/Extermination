// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x00824460 (splat/link name 00824420; overlay code
//  is linked 0x40 below where it runs), 0x2C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Role: script callback (script 0x825AA0, op09 record 0x825B60):
//  func_001FABB0(), func_001FA790(0, 0x11); returns 1.
extern void func_001FABB0(void);
extern void func_001FA790(int a, int b);

int func_overlay_AREA08_00824420(void) {
    func_001FABB0();
    func_001FA790(0, 0x11);
    return 1;
}
