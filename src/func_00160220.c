// NEARMISS func_00160220  (vram 0x00160220, 0x5A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.81% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register-coloring permutation in the D_00810700==1 range-check arm; the other two mode arms already match byte-for-byte. Permuter territory, not the clean-store nop.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.81% (mwcc 2.3.3). Logic fully recovered; sole residual is an
// FP-register-coloring permutation in the D_00810700==1 range-check block
// (target uses $f1/$f2 for the loaded coord + $f0 for the constant; mwcc
// colors them $f4/$f5 + $f1). Permuter territory, not the clean-store nop.
//
// SEMANTICS: per-tick driver for a footprint/zone actor. Gate on
// (D_00810E74 & *0x70003B76). One-shot func_00184BA0 -> teardown anim
// (state 0x25). In mode 0x15, classify via func_001AAC00 and arm a 0x38/0x39
// cinematic. Otherwise, if the actor position (+0xB0/B4/B8) lies inside the
// per-mode axis-aligned box, mark "trigger"; else, unless suppressed
// (+0x236 / +0x23B==0x35), sweep build_trs_matrix at C4-angle +/- pi/4 and
// probe via func_0015DF10. Trigger -> func_0015EC50 / func_0015FDF0 spawn
// (state 0x24, +0x1F0=0x3A, +0=3).
extern unsigned short D_00810E74;
extern unsigned char D_00810700;
extern int func_00184BA0(void);
extern void func_001798D0(char *p);
extern int func_001AAC00(char *p, char *a, char *b);
extern int func_0015D4C0(char *p);
extern void build_trs_matrix(void *mtx, void *pos, void *rot, void *scale);
extern int func_0015DF10(char *p, int a, float c4);
extern float func_001B1470(float);
extern int func_0015EC50(char *p);
extern int func_0015FDF0(char *p);

int func_00160220(char *s0) {
    if ((D_00810E74 & *(unsigned short *)0x70003B76) != 0) {
        if (func_00184BA0() != 0) {
            func_001798D0(s0);
            *(unsigned char *)(s0 + 5) = 0x25;
            *(unsigned char *)(s0 + 6) = 0;
            return 1;
        }
        if (D_00810700 == 0x15) {
            int r = func_001AAC00(s0, s0 + 0x290, s0 + 0x218);
            if (r != 0) {
                if (r == 1) {
                    *(unsigned char *)(s0 + 0x1F0) = 0x38;
                } else if (r == 2) {
                    *(unsigned char *)(s0 + 0x1F0) = 0x39;
                    *(short *)(s0 + 0x2E) = 0;
                } else if (r == 3) {
                    *(unsigned char *)(s0 + 0x1F0) = 0x39;
                    *(short *)(s0 + 0x2E) = 1;
                }
                *(unsigned char *)(s0 + 5) = 0x23;
                *(unsigned char *)(s0 + 6) = 0;
                *(unsigned char *)(s0 + 0x1F1) = 0;
                *(unsigned char *)(s0 + 0) |= 2;
                return 1;
            }
        }

        if (func_0015D4C0(s0) != 0) {
            return 1;
        }

        if (D_00810700 == 1) {
            if (*(float *)(s0 + 0xB4) >= -40.0f && *(float *)(s0 + 0xB4) <= -20.0f) {
                if (*(float *)(s0 + 0xB0) >= -35.0f && *(float *)(s0 + 0xB0) <= 35.0f) {
                    if (*(float *)(s0 + 0xB8) >= -1050.0f && *(float *)(s0 + 0xB8) <= -990.0f) {
                        goto trigger;
                    }
                }
            }
        } else if (D_00810700 == 4) {
            if (*(float *)(s0 + 0xB4) >= 10.0f && *(float *)(s0 + 0xB4) <= 20.0f) {
                if (*(float *)(s0 + 0xB0) >= 315.0f && *(float *)(s0 + 0xB0) <= 360.0f) {
                    if (*(float *)(s0 + 0xB8) >= 315.0f && *(float *)(s0 + 0xB8) <= 385.0f) {
                        goto trigger;
                    }
                }
            }
        } else if (D_00810700 == 0xD) {
            if (*(float *)(s0 + 0xB4) >= 150.0f) {
                if (*(float *)(s0 + 0xB4) <= 210.0f) {
                    if (*(float *)(s0 + 0xB0) >= 720.0f && *(float *)(s0 + 0xB0) <= 800.0f) {
                        if (*(float *)(s0 + 0xB8) >= 720.0f && *(float *)(s0 + 0xB8) <= 840.0f) {
                            goto trigger;
                        }
                    }
                }
            }
            if (*(float *)(s0 + 0xB4) >= 150.0f && *(float *)(s0 + 0xB4) <= 215.0f) {
                if (*(float *)(s0 + 0xB0) >= 635.0f && *(float *)(s0 + 0xB0) <= 720.0f) {
                    if (*(float *)(s0 + 0xB8) >= 1270.0f && *(float *)(s0 + 0xB8) <= 1325.0f) {
                        goto trigger;
                    }
                }
            }
        }

        if (*(unsigned char *)(s0 + 0x236) == 0 && *(unsigned char *)(s0 + 0x23B) != 0x35) {
            float a = *(float *)(s0 + 0xC4);
            build_trs_matrix(s0 + 0xD0, s0 + 0xB0, s0 + 0xC0, s0 + 0x60);
            if (func_0015DF10(s0, 0, *(float *)(s0 + 0xC4)) != 0) {
                return 1;
            }
            *(float *)(s0 + 0xC4) = func_001B1470(a - 0.7853982f);
            build_trs_matrix(s0 + 0xD0, s0 + 0xB0, s0 + 0xC0, s0 + 0x60);
            if (func_0015DF10(s0, 1, *(float *)(s0 + 0xC4)) != 0) {
                return 1;
            }
            *(float *)(s0 + 0xC4) = func_001B1470(a + 0.7853982f);
            build_trs_matrix(s0 + 0xD0, s0 + 0xB0, s0 + 0xC0, s0 + 0x60);
            if (func_0015DF10(s0, 1, *(float *)(s0 + 0xC4)) != 0) {
                return 1;
            }
            *(float *)(s0 + 0xC4) = a;
            build_trs_matrix(s0 + 0xD0, s0 + 0xB0, s0 + 0xC0, s0 + 0x60);
        }

    trigger:
        if (*(unsigned char *)(s0 + 0x236) == 0 && *(unsigned char *)(s0 + 0x23B) != 0x35) {
            if (func_0015EC50(s0) != 0) {
                return 1;
            }
        }
        if (func_0015FDF0(s0) != 0) {
            *(unsigned char *)(s0 + 5) = 0x24;
            *(unsigned char *)(s0 + 6) = 0;
            *(unsigned char *)(s0 + 0x1F0) = 0x3A;
            *(unsigned char *)(s0 + 0) = 3;
            return 1;
        }
    }
    return 0;
}
