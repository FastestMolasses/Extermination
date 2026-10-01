// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00824F70 (splat/link name 00824F30; overlay code
//  is linked 0x40 below where it runs), 0xC0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: group 0x82B5A0 member: in state 1 copies its matrix to scratch
//  0x700036A0, scales its three axis rows by -1 and calls func_001EFEB0(5,
//  ..).
extern float D_700036A0[];
extern float D_700036B0[];
extern float D_700036C0[];
extern void func_00102958(void *dst, void *src);
extern void func_00103230(void *dst, void *src, float k);
extern void func_001EFEB0(unsigned int msg, void *m);

void func_overlay_AREA19_00824F30(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        func_00102958(D_700036A0, self + 0xD0);
        func_00103230(D_700036A0, D_700036A0, -1.0f);
        func_00103230(D_700036B0, D_700036B0, -1.0f);
        func_00103230(D_700036C0, D_700036C0, -1.0f);
        func_001EFEB0(5, D_700036A0);
        break;
    case 2:
    case 3:
        break;
    }
}
