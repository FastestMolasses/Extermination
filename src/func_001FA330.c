// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-channel level ramp over 3 channels (D_00282150[i] active flag at +4,
// D_00281FD0 state blocks of 0x60 bytes: level +0x58, delta +0x54, callback
// flag +0x5C). While the delta is nonzero the level moves by it: rising, it
// stops at the cap (0x3FFF; 0x3000 for channel 0 while D_0028215B is set);
// falling, it stops at 0 and runs func_001FAAC0(i) when the callback flag is set.
// Then the level is sent with func_0011A608: channels 1..2 to the mask at
// block+8 (both values = level); channel 0 to the masks 1 << D_00281FD4 and
// 1 << D_002820F4, as (0, level) and (level, 0) while D_0028215B is clear and
// as (level, level) to both while it is set.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 5, 2026-10-20). The
// 95.84 NEARMISS "register-allocation order" was the local declaration order
// (i first: s0, as in the target); the cap select is the three-arm
// if / else-if / else on i and D_0028215B, and the delta test is a plain float
// truth test (`if (v)`).
extern int float_to_int(float);
extern void func_0011A608(long long, int, int);
extern void func_001FAAC0(int);
extern char D_00281FD0;
extern int D_00281FD4;
extern int D_002820F4;
extern char D_00282150;
extern unsigned char D_0028215B;

void func_001FA330(void) {
    int i;
    float f20;
    char *s1;
    float v;
    char *s2;
    int cap;
    int iv;

    i = 0;
    s1 = &D_00282150;
    s2 = &D_00281FD0;
    do {
        if (i != 0) {
            cap = 0x3FFF;
        } else if (D_0028215B == 0) {
            cap = 0x3FFF;
        } else {
            cap = 0x3000;
        }
        if (*(char *)(s1 + 4) != 0) {
            v = *(float *)(s2 + 0x54);
            if (v) {
                *(float *)(s2 + 0x58) = *(float *)(s2 + 0x58) + v;
                if (*(float *)(s2 + 0x54) > 0.0f) {
                    if (!(*(float *)(s2 + 0x58) < (float)cap)) {
                        *(int *)(s2 + 0x54) = 0;
                        *(float *)(s2 + 0x58) = (float)cap;
                    }
                } else if (*(float *)(s2 + 0x58) <= 0.0f) {
                    *(int *)(s2 + 0x54) = 0;
                    *(int *)(s2 + 0x58) = 0;
                    if (*(char *)(s2 + 0x5C) != 0) {
                        func_001FAAC0(i);
                    }
                }
                if (i != 0) {
                    f20 = *(float *)(s2 + 0x58);
                    iv = float_to_int(f20);
                    func_0011A608(*(long long *)(s2 + 8), iv, float_to_int(f20));
                } else if (D_0028215B == 0) {
                    func_0011A608((long long)1 << D_00281FD4, 0, float_to_int(*(float *)(s2 + 0x58)));
                    func_0011A608((long long)1 << D_002820F4, float_to_int(*(float *)(s2 + 0x58)), 0);
                } else {
                    f20 = *(float *)(s2 + 0x58);
                    iv = float_to_int(f20);
                    func_0011A608((long long)1 << D_00281FD4, iv, float_to_int(f20));
                    f20 = *(float *)(s2 + 0x58);
                    iv = float_to_int(f20);
                    func_0011A608((long long)1 << D_002820F4, iv, float_to_int(f20));
                }
            }
        }
        i += 1;
        s1 += 1;
        s2 += 0x60;
    } while (i < 3);
}
