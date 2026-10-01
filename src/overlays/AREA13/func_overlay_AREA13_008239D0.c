// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00823A10 (splat/link name 008239D0; overlay code
//  is linked 0x40 below where it runs), 0x24 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: script callback (script 0x82A770, record 0x82AA30):
//  func_001C47A0(0x1A, 1); returns 1.
extern void func_001C47A0(int id, int on);

int func_overlay_AREA13_008239D0(void) {
    func_001C47A0(0x1A, 1);
    return 1;
}
