// NEARMISS func_0022EEF0  (vram 0x0022EEF0, 0xDA8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.12% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + scheduling artifacts, 31 of 876 instructions. (a) 4x: the read-modify-write sites on D_008106C8 load the old value into $a2 in the target vs the equally-dead $a0 here — the target consistently skips the argument registers used by the preceding call in the same basic block (sam...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS residual (mwcc 2.3.3, objdiff 99.12%): 31 of 876 instructions
// differ, all register-coloring / scheduling artifacts — (a) the four
// read-modify-write sites on D_008106C8 load the old value into $a2 in the
// target and into the equally-dead $a0 here (the target skips the argument
// registers used by the preceding call in the same block); (b) FP coloring
// in the three track guards and the 0x84 terminator test ($f1 vs $f0/$f2 for
// the time cursor); (c) which of the two ready ops fills the guard's branch
// delay slot (the cursor bump vs the following load); (d) the order of the
// two independent D_008105F4/D_008105F8 stores. Logic, control flow, call
// signatures and constants are exact; source-order, operand-order and
// temp-hoisting shapes were all tried and mwcc 2.3.3 normalises them away.
//
// Cutscene/demo timeline driver, ticked once per frame with the playback
// record d. d+0x6E is the scene id, d+0x74 the current time cursor (frames)
// and d+0x78 the scene length. The leading switch is the per-scene script:
// at hard-coded timestamps it fires camera/effect cues via func_001B1E20
// (cue id, duration) and drives the letterbox/fade flag word D_008106C8
// (mask 0xF1FFFF8F, bit 0x02000000 = wide bars, 0x10, 0x40).
//
// After the script, three independent event tracks are advanced, each a
// float[] cursor terminated by 0.0f: d+0x7C (audio ducking, state d+0x88),
// d+0x80 (subtitle/portrait pair, state d+0x89 — negative leading values
// -1/-2/-3/-4 are speaker-select records) and d+0x84 (letterbox toggle,
// state d+0x8A). Each track fires when the time cursor reaches its next
// timestamp, then steps the cursor (clearing it at the terminator).
//
// Finally func_001C7C00 samples the camera spline D_008234C0 at the cursor;
// on failure the cursor snaps to the end. Once the cursor reaches d+0x78 the
// scene tears down (func_001B0250, audio off, letterbox off, fov 480) and
// the view matrix D_008105F0 is reset. Otherwise the sampled camera
// eye/target (D_008234C0..D8) is copied to D_008105D0/E0, turned into a
// view matrix (func_001DD980 + roll about D_70003400) and the fov is set
// from the sampled fov D_008234DC clamped to [0.5, 45] degrees; the time
// cursor then advances half a frame.
extern int float_to_int(float f);
extern void func_001026A0(int *m, float *q, float *v);
extern void func_001029C0(float *q);
extern void func_00102B08(float *dst, float *src, float ang);
extern float func_0011E0A8(float *p, float f);
extern float func_0011E398(float f);
extern void func_001AEDE0(int a, int b);
extern void func_001AEE10(int a, int b);
extern void func_001B0250(void);
extern void func_001B1E20(int cue, int dur);
extern void func_001C1DC0(void);
extern int func_001C7C00(float *spline, int n, float t);
extern void func_001D25F0(float fov);
extern void func_001D2830(int a, int b);
extern void func_001DD980(float *eye, float *at);
extern void func_0021B9A0(int a, float x, float y);
extern void func_0021BA70(long long v);
extern void func_0021BA80(int r, int g, int b);
extern long long D_00275C98;
extern float D_008105D0[];
extern float D_008105D4[];
extern float D_008105D8[];
extern float D_008105E0[];
extern float D_008105E4[];
extern float D_008105E8[];
extern int D_008105F0[];
extern int D_008105F4[];
extern int D_008105F8[];
extern int D_008105FC[];
extern unsigned int D_008106C8[];
extern float D_008234C0[];
extern float D_008234C4[];
extern float D_008234C8[];
extern float D_008234CC[];
extern float D_008234D0[];
extern float D_008234D4[];
extern float D_008234D8[];
extern float D_008234DC[];
extern float D_70003400[];
extern float D_70003600[];
extern float D_70003A24[];

void func_0022EEF0(unsigned char *d)
{
    float *p;
    float t;
    float v;
    int n;
    unsigned char f;

    switch (*(short *)(d + 0x6E)) {
    case 15:
        if (*(float *)(d + 0x74) == 1330.0f) {
            func_001B1E20(6, 0);
        }
        break;
    case 22:
        t = *(float *)(d + 0x74);
        if (t == 573.0f) {
            func_001B1E20(7, 0x32);
        } else if (t == 648.0f) {
            func_001B1E20(3, 0xA);
        }
        break;
    case 24:
        if (*(float *)(d + 0x74) == 105.0f) {
            func_001B1E20(6, 0);
        }
        t = *(float *)(d + 0x74);
        if ((t >= 0.0f && t <= 263.0f) || (t >= 384.0f && t <= 655.0f) ||
            (t >= 910.0f && t <= 1159.0f)) {
            D_008106C8[0] &= 0xF1FFFF8F;
            D_008106C8[0] |= 0x02000000;
        } else {
            D_008106C8[0] &= 0xF1FFFF8F;
            D_008106C8[0] |= 0x10;
        }
        break;
    case 25:
        t = *(float *)(d + 0x74);
        if (t == 230.0f) {
            func_001B1E20(2, 0x32);
        } else if (t == 316.0f) {
            func_001B1E20(3, 0xA0);
        }
        break;
    case 26:
        if (*(float *)(d + 0x74) == 2670.0f) {
            D_008106C8[0] |= 0x40;
            func_001D2830(8, 0);
            func_001C1DC0();
        }
        break;
    case 27:
        t = *(float *)(d + 0x74);
        if (t == 2431.0f) {
            func_001B1E20(1, 0x28);
        } else if (t == 3491.0f) {
            func_001B1E20(5, 0xA0);
        } else if (t == 3580.0f) {
            func_001B1E20(8, 0x46);
        } else if (t >= 3852.0f && t < 3960.0f) {
            *(float *)0x70003A20 = t - 3852.0f;
            *(float *)0x70003A28 =
                func_0011E0A8(D_70003A24, *(float *)0x70003A20 = *(float *)0x70003A20 / 10.0f);
            if (*(float *)0x70003A28 == 0.0f) {
                func_001B1E20(6, 5);
            }
        } else if (t == 4260.0f) {
            func_001B1E20(4, 0x32);
        } else if (t == 4508.0f) {
            func_001B1E20(2, 0xF);
        } else if (t == 4609.0f) {
            func_001B1E20(2, 0xF);
        }
        break;
    case 29:
        if (*(float *)(d + 0x74) == 777.0f) {
            func_001B1E20(2, 0x32);
        }
        if (*(float *)(d + 0x74) == 0.0f) {
            D_008106C8[0] &= 0xF1FFFF8F;
            D_008106C8[0] |= 0x02000000;
        }
        break;
    case 30:
        t = *(float *)(d + 0x74);
        if (t == 31.0f) {
            func_001B1E20(2, 0x50);
        } else if (t == 75.0f) {
            func_001B1E20(1, 0x334);
        }
        break;
    case 31:
        if (*(float *)(d + 0x74) == 1.0f) {
            func_001B1E20(4, 0x30C);
        }
        break;
    case 33:
        t = *(float *)(d + 0x74);
        if ((t >= 200.0f && t <= 300.0f) || (t >= 360.0f && t <= 560.0f)) {
            D_008106C8[0] &= 0xF1FFFF8F;
            D_008106C8[0] |= 0x02000000;
        } else if (t >= 635.0f && t <= 700.0f) {
            D_008106C8[0] &= 0xF1FFFF8F;
        } else {
            D_008106C8[0] &= 0xF1FFFF8F;
            D_008106C8[0] |= 0x02000000;
        }
        break;
    case 34:
        if (*(float *)(d + 0x74) == 1.0f) {
            func_001B1E20(6, 0);
        }
        break;
    case 35:
        if (*(float *)(d + 0x74) == 50.0f) {
            func_001B1E20(6, 0);
        }
        break;
    }

    p = *(float **)(d + 0x7C);
    if (p != 0 && *(float *)(d + 0x74) >= p[0]) {
        p = p + 1;
        if (d[0x88] == 0) {
            v = *p++;
            if (v == -1.0f) {
                func_0021BA80(0x80, 0x80, 0x80);
                d[0x88] = 2;
            } else {
                func_0021B9A0(5, 0.0f, v);
                d[0x88] = 1;
            }
        } else {
            if (d[0x88] == 1) {
                func_0021B9A0(0, 0.0f, 0.0f);
            } else {
                func_0021BA70(D_00275C98);
            }
            d[0x88] = 0;
        }
        if (p[0] == 0.0f) {
            *(float **)(d + 0x7C) = 0;
        } else {
            *(float **)(d + 0x7C) = p;
        }
    }

    p = *(float **)(d + 0x80);
    if (p != 0) {
        t = p[0];
        if (t < 0.0f) {
            if (t == -3.0f) {
                d[0x89] = 0;
            } else if (t == -4.0f) {
                d[0x89] = 0x80;
            } else if (t == -1.0f) {
                d[0x89] = 1;
            } else if (t == -2.0f) {
                d[0x89] = 0x81;
            }
            *(float **)(d + 0x80) = *(float **)(d + 0x80) + 1;
        } else if (*(float *)(d + 0x74) >= t) {
            p = p + 1;
            f = d[0x89];
            if (f & 0x80) {
                if (f & 1) {
                    func_001AEDE0(float_to_int(*p++), 1);
                } else {
                    func_001AEE10(float_to_int(*p++), 1);
                }
            } else {
                if (f & 1) {
                    func_001AEDE0(float_to_int(*p++), 0);
                } else {
                    func_001AEE10(float_to_int(*p++), 0);
                }
            }
            f = d[0x89];
            n = 1 - (f & 1);
            d[0x89] = (f & 0x80) + n;
            if (p[0] == 0.0f) {
                *(float **)(d + 0x80) = 0;
            } else {
                *(float **)(d + 0x80) = p;
            }
        }
    }

    p = *(float **)(d + 0x84);
    if (p != 0 && *(float *)(d + 0x74) >= p[0]) {
        p = p + 1;
        if (d[0x8A] == 0) {
            func_001D2830(2, 1);
        } else {
            func_001D2830(2, 0);
        }
        d[0x8A] = 1 - d[0x8A];
        if (p[0] == 0.0f) {
            *(float **)(d + 0x84) = 0;
        } else {
            *(float **)(d + 0x84) = p;
        }
    }

    if (func_001C7C00(D_008234C0, *(int *)(d + 0x70), *(float *)(d + 0x74)) == 0) {
        *(float *)(d + 0x74) = *(float *)(d + 0x78);
    }
    if (!(*(float *)(d + 0x74) < *(float *)(d + 0x78))) {
        func_001B0250();
        func_0021B9A0(0, 0.0f, 0.0f);
        func_001D2830(2, 0);
        func_001D25F0(480.0f);
        D_008105F0[0] = 0;
        D_008105F4[0] = 0xBF800000;
        D_008105F8[0] = 0;
        D_008105FC[0] = 0x3F800000;
        return;
    }
    D_008105D0[0] = D_008234C0[0];
    D_008105D4[0] = D_008234C4[0];
    D_008105D8[0] = D_008234C8[0];
    D_008105E0[0] = D_008234CC[0];
    D_008105E4[0] = D_008234D0[0];
    D_008105E8[0] = D_008234D4[0];
    func_001DD980(D_008105D0, D_008105E0);
    func_001029C0(D_70003400);
    func_00102B08(D_70003400, D_70003400, (3.1415927f * D_008234D8[0]) / 180.0f);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0xBF800000;
    *(int *)0x70003608 = 0;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_008105F0, D_70003400, D_70003600);
    v = D_008234DC[0];
    if (v > 45.0f) {
        v = 45.0f;
    } else if (v < 0.5f) {
        v = 0.5f;
    }
    func_001D25F0(224.0f / func_0011E398((3.1415927f * (v / 1.45f)) / 180.0f));
    v = 0.5f;
    *(float *)(d + 0x74) = *(float *)(d + 0x74) + v;
}
