// NEARMISS func_001D0720  (vram 0x001D0720, 0x548 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.72% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two-phase particle-alpha/color-blend state machine (states 0-3 twice, on fields s0+0x70 and s1+8), plus a 6-slot random re-roll + ease-toward-target loop. Logic and structure are fully recovered and byte-match almost everywhere; residual is a genuine compiler-scheduling artifact: (1) one delay-sl...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_00122BB8(void);
extern int D_002513B0[4];
extern int D_002513C0[4];

void func_001D0720(char *arg0) {
    char *s0;
    char *s1;
    int a2;
    int a2b;
    int v1;
    int s2;
    int s4;
    int t0;
    char *a2p;
    char *a3p;
    float f4;
    float f0;
    float f1;
    float f2;

    s0 = *(char **)(arg0 + 0x90);
    s1 = s0 + 0x70;
    a2 = *(int *)(s0 + 0x70);
    if (a2 != 3) {
        switch (a2) {
        case 0:
            *(int *)(s1 + 4) = D_002513B0[(func_00122BB8() >> 16) & 3];
            *(int *)(s1 + 0) = 1;
            break;
        case 1:
            v1 = *(int *)(s1 + 4) - 1;
            *(int *)(s1 + 4) = v1;
            if (v1 <= 0) {
                *(int *)(s1 + 0) = 2;
            }
            break;
        case 2:
            f2 = *(float *)(s0 + 0x40);
            f1 = f2 + (0.4f * (1.0f - f2));
            *(float *)(s0 + 0x40) = f1;
            if (!(f1 <= 0.95f)) {
                *(float *)(s0 + 0x40) = 1.0f;
                *(int *)(s1 + 0) = 3;
            }
            break;
        }
    } else {
        f2 = *(float *)(s0 + 0x40);
        f1 = f2 + (0.4f * -f2);
        *(float *)(s0 + 0x40) = f1;
        if (f1 < 0.05f) {
            *(float *)(s0 + 0x40) = 0.0f;
            *(int *)(s1 + 0) = 0;
        }
    }

    a2b = *(int *)(s1 + 8);
    if (a2b != 4) {
        switch (a2b) {
        case 0:
            if (*(unsigned char *)(s1 + 0x10) == 0) {
                v1 = (((func_00122BB8() >> 16) * 90) >> 15);
                *(int *)(s1 + 0xC) = v1 + 0x3C;
            } else {
                *(int *)(s1 + 0xC) = D_002513C0[(func_00122BB8() >> 16) & 3];
            }
            *(int *)(s1 + 8) = 1;
            break;
        case 1:
            v1 = *(int *)(s1 + 0xC) - 1;
            *(int *)(s1 + 0xC) = v1;
            if (v1 <= 0) {
                if (*(float *)(s0 + 0x44) < 0.5f) {
                    *(int *)(s1 + 8) = 2;
                } else {
                    *(int *)(s1 + 8) = 3;
                }
            }
            break;
        case 2:
            f2 = *(float *)(s0 + 0x44);
            f1 = f2 + (0.1f * (1.0f - f2));
            *(float *)(s0 + 0x44) = f1;
            if (!(f1 <= 0.95f)) {
                *(float *)(s0 + 0x44) = 1.0f;
                *(int *)(s1 + 8) = 0;
            }
            break;
        case 3:
            f2 = *(float *)(s0 + 0x44);
            f1 = f2 + (0.1f * -f2);
            *(float *)(s0 + 0x44) = f1;
            if (f1 < 0.05f) {
                *(float *)(s0 + 0x44) = 0.0f;
                *(int *)(s1 + 8) = 0;
            }
            break;
        }
    } else if (*(unsigned char *)(s1 + 0x10) != 0) {
        *(int *)(s1 + 8) = 0;
    }

    a2p = s0 + 4;
    if (*(unsigned char *)(s1 + 0x10) != 0) {
        v1 = *(int *)(s1 + 0x14) - 1;
        *(int *)(s1 + 0x14) = v1;
        if (v1 < 0) {
            int prev = *(int *)(s1 + 0x1C);
            s2 = (func_00122BB8() >> 24) % 5;
            if (s2 == prev) {
                s2 = (prev + 1) % 5;
            }
            s4 = 1;
            if (s2 == *(int *)(s1 + 0x18)) {
                char *p;
                v1 = 1;
                p = s1 + 4;
                do {
                    if (v1 == *(int *)(s1 + 0x1C)) {
                        *(float *)(p + 0x20) = 0.0f;
                    } else {
                        *(float *)(p + 0x20) = *(float *)(p + 0x20) * 0.1f;
                    }
                    v1 += 1;
                    p += 4;
                } while (v1 < 6);
            } else {
                a3p = s1 + 4;
                do {
                    f0 = (float)((func_00122BB8() >> 24) & 0x7F);
                    if (s4 != s2) {
                        f0 = *(float *)(a3p + 0x20) * (0.1f + (f0 / 256.0f));
                    } else {
                        f0 = 0.8f + (f0 / 512.0f);
                    }
                    *(float *)(a3p + 0x20) = f0;
                    s4 += 1;
                    a3p += 4;
                } while (s4 < 6);
            }
            *(int *)(s1 + 0x1C) = *(int *)(s1 + 0x18);
            *(int *)(s1 + 0x18) = s2;
            v1 = *(unsigned char *)(s1 + 0x11);
            if (v1 != 2) {
                if (v1 != 1) {
                    v1 = ((func_00122BB8() >> 16) % 5) + 3;
                } else {
                    v1 = ((func_00122BB8() >> 16) & 7) + 3;
                }
            } else {
                v1 = ((func_00122BB8() >> 16) & 0xF) + 3;
            }
            *(int *)(s1 + 0x14) = v1;
        }
        a2p = s0 + 4;
    }

    t0 = 1;
    a3p = s1 + 4;
    do {
        v1 = *(unsigned char *)(s1 + 0x11);
        f4 = *(float *)(a2p + 0x44);
        if (v1 != 2) {
            if (v1 != 1) {
                f0 = 0.4f * (*(float *)(a3p + 0x20) - f4);
            } else {
                f0 = 0.3f * (*(float *)(a3p + 0x20) - f4);
            }
        } else {
            f0 = 0.2f * (*(float *)(a3p + 0x20) - f4);
        }
        t0 += 1;
        *(float *)(a2p + 0x44) = f4 + f0;
        a2p += 4;
        a3p += 4;
    } while (t0 < 6);
}
