// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// State-machine dispatch on the mode byte at +9 of the task slot (*(void**)0x70003B6C).
// case 0: bump the mode byte (+9), set gp-rel flag D_00275BD8 = 1, and kick
//   func_001FF080(0, 3, &slot[9]); return 0.
// case 1: gated on D_00275BD8 == 0 -> func_00200830(*D_0028A564) + func_001D19D0(),
//   return 4. (func_00200830 takes a single arg; the value is loaded in the jal
//   delay slot.)
// All other modes (and the un-gated case-1 path) return 0.
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); the 991202 build walls at
// 91.67% (clean-store / delay-slot residual). D_00275BD8 is gp-rel
// (-sdatathreshold 8); D_0028A564 over-declared int[8] to force absolute addressing.
// Verified objdiff 100% vs build/expected/func_001AD1A0.o.
extern void func_001D19D0(void);
extern void func_001FF080(int, int, unsigned char *);
extern void func_00200830(int);
extern unsigned char D_00275BD8;
extern int D_0028A564[8];

int func_001AD1A0(void) {
    unsigned char *p;
    unsigned char *slot;
    int mode;

    p = *(unsigned char **)0x70003B6C;
    slot = p + 9;
    mode = p[9];
    switch (mode) {
    case 0:
        *slot = *slot + 1;
        D_00275BD8 = 1;
        func_001FF080(0, 3, slot);
        break;
    case 1:
        if (D_00275BD8 == 0) {
            func_00200830(D_0028A564[0]);
            func_001D19D0();
            return 4;
        }
        break;
    }
    return 0;
}
