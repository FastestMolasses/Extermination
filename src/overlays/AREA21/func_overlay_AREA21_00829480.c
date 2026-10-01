// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008294C0 (splat/link name 00829480; overlay code
//  is linked 0x40 below where it runs), 0xD0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 00829480, 008294C0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8297C0 with (self, D_008102B0): places the object
//  at (0, 1, 8) from self (rotated by +0xC4, func_00182F90) and turns it to
//  +0xC4 + pi.
#define SPF(a) (*(float *)(a))
extern float D_70003400[16];
extern float D_70003430[4];
extern float D_70003600[4];
extern void func_001029C0(void *m);
extern void func_00102BB0(void *dst, void *src, float a);
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_00182F90(unsigned char *o, void *v);
extern float func_001B1470(float a);

void func_overlay_AREA21_00829480(unsigned char *self, unsigned char *o) {
    SPF(0x70003600) = 0.0f;
    SPF(0x70003604) = 1.0f;
    SPF(0x70003608) = 8.0f;
    SPF(0x7000360C) = 1.0f;
    func_001029C0(D_70003400);
    func_00102BB0(D_70003400, D_70003400, *(float *)(self + 0xC4));
    func_00102958(D_70003430, self + 0xB0);
    func_001026A0(D_70003600, D_70003400, D_70003600);
    func_00182F90(o, D_70003600);
    *(float *)(o + 0xC4) = func_001B1470(3.1415927f + *(float *)(self + 0xC4));
}
