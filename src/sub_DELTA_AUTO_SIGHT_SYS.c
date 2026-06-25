// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD overlay setup for actor arg0 (handle 0x801996FF, color 0x1996FF). Runs the
// scaled-init call func_001DCFF0(arg0, 0x801996FF, 1.0f), appends three GS/AD
// register packets via func_00207F80(arg0, ..., 0x801996FF), then emits three
// glyph/sprite draw commands via func_001CBA50 keyed on a small stack header
// struct {int tag=0x1996FF; u8 0x80; u8 0}. Returns void.
//
// mwcc 2.3.3 byte-matches (objdiff 100.0); pinned 991202 caps at 71.2%.
// KEY: the func_001CBA50 7-arg form with the two trailing pointers (&D_xxx table
// then &hdr) plus the shared named header-struct local reproduces CW's arg
// materialization order (table-ptr t1 before stack-addr t2) -- identical idiom
// to matched siblings src/sub_NIGHT_VISION_SYSTEM.c / src/sub_TACTICAL_ADVANCED.c.
typedef struct { int tag; unsigned char b4; unsigned char b5; } Hdr;
extern Hdr D_0026E758, D_0026E760, D_0026E778;
extern void func_001DCFF0(int a, int b, float c);
extern void func_00207F80(int a, int b, int c, int d, int e, int f);
extern void func_001CBA50(int a, int b, int c, int d, int e, void *f, void *g);

void sub_DELTA_AUTO_SIGHT_SYS(int arg0) {
    Hdr hdr;

    func_001DCFF0(arg0, 0x801996FF, 1.0f);
    func_00207F80(arg0, 0x7BA0, 0x7A80, 0x8460, 0x7A90, 0x801996FF);
    func_00207F80(arg0, 0x7190, 0x7AF0, 0x8E70, 0x7B20, 0x801996FF);
    func_00207F80(arg0, 0x7190, 0x84C0, 0x8E70, 0x84F0, 0x801996FF);

    hdr.tag = 0x1996FF;
    hdr.b4 = 0x80;
    hdr.b5 = 0;
    func_001CBA50(1, 0x723, 0x7A6, 0x10, 0x10, &D_0026E758, &hdr);
    func_001CBA50(1, 0x868, 0x85D, 0x6, 0x6, &D_0026E760, &hdr);
    func_001CBA50(1, 0x868, 0x860, 0x6, 0x6, &D_0026E778, &hdr);
}
