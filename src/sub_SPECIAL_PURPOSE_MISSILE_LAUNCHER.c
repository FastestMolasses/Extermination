// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD overlay setup for actor arg0 (handle 0x8091CDFF, color 0x91CDFF). Runs the
// scaled-init call func_001DCD40(arg0, 0x8091CDFF, 1.0f), appends two GS/AD
// register packets via func_00207F80(arg0, ..., 0x8091CDFF), then emits two
// glyph/sprite draw commands via func_001CBA50 keyed on a small stack header
// struct {int tag=0x91CDFF; u8 0x80; u8 0}. Returns void.
//
// mwcc 2.3.3 byte-matches (objdiff 100.0); pinned 991202 caps at 76.2%.
// KEY: the func_001CBA50 7-arg form with the two trailing pointers (&D_xxx table
// then &hdr) plus the shared named header-struct local reproduces CW's arg
// materialization order (table-ptr t1 before stack-addr t2) -- identical idiom
// to matched siblings src/sub_NIGHT_VISION_SYSTEM.c / src/sub_TACTICAL_ADVANCED.c.
typedef struct { int tag; unsigned char b4; unsigned char b5; } Hdr;
extern Hdr D_0026E720, D_0026E730;
extern void func_001DCD40(int a, int b, float c);
extern void func_00207F80(int a, int b, int c, int d, int e, int f);
extern void func_001CBA50(int a, int b, int c, int d, int e, void *f, void *g);

void sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER(int arg0) {
    Hdr hdr;

    func_001DCD40(arg0, 0x8091CDFF, 1.0f);
    func_00207F80(arg0, 0x7B40, 0x7BA0, 0x84C0, 0x7BD0, 0x8091CDFF);
    func_00207F80(arg0, 0x7B40, 0x8410, 0x84C0, 0x8430, 0x8091CDFF);

    hdr.tag = 0x91CDFF;
    hdr.b4 = 0x80;
    hdr.b5 = 0;
    func_001CBA50(1, 0x863, 0x84E, 0x10, 0x10, &D_0026E720, &hdr);
    func_001CBA50(1, 0x7A0, 0x7B1, 0x6, 0x6, &D_0026E730, &hdr);
}
