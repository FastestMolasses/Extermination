// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), decomp2 lane round 3 (was a 92.44% NEARMISS).
// State-machine step keyed on arg1[4] (the first parameter is unused; the
// target never reads $a0). Case 0: if the init sub-state arg1[0xC] is not 2,
// set it to 3 and return 2; else func_001B0C00(8), advance arg1[4], refresh via
// func_001B6250(&D_00810E40) and return 0. Case 1: when D_0028A9A0 == 2, stop
// and flush (func_001FBC50 / func_001FABB0), set D_002821B4 = 2, advance
// arg1[4], initialize the status block at D_008102B0 (+0x1F2 = 0, +0x20C = -1,
// +0x2F3 = 3, +0x40 = D_0028A580), and if D_008101E4 == 3 run the first-time
// setup: func_001B0250(), func_0021B9A0(0, 0.0f, 0.0f), func_001D2830(2, 0),
// func_001D25F0(480.0f) and the quaternion {0, -1, 0, 1} at D_008105F0..FC;
// then D_008101E4 = 2, func_001B6250 again, return 1. Anything else returns 0.
// How it matches: func_001B0250 takes no arguments (its own C, and the matched
// twin func_001B7B30 calls it so); the old C passed the state byte and the
// header recorded the resulting mask "at the call site". The quaternion is
// written as `volatile float` stores in address order (func_001B7B30's
// spelling of the same block), and the return-0 paths `break` to one shared
// `return 0` (idiom-27).

extern void func_001B0250(void);
extern void func_001B0C00(int);
extern int func_001B6250(void *);
extern void func_001D25F0(float);
extern void func_001D2830(int, int);
extern void func_001FABB0(void);
extern void func_001FBC50(void);
extern void func_0021B9A0(int, float, float);

extern int D_002821B4;
extern int D_0028A580;
extern short D_0028A9A0;
extern unsigned char D_008101E4;
extern unsigned char D_008102B0;
extern volatile float D_008105F0;
extern volatile float D_008105F4;
extern volatile float D_008105F8;
extern volatile float D_008105FC;
extern unsigned char D_00810E40;

int func_001B6BF0(int arg0, unsigned char *arg1) {
    unsigned char *base = &D_008102B0;

    switch (arg1[4]) {
    case 0:
        if (*(char *)(arg1 + 0xC) != 2) {
            *(char *)(arg1 + 0xC) = 3;
            return 2;
        }
        func_001B0C00(8);
        arg1[4] = arg1[4] + 1;
        func_001B6250(&D_00810E40);
        break;
    case 1:
        if (D_0028A9A0 == 2) {
            func_001FBC50();
            func_001FABB0();
            D_002821B4 = 2;
            arg1[4] = arg1[4] + 1;
            *(short *)(base + 0x1F2) = 0;
            *(short *)(base + 0x20C) = -1;
            *(char *)(base + 0x2F3) = 3;
            *(int *)(base + 0x40) = D_0028A580;
            if (D_008101E4 == 3) {
                func_001B0250();
                func_0021B9A0(0, 0.0f, 0.0f);
                func_001D2830(2, 0);
                func_001D25F0(480.0f);
                D_008105F0 = 0.0f;
                D_008105F4 = -1.0f;
                D_008105F8 = 0.0f;
                D_008105FC = 1.0f;
            }
            D_008101E4 = 2;
            func_001B6250(&D_00810E40);
            return 1;
        }
        break;
    }
    return 0;
}
