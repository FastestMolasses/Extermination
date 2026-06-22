// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Picks one of two sound/event IDs (depending on the byte at p+0x2F1) and
// passes it to func_001749A0(p, id, 0). The `flag` argument selects between
// two ID pairs. Matches mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202
// build walls at 93.3% on the CW branch-lowering/clean-store delay-slot case.
extern void func_001749A0(int, int, int);

void func_0017FE80(unsigned char *p, int flag) {
    if (flag == 0) {
        if (*(unsigned char *)(p + 0x2F1) == 0)
            func_001749A0((int)p, 0xF4, 0);
        else
            func_001749A0((int)p, 0xFA, 0);
    } else {
        if (*(unsigned char *)(p + 0x2F1) == 0)
            func_001749A0((int)p, 0xF7, 0);
        else
            func_001749A0((int)p, 0xFD, 0);
    }
}
