// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.83% NEARMISS).
// Fixes read from the original instructions:
//  - The 32-byte name buffer is the second half of a block based at D_0027B100
//    (as in func_00114F10): the copy targets D_0027B100 + 0x20, the terminator is
//    written at D_0027B100[0x3F], and the request records the block base itself.
//    The old C walked a pointer down from D_0027B120 instead, which reordered the
//    record store against the FlushCache argument setup.

extern char *func_00123418(char *dst, int a1, int a2);
extern int FlushCache(int ch);
extern int func_0010E8A8(int a0,int a1,int a2,int a3,int t0,int t1,int t2,int t3,int s0);
extern int D_00241D68;
extern int D_0027B0C0[];
extern char D_0027B170[];
extern char D_0027B100[];
extern int D_0027C680;

int func_00115068(int a0, int a1, int a2, int a3) {
    int s2 = a3;
    int s3v;
    char *s1;
    int v0 = D_00241D68;

    if (v0 != 0) {
        return v0;
    }
    s3v = (int)D_0027B0C0;
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }

    s1 = D_0027B170;
    *(int *)D_0027B170 = a0;
    *(int *)(s1 + 4) = a1;
    *(int *)(s1 + 8) = 0x10;
    func_00123418(s1 + 0x14, a2, 0x3FF);

    *(char *)(s1 + 0x413) = 0;
    func_00123418(D_0027B100 + 0x20, s2, 0x20);
    D_0027B100[0x3F] = 0;
    *(int *)(s1 + 0x10) = (int)D_0027B100;
    FlushCache(0);

    {
        int v1 = func_0010E8A8(s3v, 0xE, 1, (int)s1, 0x414, (int)&D_0027C680, 4, 0, 0);
        if (v1 == 0) {
            D_00241D68 = 0x13;
        }
        return v1;
    }
}
