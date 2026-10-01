// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00827400 (splat/link name 008273C0; overlay code is
// linked 0x40 below where it runs), 0x84 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Covers the splat pieces 008273C0, 00827400 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: at runtime 0x827400 (called from the gun): spawns a class 0xC
// child: +0xB0 = matrix +0x30, +0xD0 = the matrix, +0x100 = the point
// 0x82A740 through that matrix, behaviour func_001F5040 (same code as AREA01
// 0x8287C0).
typedef struct { float x, y, z, w; } Vec4;
extern char *func_001AFA90(int cls);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern Vec4 D_overlay_AREA11_0082A740;
extern char D_001F5040[];

void func_overlay_AREA11_008273C0(char *src) {
    char *o = func_001AFA90(0xC);
    if (o != 0) {
        Vec4 v = D_overlay_AREA11_0082A740;
        func_00102948(o + 0xB0, src + 0x30);
        func_00102958(o + 0xD0, src);
        func_001026A0(o + 0x100, o + 0xD0, &v);
        *(char **)(o + 0x10) = D_001F5040;
    }
}
