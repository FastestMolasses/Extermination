// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00824CD0 (splat/link name 00824C90; overlay
// code is linked 0x40 below where it runs), 0x80 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 00824C90, 00824CD0 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: transforms the points 0x827590 and 0x8275A0 by bone +0x110[0] +
//  0x90 (func_001026A0) and draws the 0x824C40 sprite at both.
// Matching: the two results are one local array (&w[1] is kept in s0).
typedef struct {
    float v[4];
} Vec4 __attribute__((aligned(16)));
extern Vec4 D_overlay_AREA02_00827590;
extern Vec4 D_overlay_AREA02_008275A0;
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_overlay_AREA02_00824C40(void *pos);

void func_overlay_AREA02_00824C90(unsigned char *self) {
    Vec4 a = D_overlay_AREA02_00827590;
    Vec4 b = D_overlay_AREA02_008275A0;
    Vec4 w[2];
    Vec4 *q;

    func_001026A0(&w[0], *(char **)(self + 0x110) + 0x90, &a);
    q = &w[1];
    func_001026A0(q, *(char **)(self + 0x110) + 0x90, &b);
    func_overlay_AREA02_00824C40(&w[0]);
    func_overlay_AREA02_00824C40(q);
}
