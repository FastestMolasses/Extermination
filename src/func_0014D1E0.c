// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Allocates an object with func_001AFA90(2); when it succeeds sets +3 = 8,
// +0xD = a2, the handler +0x10 = func_0014C220, and copies the vectors a0 / a1
// to +0xB0 / +0xC0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern char *func_001AFA90(int);
extern void func_00102948(void *dst, void *src);
extern void func_0014C220(void);

void func_0014D1E0(void *a0, void *a1, int a2) {
    char *s0;

    if ((s0 = func_001AFA90(2)) == 0) {
        return;
    }
    s0[3] = 8;
    s0[0xD] = a2;
    *(void (**)(void))(s0 + 0x10) = func_0014C220;
    func_00102948(s0 + 0xB0, a0);
    func_00102948(s0 + 0xC0, a1);
}
