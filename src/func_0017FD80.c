// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Dispatch: pick a value for func_001749A0's second arg based on `flag`
// and a status byte at p[0x2F1]. flag selects the 0xF8/0xF2 vs 0xFB/0xF5
// pair; the status byte selects within the pair (nonzero -> high value).
extern void func_001749A0(unsigned char *p, int a1, int a2);

void func_0017FD80(unsigned char *p, int flag) {
    if (flag == 0) {
        if (p[0x2F1] == 0) {
            func_001749A0(p, 0xF2, 0);
        } else {
            func_001749A0(p, 0xF8, 0);
        }
    } else {
        if (p[0x2F1] == 0) {
            func_001749A0(p, 0xF5, 0);
        } else {
            func_001749A0(p, 0xFB, 0);
        }
    }
}
