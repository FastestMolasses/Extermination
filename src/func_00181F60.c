// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.46% NEARMISS).
// Fixes read from the original instructions:
// - The scratchpad vector at 0x700038A0 is written as integer words (0, 0,
//   0x40800000 = 4.0f, 0x3F800000 = 1.0f), not float stores.
// - The scratchpad bases are char array externs (D_700038A0/B0/C0[]) passed
//   uncast, with callee prototypes taking void pointers as in their twins.
// - The angle update is a compound `+= 25.0f`, which fixes the store/call
//   order and the float register colouring.
// - The heading arguments read 0x700031D0 directly inside the call instead of
//   through a reloaded local pointer.
//
// Resets the scratch vector at 0x700038A0 to (0, 0, 4.0, 1.0), transforms it
// by the matrix at self+0xD0 into 0x700038B0 (func_001026A0), copies that to
// 0x700038C0 (func_001031E0), advances the copy's float at 0x700038C4 by 25.0,
// and if func_0019A570 on the two points (mode 4) succeeds AND the global state
// object (*0x700031D0) is in mode 0x36 (byte at +0x1A), runs func_00199DB0
// (self+0x290), computes a heading func_0011E620(-(*+0x3C), *+0x34) into
// 0x70003A20, rotates it by func_001B1470(pi/2 + heading), stores the result
// to 0x70003A20 and self+0x218, and returns 1; all other paths return 0.
extern void func_001026A0(void *, void *, void *);
extern void func_001031E0(void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern int func_00199DB0(char *);
extern float func_0011E620(float y, float x);
extern float func_001B1470(float angle);
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];

int func_00181F60(char *self) {
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(int *)0x700038A8 = 0x40800000;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
    func_001031E0(D_700038C0, D_700038B0);
    *(float *)0x700038C4 += 25.0f;
    if (func_0019A570(D_700038B0, D_700038C0, 4, 0) != 0) {
        if (*(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x36) {
            func_00199DB0(self + 0x290);
            *(float *)0x70003A20 = func_0011E620(-*(float *)(*(char **)0x700031D0 + 0x3C), *(float *)(*(char **)0x700031D0 + 0x34));
            *(float *)0x70003A20 = func_001B1470(1.5707964f + *(float *)0x70003A20);
            *(float *)(self + 0x218) = *(float *)0x70003A20;
            return 1;
        }
    }
    return 0;
}
