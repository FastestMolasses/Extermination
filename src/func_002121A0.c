// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// GS/GIF register-setup sequence for the SPR4 pager page (same idiom as
// func_002125B0/func_00210A00). Opens with the page tile token via
// func_0020A7A0, func_00207D00(1,0), then six fixed func_00207E40 blits,
// func_00207D00(1,3), and a seventh blit. Then: if arg0 is odd AND the
// main-loop counter (0x70003B64) has bit 0x10 set, OR arg0 is even, issues
// one more identical func_00207E40 blit; either way falls through to a
// shared "3" merge. Unless arg0==3, draws the reserve-ammo readout
// (D_00810CB4 formatted via func_001C5FB0 into the scratch string
// D_002862C0 via func_00123168, then blit via func_001CBA50(...,
// &D_002862C0, D_00265510) at 0x8AC/0x79F — D_002862C0 is the just-
// formatted digit string, D_00265510 the glyph/table pointer). Emits
// another fixed func_00207E40 blit, then draws the mag-pack readout
// (D_00810C63*30 rounds, same format/blit pipeline at 0x8AC/0x7B6), and
// the raw pack count (D_00810C63 itself, at 0x8AC/0x7CC). Closes with a
// final func_00207E40 blit.
extern void func_0020A7A0(unsigned long long a);
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern char *func_001C5FB0(int arg0, int arg1, int arg2);
extern void func_00123168(int a, int b);
extern void func_001CBA50(int a, int b, int c, int d, int e, void *f, void *g);
extern short D_00810CB4;
extern unsigned char D_00810C63;
extern char D_002862C0;
extern char D_00265510[];

void func_002121A0(int arg0) {
    char *s;
    int n;

    func_0020A7A0(0x200479459D422300ULL);
    func_00207D00(1, 0);
    func_00207E40(1, 0x7950, 0x7A90, 0x80, 0x80, 0x40808080, 0x20046785DD322160ULL);
    func_00207E40(1, 0x7800, 0x8300, 0x100, 0x80, 0x40808080, 0x200479C5E1422100ULL);
    func_00207E40(1, 0x74F0, 0x7A40, 0x40, 0x80, 0x80808080, 0x20047B45D9422320ULL);
    func_00207E40(1, 0x7E20, 0x7A20, 0x80, 0x80, 0x80808080, 0x20047B65DD4222C0ULL);
    func_00207E40(1, 0x7000, 0x7E80, 0x100, 0x80, 0x20808080, 0x200479E5E1422180ULL);
    func_00207E40(1, 0x8000, 0x7E80, 0x100, 0x80, 0x20808080, 0x200479E5E1422200ULL);
    func_00207D00(1, 3);
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080, 0x20047965DD422240ULL);

    if (arg0 & 1) {
        if (*(int *)0x70003B64 & 0x10) {
            func_00207E40(1, 0x87E0, 0x7960, 0x30, 0x30, 0x80808080, 0x20047BE5554223C8ULL);
        }
    } else {
        func_00207E40(1, 0x87E0, 0x7960, 0x30, 0x30, 0x80808080, 0x20047BE5554223C8ULL);
    }

    if (arg0 != 3) {
        s = func_001C5FB0(D_00810CB4, 4, 1);
        func_00123168((int)&D_002862C0, (int)s);
        func_001CBA50(1, 0x8AC, 0x79F, 0x10, 0x10, &D_002862C0, D_00265510);
    }

    func_00207E40(1, 0x8800, 0x7AE0, 0x80, 0x80, 0x80808080, 0x20047D45DD4222E0ULL);

    n = D_00810C63;
    s = func_001C5FB0(n * 30, 4, 1);
    func_00123168((int)&D_002862C0, (int)s);
    func_001CBA50(1, 0x8AC, 0x7B6, 0x10, 0x10, &D_002862C0, D_00265510);

    s = func_001C5FB0(n, 4, 1);
    func_00123168((int)&D_002862C0, (int)s);
    func_001CBA50(1, 0x8AC, 0x7CC, 0x10, 0x10, &D_002862C0, D_00265510);

    func_00207E40(1, 0x7100, 0x7900, 0x80, 0x40, 0x80808080, 0x200480A59D422340ULL);
}
