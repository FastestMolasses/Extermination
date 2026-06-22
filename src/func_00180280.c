// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD/dispatch helper: selects one of four event codes via func_001749A0,
// keyed on the arg1 flag (0 -> codes 0xA1/0xA3, else -> 0xA2/0xA4) and the
// byte at arg0+0x2F1 (0 -> low code, nonzero -> high code). a2 is always 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the two `b` epilogue branches' delay slots diverge (wall #13-class
// branch-delay fill, 93.3%). 2.3.3 leaves them as the target does -> 100%.
extern void func_001749A0(int, int, int);

void func_00180280(unsigned char *arg0, int arg1) {
    if (arg1 == 0) {
        if (arg0[0x2F1] == 0) {
            func_001749A0((int)arg0, 0xA1, 0);
        } else {
            func_001749A0((int)arg0, 0xA3, 0);
        }
    } else {
        if (arg0[0x2F1] == 0) {
            func_001749A0((int)arg0, 0xA2, 0);
        } else {
            func_001749A0((int)arg0, 0xA4, 0);
        }
    }
}
