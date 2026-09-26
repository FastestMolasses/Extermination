// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x008287C0 (splat/link name 00828780; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: called from 0x826D40 (O4); spawns a class-0xC child at the owner
//  position with the colour 0x82CB30.
// Covers the splat pieces 00828780, 008287C0 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef struct { float x, y, z, w; } Vec4;
extern char *func_001AFA90(int cls);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern Vec4 D_overlay_AREA01_0082CB30;
extern char D_001F5040[];

void func_overlay_AREA01_00828780(char *src) {
    char *o = func_001AFA90(0xC);
    if (o != 0) {
        Vec4 v = D_overlay_AREA01_0082CB30;
        func_00102948(o + 0xB0, src + 0x30);
        func_00102958(o + 0xD0, src);
        func_001026A0(o + 0x100, o + 0xD0, &v);
        *(char **)(o + 0x10) = D_001F5040;
    }
}
