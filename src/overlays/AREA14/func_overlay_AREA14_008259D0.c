// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA14 overlay, runtime 0x00825A10 (splat/link name 008259D0; overlay code
//  is linked 0x40 below where it runs), 0x84 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA14; lane A03C).
// Covers the splat pieces 008259D0, 00825A10 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x823FC0; the same C as AREA19 0x829840 with the colour
//  vector 0x828110.
typedef struct { float x, y, z, w; } Vec4;
extern char *func_001AFA90(int cls);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern Vec4 D_overlay_AREA14_00828110;
extern char D_001F5040[];

void func_overlay_AREA14_008259D0(char *src) {
    char *o = func_001AFA90(0xC);
    if (o != 0) {
        Vec4 v = D_overlay_AREA14_00828110;
        func_00102948(o + 0xB0, src + 0x30);
        func_00102958(o + 0xD0, src);
        func_001026A0(o + 0x100, o + 0xD0, &v);
        *(char **)(o + 0x10) = D_001F5040;
    }
}
