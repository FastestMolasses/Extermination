// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Iterates the global object-pointer table (count D_00275B84, base D_00275B7C),
// gated by the short flag D_0028A9A0 (returns 0 immediately if set). For each
// active entry whose tag byte e[0]==1, dispatches on the subtype byte e[3]:
//   0x4B -> func_001AAA90(self,e,arg1,arg2), 0x4A -> func_001AA8D0(...).
// A nonzero callee result is returned immediately; otherwise scans to the end
// and returns 0. D_0028A9A0 is over-declared short[4] to force absolute
// addressing (idiom #20) while the gp-rel globals stay at -sdatathreshold 4.
extern short D_0028A9A0[4];
extern short D_00275B84;
extern unsigned char **D_00275B7C;
extern int func_001AA8D0(int a, unsigned char *b, int c, int d);
extern int func_001AAA90(int a, unsigned char *b, int c, int d);

int func_001AAC00(int a0, int a1, int a2) {
    int n;
    unsigned char **p;
    unsigned char *e;
    int r;

    if (D_0028A9A0[0] != 0) {
        return 0;
    }
    n = D_00275B84;
    p = D_00275B7C;
    while (n != 0) {
        e = *p;
        n--;
        p++;
        if (e[0] == 1) {
            switch (e[3]) {
            case 0x4A:
                r = func_001AA8D0(a0, e, a1, a2);
                if (r != 0) {
                    return r;
                }
                break;
            case 0x4B:
                r = func_001AAA90(a0, e, a1, a2);
                if (r != 0) {
                    return r;
                }
                break;
            }
        }
    }
    return 0;
}
