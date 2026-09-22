// NEARMISS func_0020D930  (vram 0x0020D930, 0x668 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.58% via mwcc 2.3.3 (-O4,p -sdatathreshold 0), remeasured
// after correcting table0's left sector and the alternate table's last sector.
// Control/argument behavior is independently checked against the original
// instructions; remaining code-generation differences are not classified.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Menu hover-page angle quantizer. 001B62C0 produces normalized stick
// magnitude/angle. 00128350 converts binary32 to a soft-double bit pattern;
// 00100130 compares two such patterns through 001274B0. The original gate
// is magnitude >= double0.8 (3FE999999999999A), including float-to-double
// conversion, rather than a raw stick dead zone. Table0 is the status hub;
// table1 is the ITEM wheel.
// A changed nonzero selection plays cue5; releasing the stick resets the
// hover byte without a sound. Raw original-instruction validation lives in
// the native port's tools/test_menu_hover_source_reference.py (8,064 cases,
// all selector tables, comparing this readable C with the original function).
extern void func_001B62C0(float *out);
extern int func_00100130(unsigned long long left, unsigned long long right);
extern unsigned long long func_00128350(float x);
extern void func_001FB9F0(int a, int b, int c, int d);
extern float D_700038A0;
extern float D_700038A8;
extern float D_700038AC;

void func_0020D930(char *arg0, int arg1) {
    float ang;
    unsigned char cur;

    func_001B62C0(&D_700038A0);
    if (func_00100130(func_00128350(D_700038A8), 0x3FE999999999999AULL) != 0) {
        if (arg1 == 0) {
            ang = D_700038AC;
            if (ang < -0.7853982f) {
                if (ang < -2.3561945f) {
                    cur = arg0[0x11];
                    if (cur != 4) {
                        func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                    }
                    arg0[0x11] = 4;
                    return;
                }
                cur = arg0[0x11];
                if (cur != 3) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 3;
                return;
            }
            if (ang < 0.7853982f) {
                cur = arg0[0x11];
                if (cur != 2) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 2;
                return;
            }
            if (ang < 2.3561945f) {
                cur = arg0[0x11];
                if (cur != 1) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 1;
                return;
            }
            cur = arg0[0x11];
            if (cur != 4) {
                func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
            }
            arg0[0x11] = 4;
            return;
        }

        if (arg1 != 1) {
            ang = D_700038AC;
            if (ang < -1.5707964f) {
                if (ang < -2.670354f) {
                    cur = arg0[0x11];
                    if (cur != 1) {
                        func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                    }
                    arg0[0x11] = 1;
                    return;
                }
                cur = arg0[0x11];
                if (cur != 2) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 2;
                return;
            }
            if (ang < -0.41887903f) {
                cur = arg0[0x11];
                if (cur != 3) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 3;
                return;
            }
            if (ang < 0.36651915f) {
                cur = arg0[0x11];
                if (cur != 4) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 4;
                return;
            }
            if (ang < 1.5707964f) {
                cur = arg0[0x11];
                if (cur != 5) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 5;
                return;
            }
            if (ang < 2.7576203f) {
                cur = arg0[0x11];
                if (cur != 6) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 6;
                return;
            }
            cur = arg0[0x11];
            if (cur != 1) {
                func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
            }
            arg0[0x11] = 1;
            return;
        }

        ang = D_700038AC;
        if (ang < -0.5235988f) {
            if (ang < -2.0071287f) {
                cur = arg0[0x11];
                if (cur != 4) {
                    func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
                }
                arg0[0x11] = 4;
                return;
            }
            cur = arg0[0x11];
            if (cur != 3) {
                func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
            }
            arg0[0x11] = 3;
            return;
        }
        if (ang < 0.5235988f) {
            cur = arg0[0x11];
            if (cur != 2) {
                func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
            }
            arg0[0x11] = 2;
            return;
        }
        if (ang < 2.0071287f) {
            cur = arg0[0x11];
            if (cur != 1) {
                func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
            }
            arg0[0x11] = 1;
            return;
        }
        if (ang < 3.1415927f) {
            cur = arg0[0x11];
            if (cur != 5) {
                func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
            }
            arg0[0x11] = 5;
            return;
        }
        cur = arg0[0x11];
        if (cur != 4) {
            func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
        }
        arg0[0x11] = 4;
        return;
    }

    arg0[0x11] = 0;
}
