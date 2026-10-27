// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A8 0x700038AC

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 72.58% NEARMISS).
// Fixes read from the original instructions:
//  - Each selector table is an if / else-if chain of half-open angle ranges
//    (lo <= ang < hi) tested in the original's order, with the wrap-around
//    sector as the final else; the nested less-than tree was the wrong shape.
//  - The selection byte is compared unsigned (arg0 is unsigned char *).
//  - func_00100130 / func_00128350 take and return doubles (soft-float
//    __gedf2-style compare / __extendsfdf2); the threshold is the literal 0.8,
//    so its 64-bit constant is built before the conversion result is moved.
//  - The scratchpad words 0x700038A8 / 0x700038AC are relocated externs
//    (idiom-32, opted in with // SPAD:), which keeps the 0x7000 upper half out
//    of the arg1 == 1 branch slot.
//
// Menu hover-page angle quantizer. 001B62C0 produces normalized stick
// magnitude/angle. 00128350 converts binary32 to a soft-float double;
// 00100130 compares two such doubles through 001274B0 (a >= b). The original
// gate is magnitude >= 0.8 as a double, including the float-to-double
// conversion, rather than a raw stick dead zone. Table0 is the status hub;
// table1 is the ITEM wheel.
// A changed nonzero selection plays cue5; releasing the stick resets the
// hover byte without a sound. Raw original-instruction validation lives in
// the native port's tools/test_menu_hover_source_reference.py (8,064 cases,
// all selector tables, comparing this readable C with the original function).
extern void func_001B62C0(float *out);
extern int func_00100130(double left, double right);
extern double func_00128350(float x);
extern void func_001FB9F0(int a, int b, int c, int d);
extern float D_700038A0;
extern float D_700038A8;
extern float D_700038AC;

void func_0020D930(unsigned char *arg0, int arg1) {
    float ang;

    func_001B62C0(&D_700038A0);
    if (func_00100130(func_00128350(D_700038A8), 0.8) != 0) {
        if (arg1 == 0) {
            ang = D_700038AC;
            if (ang >= -0.7853982f && ang < 0.7853982f) {
                if (arg0[0x11] != 2) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 2;
            } else if (ang >= -2.3561945f && ang < -0.7853982f) {
                if (arg0[0x11] != 3) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 3;
            } else if (ang >= 0.7853982f && ang < 2.3561945f) {
                if (arg0[0x11] != 1) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 1;
            } else {
                if (arg0[0x11] != 4) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 4;
            }
        } else if (arg1 == 1) {
            ang = D_700038AC;
            if (ang >= -0.5235988f && ang < 0.5235988f) {
                if (arg0[0x11] != 2) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 2;
            } else if (ang >= 0.5235988f && ang < 2.0071287f) {
                if (arg0[0x11] != 1) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 1;
            } else if (ang >= 2.0071287f && ang < 3.1415927f) {
                if (arg0[0x11] != 5) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 5;
            } else if (ang >= -2.0071287f && ang < -0.5235988f) {
                if (arg0[0x11] != 3) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 3;
            } else {
                if (arg0[0x11] != 4) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 4;
            }
        } else {
            ang = D_700038AC;
            if (ang >= -0.41887903f && ang < 0.36651915f) {
                if (arg0[0x11] != 4) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 4;
            } else if (ang >= 0.36651915f && ang < 1.5707964f) {
                if (arg0[0x11] != 5) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 5;
            } else if (ang >= 1.5707964f && ang < 2.7576203f) {
                if (arg0[0x11] != 6) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 6;
            } else if (ang >= -1.5707964f && ang < -0.41887903f) {
                if (arg0[0x11] != 3) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 3;
            } else if (ang >= -2.670354f && ang < -1.5707964f) {
                if (arg0[0x11] != 2) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 2;
            } else {
                if (arg0[0x11] != 1) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 1;
            }
        }
    } else {
        arg0[0x11] = 0;
    }
}
