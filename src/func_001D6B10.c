// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Returns func_001D6930(a0, a1, a2, a3, D_0026E510) after running
// func_001D1F20(a0).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern char D_0026E510[];
extern int func_001D6930(int a0, int a1, int a2, int a3, char *t0);
extern void func_001D1F20(int ch);

int func_001D6B10(int a0, int a1, int a2, int a3) {
    int r = func_001D6930(a0, a1, a2, a3, D_0026E510);
    func_001D1F20(a0);
    return r;
}
