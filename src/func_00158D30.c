// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038B0 0x700038B4 0x700038B8 0x700038BC 0x700038C0 0x700038C4 0x700038C8 0x700038CC
//
// State dispatch on the byte at p+4.
//  0: func_001B0FD0(p), the words at p+0x80/0x84/0x88 = 0x40000000, then
//     func_001C6380(p).
//  1: by the halfword at p+0x2E, writes the four scratchpad words at
//     0x700038B0..0x700038BC: mode 0 gives (0x80, 0, 0, 0x80) when the byte at
//     p+0xB is 0, else (0, 0x80, 0, 0x80); mode 1 gives (0, 0x80, 0, 0x80);
//     other modes leave them. Then the words at 0x700038C0..0x700038CC =
//     0x3F800000 (1.0f), the callback at p+0x4C runs when func_001B17A0(p) is
//     non-zero, and func_001F4A10(p + 0xD0, &D_700038B0, &D_700038C0).
//  2, 3: func_001AFC10(p).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// a 96.65 NEARMISS. Two keys: the scratchpad words are volatile externs, one
// per address (the target reloads the upper half before every store, keeps the
// source store order and leaves both inner branch slots as nop), and every
// case ends in `break`, not `return`. With `return` mwcc fills the two inner
// branch slots with the scratchpad upper half in every spelling tested
// (literal, plain or volatile externs, all 256 extern subsets).

extern void func_001AFC10(char *p);
extern int func_001B0FD0(char *);
extern int func_001B17A0(char *p);
extern void func_001C6380(char *p);
extern void func_001F4A10(void *a, void *b, void *c);
extern volatile int D_700038B0;
extern volatile int D_700038B4;
extern volatile int D_700038B8;
extern volatile int D_700038BC;
extern volatile int D_700038C0;
extern volatile int D_700038C4;
extern volatile int D_700038C8;
extern volatile int D_700038CC;

void func_00158D30(char *p) {
    int t;

    t = *(unsigned char *)(p + 4);
    switch (t) {
    case 0:
        func_001B0FD0(p);
        *(int *)(p + 0x80) = 0x40000000;
        *(int *)(p + 0x84) = 0x40000000;
        *(int *)(p + 0x88) = 0x40000000;
        func_001C6380(p);
        break;
    case 1:
        switch (*(unsigned short *)(p + 0x2E)) {
        case 0:
            if (*(unsigned char *)(p + 0xB) == 0) {
                D_700038B0 = 0x80;
                D_700038B4 = 0;
                D_700038B8 = 0;
                D_700038BC = 0x80;
            } else {
                D_700038B0 = 0;
                D_700038B4 = 0x80;
                D_700038B8 = 0;
                D_700038BC = 0x80;
            }
            break;
        case 1:
            D_700038B0 = 0;
            D_700038B4 = 0x80;
            D_700038B8 = 0;
            D_700038BC = 0x80;
            break;
        }
        D_700038C0 = 0x3F800000;
        D_700038C4 = 0x3F800000;
        D_700038C8 = 0x3F800000;
        D_700038CC = 0x3F800000;
        if (func_001B17A0(p) != 0) {
            (*(void (**)(char *))(p + 0x4C))(p);
        }
        func_001F4A10(p + 0xD0, &D_700038B0, &D_700038C0);
        break;
    case 2:
    case 3:
        func_001AFC10(p);
        break;
    }
}
