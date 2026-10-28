// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.69% NEARMISS).
// Fixes read from the original instructions:
//  - The "register-allocation permutation" was source shape, not a wall:
//    the stores to the output entry come before the counter/pointer steps
//    (same shape as the byte-matched twin func_002032C0), with compound
//    increments;
//  - locals are declared counter first, then src, then dst (the counter gets
//    the lowest of the three saved registers), while the first pass still
//    initialises src and dst before the counter and the second pass resets
//    the counter first.

//
// Audio/stream table init. Two passes fill parallel 8-byte (lo,hi) records:
// pass 1 reads 2 entries from the gp-rel table D_00275848 into D_0028A480,
// pass 2 reads 0x17 entries from D_00264E40 into D_0028A3C0. Each entry is
// produced by spinning func_00113280(0) / func_00111C28(buf, src) until
// func_00111C28 returns nonzero, then copying buf[0]/buf[1] out (buf is the
// 32-byte file record the callee writes). Finishes with func_002032C0() and
// sub_O_STREAM_MUSIC_DAT_1().
extern void func_00113280(int a0);
extern int func_00111C28(int *a0, int a1);
extern void func_002032C0(void);
extern void sub_O_STREAM_MUSIC_DAT_1(void);
extern int D_00275848;
extern int D_00264E40[];
extern int D_0028A480[];
extern int D_0028A3C0[];

void func_001FEE60(void) {
    int buf[8];
    int i;
    int *src;
    int *dst;

    src = &D_00275848;
    dst = D_0028A480;
    i = 0;
    do {
        func_00113280(0);
        while (func_00111C28(buf, *src) == 0) {
            func_00113280(0);
        }
        dst[0] = buf[0];
        dst[1] = buf[1];
        i += 1;
        src += 1;
        dst += 2;
    } while (i < 2);
    i = 0;
    src = D_00264E40;
    dst = D_0028A3C0;
    do {
        func_00113280(0);
        while (func_00111C28(buf, *src) == 0) {
            func_00113280(0);
        }
        dst[0] = buf[0];
        dst[1] = buf[1];
        i += 1;
        src += 1;
        dst += 2;
    } while (i < 0x17);
    func_002032C0();
    sub_O_STREAM_MUSIC_DAT_1();
}
