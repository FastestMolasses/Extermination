// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state damage/animation reaction handler, gated on actor flag bit
// (*(arg0+0xB) & 4); returns 0 immediately when that bit is clear. When set, it
// seeds the 4-word scratch param block at absolute hardware addr 0x700038A0
// (0x3E99999A, 0, 0x41100000/9.0f-ish, 1.0f) and calls
// func_001B6F00(actor, PI, &D_700038A0). It then dispatches on arg2 and the
// actor's byte +3 (== 0x2C):
//   - arg2==1, or byte+3 != 0x2C: if global D_00810C7F is clear, arm event
//     0x80000018 in D_00246FB4, kick list D_00246F20 via
//     func_001BA1A0(arg1,...)/func_001BA1F0(actor), set actor byte0 = 2, return 2;
//     otherwise fall to block_13.
//   - byte+3 == 0x2C and D_00810CB7 == D_00810CB2: if actor bit (+0xB & 1) set,
//     kick list D_00247BA0; else arm 0x8000001E / kick D_00246F20. byte0 = 2, ret 2.
// block_13: if actor bit (+0xB & 1) set, mark byte+0xA = 1, byte0 = 2, return 3.
//   Otherwise a final 3-way: arg2==1 -> arm 0x80000018 / list D_002477A0;
//   byte+3==0x2C -> pick D_002474F4 = (actor f32 +0xB4 < 6.0f + D_00810354) ? 0x42
//   : 0x41, arm 0x80000010 / list D_00247420; else arm 0x80000018 / list
//   D_002477A0. Returns 1.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 81.2% (its idiom-13 clean-store delay-slot fill). Keys: (1) func_001B6F00's
// real signature is (void *actor, float, void *block) -- the PI float arg does
// NOT consume an integer slot in the EE mwcc ABI, so actor->a0, PI->f12,
// &D_700038A0->a1, and PI is written 3.14159265f (0x40490FDB); (2) the
// D_002474F4 = 0x41/0x42 select must be written as a full if/else (each arm doing
// its own store) so mwcc recomputes the store address per-path instead of
// hoisting, matching the target's duplicated lui at,hi(D_002474F4). objdiff
// reports 100.0 and the assembled .text is byte-identical (716 bytes) to
// build/expected/func_00157860.o.
extern void func_001B6F00(void *a, float f, void *b);
extern void func_001BA1A0(int a, int *p);
extern void func_001BA1F0(char *p);
extern int D_00246F20;
extern int D_00246FB4;
extern int D_00247420;
extern int D_002474F4;
extern int D_00247534;
extern int D_002477A0;
extern int D_00247834;
extern int D_00247BA0;
extern float D_00810354;
extern unsigned char D_00810C7F;
extern short D_00810CB2;
extern unsigned char D_00810CB7;
extern int D_700038A0;

int func_00157860(char *arg0, int arg1, int arg2) {
    if (*(unsigned char *)(arg0 + 0xB) & 4) {
        *(int *)0x700038A0 = 0x3E99999A;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x41100000;
        *(int *)0x700038AC = 0x3F800000;
        func_001B6F00(arg0, 3.14159265f, &D_700038A0);
        if (arg2 == 1) {
            if (D_00810C7F == 0) {
                D_00246FB4 = 0x80000018;
                func_001BA1A0(arg1, &D_00246F20);
                func_001BA1F0(arg0);
                *(char *)(arg0 + 0) = 2;
                return 2;
            }
            goto block_13;
        }
        if (*(unsigned char *)(arg0 + 3) != 0x2C) {
            if (D_00810C7F == 0) {
                D_00246FB4 = 0x80000018;
                func_001BA1A0(arg1, &D_00246F20);
                func_001BA1F0(arg0);
                *(char *)(arg0 + 0) = 2;
                return 2;
            }
            goto block_13;
        }
        if (D_00810CB7 == D_00810CB2) {
            if (*(unsigned char *)(arg0 + 0xB) & 1) {
                func_001BA1A0(arg1, &D_00247BA0);
                func_001BA1F0(arg0);
            } else {
                D_00246FB4 = 0x8000001E;
                func_001BA1A0(arg1, &D_00246F20);
                func_001BA1F0(arg0);
            }
            *(char *)(arg0 + 0) = 2;
            return 2;
        }
    block_13:
        if (*(unsigned char *)(arg0 + 0xB) & 1) {
            *(char *)(arg0 + 0xA) = 1;
            *(char *)(arg0 + 0) = 2;
            return 3;
        }
        if (arg2 == 1) {
            D_00247834 = 0x80000018;
            func_001BA1A0(arg1, &D_002477A0);
            func_001BA1F0(arg0);
        } else if (*(unsigned char *)(arg0 + 3) == 0x2C) {
            if (*(float *)(arg0 + 0xB4) < (6.0f + D_00810354)) {
                D_002474F4 = 0x42;
            } else {
                D_002474F4 = 0x41;
            }
            D_00247534 = 0x80000010;
            func_001BA1A0(arg1, &D_00247420);
            func_001BA1F0(arg0);
        } else {
            D_00247834 = 0x80000018;
            func_001BA1A0(arg1, &D_002477A0);
            func_001BA1F0(arg0);
        }
        return 1;
    }
    return 0;
}
