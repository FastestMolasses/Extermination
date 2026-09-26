// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00823900 (splat/link name 008238C0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: spawned by the 0x825950 manager (group 0x82A900); state 1 transforms
//  (0,0,1,1) by the matrix at +0xD0 and emits events 0x80000003/0x80000024 at
//  +0x100.
typedef struct { float x, y, z, w; } Vec4;
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern void func_001EFD90(int id, void *a, void *b);
extern void func_001EFD20(int id, void *a);

void func_overlay_AREA01_008238C0(unsigned char *self) {
    Vec4 v;
    switch (self[4]) {
    case 0:
        break;
    case 1:
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 1.0f;
        v.w = 1.0f;
        func_001026A0(&v, self + 0xD0, &v);
        func_001028D0(&v, &v, self + 0x100);
        func_00102760(&v, &v);
        func_001EFD90(0x80000003, self + 0x100, &v);
        func_001EFD20(0x80000024, self + 0x100);
        break;
    case 2:
        break;
    case 3:
        break;
    }
}
