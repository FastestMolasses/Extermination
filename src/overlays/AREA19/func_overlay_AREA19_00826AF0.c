// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00826B30 (splat/link name 00826AF0; overlay code
//  is linked 0x40 below where it runs), 0xE0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: script callback (script 0x82D290, record 0x82D490): three
//  func_001EFD90(0x80000015, ..) at (855.9, 370, 863), (850.9, 370, 856),
//  (863.9, 370, 876.3); returns 1.
extern void func_001EFD90(unsigned int msg, float *pos, void *m);
extern float D_700038A0[4];

int func_overlay_AREA19_00826AF0(unsigned char *self) {
    D_700038A0[3] = 1.0f;
    D_700038A0[1] = 370.0f;
    D_700038A0[0] = 855.9f;
    D_700038A0[2] = 863.0f;
    func_001EFD90(0x80000015, D_700038A0, self + 0xC0);
    D_700038A0[0] = 850.9f;
    D_700038A0[2] = 856.0f;
    func_001EFD90(0x80000015, D_700038A0, self + 0xC0);
    D_700038A0[0] = 863.9f;
    D_700038A0[2] = 876.3f;
    func_001EFD90(0x80000015, D_700038A0, self + 0xC0);
    return 1;
}
