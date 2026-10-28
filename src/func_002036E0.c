// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.33% NEARMISS).
// Fixes read from the original instructions:
//  - the tail "pump until finished or state 3" loop is one while loop whose
//    condition tests both (func_00206CC0 == 0 && func_00206B80 != 3); the
//    earlier body/break form laid the blocks out differently;
//  - the two guard compares are written `> 10` and `s1 > 4` (not `>= 0xB`,
//    `>= 5`), which keeps the compare result in the scratch register;
//  - the decoder's fifth argument is a struct member past the 0x50000-byte
//    data area, not an int-array index off a byte offset.
//
// Per-frame stream/decode pump: arg0 is the stream/device handle, arg1 the
// active buffer cursor, arg2 the byte budget (split into two running
// budgets s0/s1). Loops while the remaining budget s1 >= 5 and the device
// isn't in state 3 (func_00206B80): pings the watchdog (func_001B57E0);
// if global flag D_00810E70 has the relevant bit set (bit set depends on
// byte *0x70003B90) and the handle's queue-depth field +8 is >= 0xB, kicks
// a flush (func_00206B70(&D_002DF740)); reads the next chunk size
// (func_002039D0); while budget s0 is still positive and the chunk is >=
// 0x10000, requests func_00113D08 (aligns/consumes 0x800-byte units,
// advancing the buffer via func_00203A10 and debiting s0, or on failure
// zeroes s0 and flushes); always pumps func_00203980(); then drains any
// decoded output via func_00203A80/func_00108AA0/func_00203AE0, debiting
// s1; pings func_00207290(); and once (latched by s2) checks
// func_00203B80(&D_007A55A0) && func_00203990() to fire a one-shot
// func_002061B0(1)+func_00206520(&D_002DF800). After the loop it resets two
// subsystems via func_00119978(0/1, 0, 0), pumps func_00203980() until the
// device reports ready (func_00206BF0), then pumps again until either the
// device finishes (func_00206CC0) or drops into state 3
// (func_00206B80==3), and finally tears down via func_00206200() +
// func_00206320(&D_002DF800), returning 1.
//
/* The buffer cursor (arg1) heads a 0x50000-byte data area followed by
   three header words; the third is passed to the decoder. */
typedef struct { char buf[0x50000]; int f0; int f4; int f8; } StreamBuf;
extern void func_001B57E0(void);
extern int func_002039D0(char *, int *);
extern void func_00203A10(char *, int);
extern int func_00203A80(char *, int *);
extern void func_00203AE0(char *, int);
extern int func_00203B80(void *);
extern void func_00203980(void);
extern int func_00203990(void);
extern int func_00113D08(int, int, int, int *);
extern int func_00108AA0(char *, int, int, char *, int);
extern void func_00119978(int, int, int);
extern void func_002061B0(int);
extern void func_00206520(void *);
extern void func_00206B70(void *);
extern int func_00206B80(char *);
extern int func_00206BF0(char *);
extern int func_00206CC0(char *);
extern void func_00207290(void);
extern void func_00206200(void);
extern void func_00206320(void *);
extern int D_002DF740;
extern int D_002DF800;
extern int D_007A55A0;
extern unsigned short D_00810E70;

int func_002036E0(char *arg0, char *arg1, int arg2) {
    int sp74;
    int sp78;
    int sp7C;
    int s0 = arg2;
    int s1 = arg2;
    int s2 = 0;
    int n;
    int v0;

    while (s1 > 4 && func_00206B80(arg0) != 3) {
        func_001B57E0();
        if (D_00810E70 & (short)(*(unsigned char *)0x70003B90 == 0 ? 0x8F0 : 0x800)) {
            if (*(int *)(arg0 + 8) > 10) {
                func_00206B70(&D_002DF740);
            }
        }
        v0 = func_002039D0(arg1, &sp74);
        if (s0 > 0 && v0 >= 0x10000) {
            n = func_00113D08(0x20, sp74, 1, &sp7C);
            if (sp7C == 0) {
                n <<= 0xB;
                func_00203A10(arg1, n);
                s0 -= n;
            }
            if (sp7C != 0) {
                s0 = 0;
                func_00206B70(&D_002DF740);
            }
        }
        func_00203980();
        v0 = func_00203A80(arg1, &sp78);
        if (v0 > 0) {
            n = func_00108AA0(arg0, sp78, v0, arg1, ((StreamBuf *)arg1)->f8);
            func_00203AE0(arg1, n);
            s1 -= n;
        }
        func_00207290();
        if (s2 == 0) {
            if (func_00203B80(&D_007A55A0) != 0 && func_00203990() != 0) {
                func_002061B0(1);
                func_00206520(&D_002DF800);
                s2 = 1;
            }
        }
    }
    func_00119978(0, 0, 0);
    func_00119978(1, 0, 0);
    if (func_00206BF0(arg0) == 0) {
        do {
            func_00203980();
        } while (func_00206BF0(arg0) == 0);
    }
    while (func_00206CC0(arg0) == 0 && func_00206B80(arg0) != 3) {
        func_00203980();
    }
    func_00206200();
    func_00206320(&D_002DF800);
    return 1;
}
