// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x00823780 (splat/link name 00823740; overlay code
//  is linked 0x40 below where it runs), 0xB4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Role: effect emitter (no static reference): state 1 effect 1 on +0xD0
//  (func_001EFEB0, the effect's +0x94 = own +0x94) when D_70003B68 is a
//  multiple of k, k = 5 (25 while 150 < func_001C6190(self) < 450); the
//  AREA21 0x8237B0 shape.
extern int D_70003B68[];
extern int func_001C6190(unsigned char *self);
extern unsigned char *func_001EFEB0(int id, void *m);

void func_overlay_AREA20_00823740(unsigned char *self) {
    int v;
    int k;
    unsigned char *o;
    switch (self[4]) {
    case 0:
        break;
    case 1:
        v = func_001C6190(self);
        k = 5;
        if (v > 150 && v < 450) {
            k = 25;
        }
        if (D_70003B68[0] % k == 0) {
            o = func_001EFEB0(1, self + 0xD0);
            if (o != 0) {
                *(short *)(o + 0x94) = *(short *)(self + 0x94);
            }
        }
        break;
    case 2:
    case 3:
        break;
    }
}
