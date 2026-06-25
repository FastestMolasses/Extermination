// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD/tactical-overlay packet setup. Copies a 0x40-byte constant template
// (D_00253390) into a stack qword buffer, runs two scaled-init calls
// (func_001DCFF0 / func_001DCF40 with the 0x8073AA0A handle and scale 1.0f),
// appends 14 GS/AD register packets via func_00207F80(arg0, set, base, ad,
// extra, 0x8073AA0A), then emits 9 glyph/sprite entries via func_001CBA50 keyed
// on a small stack header struct {int tag; u8; u8;} (first 7 with tag 0x73AA0A,
// last 2 with tag 0xEBFFC8), and finally submits the buffer via
// func_002081A0(arg0, &buf, 2, 4, 0x8073AA0A). Returns void.
//
// mwcc 2.3.3 byte-matches; the pinned 991202 build caps at 73.0%. KEY (the
// func_001DFE70 idiom): the constant template copy must be a NAMED struct
// assignment (`Buf buf; buf = D_00253390;`) so mwcc materializes one source
// base pointer + one dest stack pointer and copies as four lq/sq, exactly like
// CW; a u128-array or value load mis-schedules. func_00207F80 takes 6 ints,
// func_001CBA50 takes 7 args (two trailing pointers), func_002081A0 takes 5.
// Verified objdiff 100.0%.
typedef struct { unsigned __int128 q0, q1, q2, q3; } Buf;
typedef struct { int tag; unsigned char b4; unsigned char b5; } Hdr;
extern Buf D_00253390;
extern Hdr D_0026E7A8, D_0026E7B0, D_0026E7C0, D_0026E7D0, D_0026E7E8, D_0026E7F8, D_0026E800, D_0026E808;
extern void func_001DCFF0(int a, int b, float c);
extern void func_001DCF40(int a, int b, float c);
extern void func_00207F80(int a, int b, int c, int d, int e, int f);
extern void func_001CBA50(int a, int b, int c, int d, int e, void *f, void *g);
extern void func_002081A0(int a, void *b, int c, int d, int e);

void sub_TACTICAL_ADVANCED(int arg0) {
    Buf buf;
    Hdr hdr;

    buf = D_00253390;

    func_001DCFF0(arg0, 0x8073AA0A, 1.0f);
    func_001DCF40(arg0, 0x8073AA0A, 1.0f);
    func_00207F80(arg0, 0x8630, 0x7A10, 0x8EA0, 0x7A40, 0x8073AA0A);
    func_00207F80(arg0, 0x7140, 0x7A40, 0x7B10, 0x7A50, 0x8073AA0A);
    func_00207F80(arg0, 0x7140, 0x7A60, 0x7B10, 0x7A90, 0x8073AA0A);
    func_00207F80(arg0, 0x7140, 0x7A90, 0x7160, 0x7AE0, 0x8073AA0A);
    func_00207F80(arg0, 0x72D0, 0x7A90, 0x72F0, 0x7AB0, 0x8073AA0A);
    func_00207F80(arg0, 0x7460, 0x7A90, 0x7480, 0x7AB0, 0x8073AA0A);
    func_00207F80(arg0, 0x75F0, 0x7A90, 0x7610, 0x7AB0, 0x8073AA0A);
    func_00207F80(arg0, 0x7780, 0x7A90, 0x77A0, 0x7AB0, 0x8073AA0A);
    func_00207F80(arg0, 0x7910, 0x7A90, 0x7930, 0x7AB0, 0x8073AA0A);
    func_00207F80(arg0, 0x7AF0, 0x7A90, 0x7B10, 0x7AE0, 0x8073AA0A);
    func_00207F80(arg0, 0x7140, 0x79A0, 0x7380, 0x7A30, 0x8073AA0A);
    func_00207F80(arg0, 0x78D0, 0x79A0, 0x7B10, 0x7A30, 0x8073AA0A);
    func_00207F80(arg0, 0x7FE0, 0x7FB0, 0x8010, 0x8020, 0x8073AA0A);
    func_00207F80(arg0, 0x7F90, 0x7FE0, 0x8060, 0x7FF0, 0x8073AA0A);

    hdr.tag = 0x73AA0A;
    hdr.b4 = 0x80;
    hdr.b5 = 0;
    func_001CBA50(1, 0x89B, 0x798, 0x10, 0x10, &D_0026E7A8, &hdr);
    func_001CBA50(1, 0x8A6, 0x7A7, 0x6, 0x6, &D_0026E7B0, &hdr);
    func_001CBA50(1, 0x8A6, 0x7AA, 0x6, 0x6, &D_0026E7C0, &hdr);
    func_001CBA50(1, 0x885, 0x85A, 0x6, 0x6, &D_0026E7D0, &hdr);
    func_001CBA50(1, 0x885, 0x85D, 0x6, 0x6, &D_0026E7E8, &hdr);
    func_001CBA50(1, 0x712, 0x7AE, 0x8, 0x8, &D_0026E7F8, &hdr);
    func_001CBA50(1, 0x791, 0x7AE, 0x8, 0x8, &D_0026E7A8, &hdr);

    hdr.tag = 0xEBFFC8;
    hdr.b4 = 0x80;
    hdr.b5 = 0;
    func_001CBA50(1, 0x714, 0x79A, 0x10, 0x10, &D_0026E800, &hdr);
    func_001CBA50(1, 0x78D, 0x79A, 0x10, 0x10, &D_0026E808, &hdr);

    func_002081A0(arg0, &buf, 2, 4, 0x8073AA0A);
}
