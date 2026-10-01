// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008237B0 (splat/link name 00823770; overlay code
//  is linked 0x40 below where it runs), 0xA4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: group 0x82BA70 member (x12). State 1: effect 0 at +0x100
//  (func_001EFD20) when D_70003B68 is a multiple of k, k = 25 (65 while 600 <
//  func_001C6190(self) < 950).
extern int D_70003B68[];
extern int func_001C6190(unsigned char *self);
extern void func_001EFD20(int id, void *pos);

void func_overlay_AREA21_00823770(unsigned char *self) {
    int v;
    int k;
    switch (self[4]) {
    case 0:
        break;
    case 1:
        v = func_001C6190(self);
        k = 25;
        if (v > 600 && v < 950) {
            k = 65;
        }
        if (D_70003B68[0] % k == 0) {
            func_001EFD20(0, self + 0x100);
        }
        break;
    case 2:
    case 3:
        break;
    }
}
