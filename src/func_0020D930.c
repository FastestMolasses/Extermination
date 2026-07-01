// NEARMISS func_0020D930  (vram 0x0020D930, 0x668 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 66.34% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation-ORDER permutation: target colors arg0->$s1 (high)/arg1->$s0 (low); this mwcc build always colors arg0->$s0/arg1->$s1 regardless of source shape. Body/structure/branch-shape fully correct; residual is purely the saved-reg swap propagated through every access site.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Menu hover-page angle quantizer: reads the left-stick angle (D_700038AC, a
// PS2 pad-lib float register) and, if the stick deflection (D_700038A8) is
// past the noise floor (gated by func_00128350/func_00100130), quantizes the
// angle into an 8-direction hover state stored at arg0+0x11 (right=2,
// down=1/3/4/5..., up, left, etc., depending on which of the three angle
// tables arg1 selects). Whenever the state CHANGES it fires the hover sound
// cue func_001FB9F0(5, 0x1000, 0x1000, 0x1000). If the gate fails, state
// resets to 0 (stick released / no hover).
extern void func_001B62C0(float *out);
extern int func_00100130(void);
extern int func_00128350(float x);
extern void func_001FB9F0(int a, int b, int c, int d);
extern float D_700038A0;
extern float D_700038A8;
extern float D_700038AC;

void func_0020D930(char *arg0, int arg1) {
    float ang;
    unsigned char cur;

    func_001B62C0(&D_700038A0);
    func_00128350(D_700038A8);

    if (func_00100130() != 0) {
        if (arg1 == 0) {
            ang = D_700038AC;
            if (ang < -0.7853982f) {
                if (ang < -2.3561945f) {
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
            if (cur != 3) {
                func_001FB9F0(5, 0x1000, 0x1000, 0x1000);
            }
            arg0[0x11] = 3;
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
