// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// State-machine dispatch on the global mode byte at +0xA of *(void**)0x70003B6C.
// case 0: prime/start (func_001AED80), bump the mode byte, set the gp-rel flag
//   D_00275BD8=1, then func_001FF080(1,0) + func_0021B180().
// case 1: poll func_0021B550(); only if D_00275BD8==0, run func_0021B840() and
//   bump the mode byte.
// case 2: poll func_0021B550(); if nonzero, func_001D2830(3,1) + func_001AEDB0(0)
//   and return 4. All other paths return 0.
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); the 991202 build leaves a
// dead-block / delay-slot residual (94.83%). D_00275BD8 must be gp-rel
// (-sdatathreshold 8). Verified objdiff 100% vs build/expected/func_001ADF50.o.
extern void func_001AED80(int);
extern void func_001AEDB0(int);
extern void func_001D2830(int, int);
extern void func_001FF080(int, int);
extern void func_0021B180(void);
extern int func_0021B550(void);
extern void func_0021B840(void);
extern unsigned char D_00275BD8;

int func_001ADF50(void) {
    unsigned char *p;
    int mode;

    p = *(unsigned char **)0x70003B6C;
    mode = p[0xA];
    switch (mode) {
    case 0:
        func_001AED80(0);
        p = *(unsigned char **)0x70003B6C;
        p[0xA] = p[0xA] + 1;
        D_00275BD8 = 1;
        func_001FF080(1, 0);
        func_0021B180();
        break;
    case 1:
        func_0021B550();
        if (D_00275BD8 == 0) {
            func_0021B840();
            p = *(unsigned char **)0x70003B6C;
            p[0xA] = p[0xA] + 1;
        }
        break;
    case 2:
        if (func_0021B550() != 0) {
            func_001D2830(3, 1);
            func_001AEDB0(0);
            return 4;
        }
        break;
    }
    return 0;
}
