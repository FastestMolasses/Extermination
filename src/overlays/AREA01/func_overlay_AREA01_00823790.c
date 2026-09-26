// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x008237D0 (splat/link name 00823790; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: spawned by the 0x825950 manager (group 0x82A900); state 1 counts
//  +0x28 and every tenth frame spawns a class-0xC child at +0x100 and passes
//  a matrix to func_001F4010.
typedef struct { float x, y, z, w; } Vec4;
extern char *func_001AFA90(int cls);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001F4010(int idx, void *m);
extern Vec4 D_overlay_AREA01_00829DA0;
extern Vec4 D_overlay_AREA01_00829DB0;
extern char D_001F5040[];

void func_overlay_AREA01_00823790(unsigned char *self) {
    float m[4][4];
    Vec4 a;
    Vec4 b;
    char *o;
    a = D_overlay_AREA01_00829DA0;
    b = D_overlay_AREA01_00829DB0;
    switch (self[4]) {
    case 0:
        *(short *)(self + 0x28) = 0;
        break;
    case 1:
        *(short *)(self + 0x28) += 1;
        if (*(short *)(self + 0x28) % 10 == 0) {
            o = func_001AFA90(0xC);
            if (o != 0) {
                func_00102948(o + 0xB0, self + 0x100);
                func_00102958(o + 0xD0, self + 0xD0);
                func_001026A0(o + 0x100, o + 0xD0, &a);
                *(char **)(o + 0x10) = D_001F5040;
            }
            func_00102958(m, self + 0xD0);
            func_001026A0(m[3], o + 0xD0, &b);
            func_001F4010(3, m);
        }
        break;
    case 2:
        break;
    case 3:
        break;
    }
}
