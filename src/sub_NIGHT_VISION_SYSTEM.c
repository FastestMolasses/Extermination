// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Night-vision overlay setup. Issues two display/effect-list registrations for
// the actor in arg0 at full intensity: func_001DCD40(arg0, 0x80B9E1A5, 1.0f)
// then func_001DCF40(arg0, 0x80B9E1A5, 1.0f). Builds a local color/style record
// { rgb=0xB9E1A5, alpha=0x80, flag=0 } and emits three HUD/text draw commands
// via func_001CBA50(mode=1, x, y, len, len, glyph_table, &style): one with the
// D_0026E6F0 table (len 0x10) and two with the D_0026E700/D_0026E718 tables
// (len 6) at the same baseline (a1=0x72D). Returns void.
//
// mwcc 2.3.3 (mwcps2-2.3.3-000906) byte-matches; pinned 991202 caps at 77.9%.
// The local color record { int rgb; unsigned char a; unsigned char b; } reproduces
// the sw 0xB9E1A5 + sb 0x80 + sb 0 stack pattern, and func_001CBA50's true
// 7-arg signature (a0..a3, t0, t1=table ptr, t2=&record) with t0 repeating the
// length argument matches the paddub t0,a3 register move. objdiff 100.0 vs expected.
typedef struct { int rgb; unsigned char a; unsigned char b; } Col;
extern int D_0026E6F0;
extern int D_0026E700;
extern int D_0026E718;
extern void func_001DCD40(int a0, int a1, float f);
extern void func_001DCF40(int a0, int a1, float f);
extern void func_001CBA50(int a0, int a1, int a2, int a3, int t0, void *t1, void *t2);

void sub_NIGHT_VISION_SYSTEM(int arg0) {
    Col c;
    func_001DCD40(arg0, 0x80B9E1A5, 1.0f);
    func_001DCF40(arg0, 0x80B9E1A5, 1.0f);
    c.rgb = 0xB9E1A5;
    c.a = 0x80;
    c.b = 0;
    func_001CBA50(1, 0x863, 0x843, 0x10, 0x10, &D_0026E6F0, &c);
    func_001CBA50(1, 0x72D, 0x7B3, 0x6, 0x6, &D_0026E700, &c);
    func_001CBA50(1, 0x72D, 0x7B6, 0x6, 0x6, &D_0026E718, &c);
}
