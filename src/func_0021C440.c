// NEARMISS func_0021C440  (vram 0x0021C440, 0xD60 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc dead-code-eliminates one redundant compare that the target keeps. In the `p[0x1F0] == 0x3C` arm the target emits a 3-way dispatch on p[0xD] as `bnez v0,X / <case-0 body> / X: beq v0,a2(1),tail / beq v0,a1(2),tail / b tail` - i.e. it keeps BOTH the ==1 and the ==2 compares even though all thr...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Damage/knockback reaction state machine. p+0xF is a pending-hit request code
// (bit 0x80 = "already serviced this frame"); p+4/p+5/p+6 are the animation
// bank/clip/frame triple, p+0x1F0/0x1F1 the reaction mode + variant, p+0x220 is
// remaining HP, p+0x224 / p+0x22C the pending front/back damage amounts.
// Return value: 1 when the entity entered a fresh reaction this frame.
// `s` accumulates two flags: 0x80 = refresh the collision/anim state via
// func_0017C370, 0x01 = a reaction actually started.

extern unsigned char D_008106F1;
extern unsigned char D_0081083C;

extern void copy_qw4(void *dst, void *src);
extern float func_0011E620(float y, float x);
extern void func_0017C370(char *p);
extern float func_001B1470(float a);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001EFE00(int id, char *p);
extern char *func_001F00A0(int id, void *pos, void *rot, int a);
extern void func_001FBD50(char *p, int id, int a, float b);
extern int func_0021BB00(char *p);
extern int func_0021BC40(char *p);
extern void func_0021C200(char *p);
extern void func_0021C270(char *p);
extern void func_0021C350(char *p);
extern int func_0021C3F0(void);
extern int func_0021D640(char *p);
extern int func_0021D6C0(char *p);

int func_0021C440(char *p)
{
    int s;
    int st;
    int anim;
    int mode;
    char *o;
    char *q;

    s = 0;
    if (*(float *)(p + 0x220) <= 0.0f) {
        if (*(float *)(p + 0x224)) {
            *(float *)(p + 0x224) = 0.0f;
            *(unsigned char *)(p + 0) = 2;
        }
        if (*(float *)(p + 0x22C)) {
            *(float *)(p + 0x22C) = 0.0f;
            *(unsigned char *)(p + 0) = 2;
        }
        return 0;
    }
    if (*(unsigned char *)(p + 0x234) == 1) {
        if (*(float *)(p + 0x22C)) {
            *(float *)(p + 0x22C) = 0.0f;
            if (*(unsigned char *)(p + 0xF) == 0 && !*(float *)(p + 0x224)) {
                *(unsigned char *)(p + 0) = 1;
                return 0;
            }
        }
    }
    if (func_0021BB00(p) != 0) {
        if (*(unsigned char *)(p + 0xF) != 7) {
            *(float *)(p + 0x224) = 0.0f;
            *(float *)(p + 0x22C) = 0.0f;
            if (*(unsigned char *)(p + 0) & 2) {
                *(unsigned char *)(p + 0) = 1;
            }
            if (*(unsigned char *)(p + 0xF) == 2) {
                *(unsigned char *)(p + 0xF) = 0;
            }
        }
        return 0;
    }
    st = *(unsigned char *)(p + 0xF);
    if (st & 0x80) {
        return 0;
    }
    if (st == 1) {
        func_0021C270(p);
        *(unsigned char *)(p + 4) = 2;
        *(unsigned char *)(p + 5) = 0xC;
        *(char *)(p + 6) = 0;
        *(unsigned char *)(p + 0x1F0) = 0x3E;
        *(unsigned char *)(p + 0x1F1) = 1;
        s = 0x81;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 2) {
        mode = *(unsigned char *)(p + 0x1F0);
        if ((mode == 0x2C && *(unsigned char *)(p + 0xD) == 2) ||
            mode == 0x10 || mode == 0x17 || mode == 0x1A || mode == 0x1D ||
            mode == 0x2A || mode == 0x21 || mode == 0x22 ||
            mode == 0x2F || mode == 0x30 || mode == 0x39) {
            s = 1;
        } else if (func_0021D640(p) != 0) {
            if (func_0021D6C0(p) != 0) {
                s = 1;
            }
        } else {
            func_0021C350(p);
            *(unsigned char *)(p + 4) = 2;
            *(unsigned char *)(p + 5) = 0x10;
            *(char *)(p + 6) = 0;
            s = 0x81;
            *(unsigned char *)(p + 0x1F0) = 0x3E;
        }
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 3) {
        func_0021C270(p);
        *(unsigned char *)(p + 4) = 2;
        *(unsigned char *)(p + 5) = 0xF;
        *(char *)(p + 6) = 0;
        *(unsigned char *)(p + 0x1F0) = 0x3E;
        *(unsigned char *)(p + 0x1F1) = 1;
        s = 0x81;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 5) {
        *(float *)(p + 0x220) = 0.0f;
        *(unsigned char *)(p + 4) = 2;
        *(unsigned char *)(p + 5) = 0xF;
        *(char *)(p + 6) = 0;
        *(unsigned char *)(p + 0x1F0) = 0x3E;
        *(unsigned char *)(p + 0x1F1) = 1;
        s = 0x81;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 4) {
        func_0021C270(p);
        s = 0x80;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 6) {
        func_0021C350(p);
        if (*(float *)(p + 0x220) <= 0.0f) {
            if (*(unsigned char *)(p + 0x234) == 1) {
                *(unsigned char *)(p + 4) = 2;
                *(unsigned char *)(p + 5) = 3;
                *(char *)(p + 6) = 0;
                *(unsigned char *)(p + 0x1F0) = 0x3F;
            } else {
                *(unsigned char *)(p + 4) = 2;
                *(unsigned char *)(p + 5) = 1;
                *(char *)(p + 6) = 0;
                *(unsigned char *)(p + 0x1F0) = 0x40;
            }
        } else {
            *(unsigned char *)(p + 4) = 2;
            *(unsigned char *)(p + 5) = 0x11;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x3E;
        }
        s = 0x81;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 7) {
        if (*(unsigned char *)(p + 0x1F0) == 0x17) {
            *(unsigned char *)(p + 4) = 2;
            *(unsigned char *)(p + 5) = 4;
            *(char *)(p + 6) = 0;
            *(float *)(p + 0x224) = *(float *)(p + 0x220);
            *(char *)(p + 0x302) = 0;
            func_001FBD50(p, 0x159, 0, 300.0f);
            s = 1;
        } else {
            *(unsigned char *)(p + 4) = 2;
            *(unsigned char *)(p + 5) = 0x12;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x3E;
            s = 0x81;
            *(float *)(p + 0x220) = 0.0f;
        }
        *(unsigned char *)(p + 0) = 2;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 8) {
        func_0021C270(p);
        func_0021C350(p);
        s = 0x80;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 9) {
        func_0021C350(p);
        s = 0x80;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 0xA) {
        *(float *)(p + 0x220) = 0.0f;
        *(unsigned char *)(p + 4) = 2;
        *(unsigned char *)(p + 5) = 0x13;
        *(char *)(p + 6) = 0;
        *(unsigned char *)(p + 0x1F0) = 0x3E;
        s = 0x81;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else if (st == 0xB) {
        *(float *)(p + 0x220) = 0.0f;
        *(unsigned char *)(p + 4) = 2;
        *(unsigned char *)(p + 5) = 0x14;
        *(char *)(p + 6) = 0;
        *(unsigned char *)(p + 0x1F0) = 0x3E;
        s = 0x81;
        *(unsigned char *)(p + 0xF) |= 0x80;
    } else {
        mode = *(unsigned char *)(p + 0x1F0);
        if (mode == 0x3B) {
            goto tail;
        }
        if (mode == 0x3C) {
            if (*(unsigned char *)(p + 0xD) == 0) {
                if (*(float *)(p + 0x224)) {
                    func_0021C350(p);
                    func_001B61C0(0, 0xC0, 5, 1);
                    func_001FBD50(p, 0x152, 0, 300.0f);
                }
            } else {
                if (*(unsigned char *)(p + 0xD) == 1) {
                    goto tail;
                }
                if (*(unsigned char *)(p + 0xD) == 2) {
                    goto tail;
                }
            }
            goto tail;
        }
        if (func_0021D640(p) != 0) {
            if (func_0021D6C0(p) != 0) {
                s = 1;
            }
            goto tail;
        }
        if (D_0081083C != 0) {
            *(unsigned char *)(p + 4) = 2;
            *(unsigned char *)(p + 5) = 0xB;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x3B;
            s = 0x81;
            *(unsigned char *)(p + 0x1F1) = 1;
            goto tail;
        }
        if (*(unsigned char *)(p + 0x234) == 1 && *(unsigned char *)(p + 0) == 1 &&
            (*(unsigned char *)(p + 0x23A) == 0x5B || *(unsigned char *)(p + 0x23A) == 6)) {
            if (*(unsigned char *)(p + 4) == 1 && *(unsigned char *)(p + 5) == 0) {
                goto hit_a;
            }
            anim = *(unsigned char *)(p + 5);
            if (anim == 1) {
                goto hit_a;
            }
            if (anim == 0x21 || anim == 0x22) {
                goto hit_a;
            }
            if (anim == 0x1D && *(unsigned char *)(p + 0x1F1) == 1) {
                goto hit_a;
            }
            if (anim == 0x1E && *(unsigned char *)(p + 0x1F1) == 1) {
                goto hit_a;
            }
            goto try_b;
        hit_a:
            *(unsigned char *)(p + 0) |= 2;
            if (*(unsigned char *)(p + 0x23A) == 6) {
                if (*(unsigned char *)(p + 0x31E) == 0) {
                    *(float *)(p + 0x224) = 3.0f;
                } else {
                    *(float *)(p + 0x224) = 5.0f;
                }
            } else {
                *(float *)(p + 0x224) = 3.0f;
            }
            func_0021C350(p);
            q = func_001F00A0(0x8000001B, p + 0xB0, p + 0xC0, 0);
            if (q != 0) {
                copy_qw4(q + 0xD0, p + 0xD0);
                *(float *)(q + 0x104) = *(float *)(p + 0x250);
            }
            if (*(float *)(p + 0x220) <= 0.0f) {
                if (*(unsigned char *)(p + 0x234) == 1) {
                    *(unsigned char *)(p + 4) = 2;
                    *(unsigned char *)(p + 5) = 3;
                    *(char *)(p + 6) = 0;
                    *(unsigned char *)(p + 0x1F0) = 0x3F;
                } else {
                    *(unsigned char *)(p + 4) = 2;
                    *(unsigned char *)(p + 5) = 1;
                    *(char *)(p + 6) = 0;
                    *(unsigned char *)(p + 0x1F0) = 0x40;
                }
            } else {
                if (*(unsigned char *)(p + 5) == 0x1D || *(unsigned char *)(p + 5) == 0x1E) {
                    o = *(char **)(p + 0x20);
                    *(float *)0x70003A20 = func_0011E620(-*(float *)(o + 0xC8), *(float *)(o + 0xC0));
                    *(float *)(p + 0xC4) = func_001B1470(1.5707964f + *(float *)0x70003A20);
                    if (*(unsigned char *)(p + 0x1F0) == 0x33) {
                        *(float *)(p + 0x27C) = *(float *)(p + 0x2E0);
                        *(float *)(p + 0x278) = *(float *)(p + 0x2E4);
                    }
                    *(unsigned char *)(p + 5) = 0x17;
                } else {
                    *(unsigned char *)(p + 5) = 0;
                }
                *(unsigned char *)(p + 4) = 2;
                *(char *)(p + 6) = 0;
                *(unsigned char *)(p + 0x1F0) = 0x3E;
                *(unsigned char *)(p + 0x1F1) = 2;
            }
            s = 0x81;
            goto tail;
        }
    try_b:
        if (func_0021C3F0() != 0 && *(unsigned char *)(p + 0) == 1 &&
            *(unsigned char *)(p + 0x23B) == 0xA) {
            if (*(unsigned char *)(p + 4) == 1 && *(unsigned char *)(p + 5) == 0) {
                goto hit_b;
            }
            anim = *(unsigned char *)(p + 5);
            if (anim == 1) {
                goto hit_b;
            }
            if (anim == 0x21 || anim == 0x22) {
                goto hit_b;
            }
            if (anim == 0x1D && *(unsigned char *)(p + 0x1F1) == 1) {
                goto hit_b;
            }
            if (anim == 0x1E && *(unsigned char *)(p + 0x1F1) == 1) {
                goto hit_b;
            }
            goto try_c;
        hit_b:
            *(unsigned char *)(p + 0) |= 2;
            *(float *)(p + 0x224) = 8.0f;
            func_0021C350(p);
            func_001EFE00(0x80000044, p);
            if (*(float *)(p + 0x220) <= 0.0f) {
                if (*(unsigned char *)(p + 0x234) == 1) {
                    *(unsigned char *)(p + 4) = 2;
                    *(unsigned char *)(p + 5) = 3;
                    *(char *)(p + 6) = 0;
                    *(unsigned char *)(p + 0x1F0) = 0x3F;
                } else {
                    *(unsigned char *)(p + 4) = 2;
                    *(unsigned char *)(p + 5) = 1;
                    *(char *)(p + 6) = 0;
                    *(unsigned char *)(p + 0x1F0) = 0x40;
                }
            } else {
                if (*(unsigned char *)(p + 5) == 0x1D || *(unsigned char *)(p + 5) == 0x1E) {
                    o = *(char **)(p + 0x20);
                    *(float *)0x70003A20 = func_0011E620(-*(float *)(o + 0xC8), *(float *)(o + 0xC0));
                    *(float *)(p + 0xC4) = func_001B1470(1.5707964f + *(float *)0x70003A20);
                    if (*(unsigned char *)(p + 0x1F0) == 0x33) {
                        *(float *)(p + 0x27C) = *(float *)(p + 0x2E0);
                        *(float *)(p + 0x278) = *(float *)(p + 0x2E4);
                    }
                    *(unsigned char *)(p + 5) = 0x17;
                } else {
                    *(unsigned char *)(p + 5) = 0;
                }
                *(unsigned char *)(p + 4) = 2;
                *(char *)(p + 6) = 0;
                *(unsigned char *)(p + 0x1F0) = 0x3E;
                *(unsigned char *)(p + 0x1F1) = 3;
            }
            s = 0x81;
            goto tail;
        }
    try_c:
        if (!*(float *)(p + 0x224) && !*(float *)(p + 0x22C)) {
            goto tail;
        }
        if (func_0021BC40(p) != 0) {
            goto tail;
        }
        if (*(float *)(p + 0x224)) {
            func_0021C350(p);
            if (*(unsigned char *)(p + 0xF) == 0xC) {
                *(unsigned char *)(p + 0x1F1) = 4;
                *(unsigned char *)(p + 0xF) = 0;
            } else {
                *(unsigned char *)(p + 0x1F1) = 0;
            }
        }
        if (*(float *)(p + 0x22C)) {
            func_0021C270(p);
            *(unsigned char *)(p + 0x1F1) = 1;
        }
        if (*(float *)(p + 0x220) <= 0.0f) {
            if (*(unsigned char *)(p + 0xF) == 0x63 || *(unsigned char *)(p + 0x234) == 1) {
                func_0021C200(p);
                *(unsigned char *)(p + 4) = 2;
                *(unsigned char *)(p + 5) = 3;
                *(char *)(p + 6) = 0;
                *(unsigned char *)(p + 0x1F0) = 0x3F;
            } else {
                *(unsigned char *)(p + 4) = 2;
                *(unsigned char *)(p + 5) = 1;
                *(char *)(p + 6) = 0;
                *(unsigned char *)(p + 0x1F0) = 0x40;
            }
        } else {
            if (*(unsigned char *)(p + 4) == 1 &&
                (*(unsigned char *)(p + 5) == 0x1D || *(unsigned char *)(p + 5) == 0x1E)) {
                o = *(char **)(p + 0x20);
                *(float *)0x70003A20 = func_0011E620(-*(float *)(o + 0xC8), *(float *)(o + 0xC0));
                *(float *)(p + 0xC4) = func_001B1470(1.5707964f + *(float *)0x70003A20);
                if (*(unsigned char *)(p + 0x1F0) == 0x33) {
                    *(float *)(p + 0x27C) = *(float *)(p + 0x2E0);
                    *(float *)(p + 0x278) = *(float *)(p + 0x2E4);
                }
                if (!(*(float *)(p + 0x228) < 100.0f) && D_008106F1 != 0) {
                    *(unsigned char *)(p + 5) = 0x18;
                } else {
                    *(unsigned char *)(p + 5) = 0x17;
                }
            } else if (!(*(float *)(p + 0x228) < 100.0f) && D_008106F1 != 0) {
                *(unsigned char *)(p + 5) = 2;
            } else {
                *(unsigned char *)(p + 5) = 0;
            }
            *(unsigned char *)(p + 4) = 2;
            *(char *)(p + 6) = 0;
            *(unsigned char *)(p + 0x1F0) = 0x3E;
        }
        s = 0x81;
    }
tail:
    if (*(float *)(p + 0x220) <= 35.0f) {
        *(unsigned char *)(p + 0x235) &= 0xFE;
        *(unsigned char *)(p + 0x235) |= 1;
    }
    if (s & 0x80) {
        func_0017C370(p);
    }
    if (s & 1) {
        *(char *)(p + 0x2F2) = 0;
        *(char *)(p + 0x274) = 0;
        *(short *)(p + 0x276) = 0;
        return 1;
    }
    return 0;
}
