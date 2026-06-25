// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD/overlay batch setup for actor arg0 (handle 0x80CDC355, color 0xCDC355).
// Copies four 0x40-byte constant GS-packet templates (D_00253290/D_002532D0/
// D_00253310/D_00253350) into four stack qword buffers, runs the scaled-init
// call func_001DCFF0(arg0, 0x80CDC355, 1.0f), appends four GS/AD register
// packets via func_00207F80(arg0, a,b,c,d, 0x80CDC355), emits four glyph/sprite
// draw commands via func_001CBA50 keyed on a small stack header struct
// {int tag=0xCDC355; u8 0x80; u8 0}, then submits the four buffers via
// func_002081A0(arg0, &buf, 2, 4, 0x80CDC355). Returns void.
//
// mwcc 2.3.3 byte-matches (objdiff 100.0); pinned 991202 caps at 75.7%.
// KEY (the func_001DFE70 / func_001CBA50 idiom, identical to matched siblings
// src/sub_TACTICAL_ADVANCED.c and src/sub_NIGHT_VISION_SYSTEM.c): each template
// copy must be a NAMED struct assignment (`Buf bN; bN = D_xxx;`) so mwcc emits
// one source base + one dest stack pointer and copies as four lq/sq like CW; and
// the func_001CBA50 7-arg form with the two trailing pointers (&D_xxx table then
// &hdr) reproduces CW's arg-materialization order (table-ptr t1 before stack-addr
// t2). A u128-array template or a per-call inline color record mis-schedules.
typedef struct { unsigned __int128 q0, q1, q2, q3; } Buf;
typedef struct { int tag; unsigned char b4; unsigned char b5; } Hdr;
extern Buf D_00253290, D_002532D0, D_00253310, D_00253350;
extern Hdr D_0026E780, D_0026E790, D_0026E798, D_0026E7A0;
extern void func_001DCFF0(int a, int b, float c);
extern void func_00207F80(int a, int b, int c, int d, int e, int f);
extern void func_001CBA50(int a, int b, int c, int d, int e, void *f, void *g);
extern void func_002081A0(int a, void *b, int c, int d, int e);

void func_001DC610(int arg0) {
    Buf b0, b1, b2, b3;
    Hdr hdr;

    b0 = D_00253290;
    b1 = D_002532D0;
    b2 = D_00253310;
    b3 = D_00253350;

    func_001DCFF0(arg0, 0x80CDC355, 1.0f);
    func_00207F80(arg0, 0x7190, 0x85D0, 0x7910, 0x8640, 0x80CDC355);
    func_00207F80(arg0, 0x86F0, 0x85D0, 0x8E70, 0x8640, 0x80CDC355);
    func_00207F80(arg0, 0x7FE0, 0x7FB0, 0x8010, 0x8020, 0x80CDC355);
    func_00207F80(arg0, 0x7F90, 0x7FE0, 0x8060, 0x7FF0, 0x80CDC355);

    hdr.tag = 0xCDC355;
    hdr.b4 = 0x80;
    hdr.b5 = 0;
    func_001CBA50(1, 0x714, 0x79C, 0x10, 0x10, &D_0026E780, &hdr);
    func_001CBA50(1, 0x778, 0x7B4, 0x6, 0x6, &D_0026E790, &hdr);
    func_001CBA50(1, 0x778, 0x7B7, 0x6, 0x6, &D_0026E798, &hdr);
    func_001CBA50(1, 0x778, 0x7BA, 0x6, 0x6, &D_0026E7A0, &hdr);

    func_002081A0(arg0, &b0, 2, 4, 0x80CDC355);
    func_002081A0(arg0, &b1, 2, 4, 0x80CDC355);
    func_002081A0(arg0, &b2, 2, 4, 0x80CDC355);
    func_002081A0(arg0, &b3, 2, 4, 0x80CDC355);
}
