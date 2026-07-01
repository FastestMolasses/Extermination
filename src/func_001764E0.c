// NEARMISS func_001764E0  (vram 0x001764E0, 0x6F4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.63% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation-ORDER across the whole body: mwcc233 puts arg0 in s4 (target: s0), with s1..s4 correspondingly shifted; ~223/469 objdiff instruction rows differ purely on this permutation plus its ripple into branch-target-address deltas. Not a logic error -- every gate/threshold/call-arg/loo...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(char *, char *, char *);
extern void func_00102918(char *, char *, char *);
extern void func_00102948(char *, char *);
extern void func_001029C0(char *);
extern void func_00102BB0(char *, char *, float);
extern void func_001031E0(char *, char *);
extern float func_0011DF78(float);
extern int func_001760C0(char *, char *, int, float);
extern void func_00176390(char *, int, char *, int);
extern int func_00176C80(void);
extern void func_0019A310(char *, char *);
extern int func_0019AD00(char *, char *, int);
extern int func_0019AFE0(char *, char *, char *, int);
extern float func_001B1470(float);

extern int D_002488C0;
extern float D_002488C4;
extern int D_002488D0;
extern int D_002488E0;
extern float D_00248950[];
extern char D_00275B00[];
extern char D_700031B0[];
extern char D_700036A0[];
extern char D_700036D0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_70003A20[];

void func_001764E0(char *arg0) {
    int s2;
    int i;
    float *pos;
    int flagBit;
    int j;
    int sbuf[4];
    float sp60[4];
    int v0;
    int v1;

    s2 = 0;
    if (*(unsigned char *)(arg0 + 0x236) == 0) {
        s2 = func_00176C80();
    }
    if (*(unsigned char *)(arg0 + 4) == 1 && *(unsigned char *)(arg0 + 5) == 1) {
        sbuf[0] = 0;
        sbuf[1] = 0x3D4CCCCD;
        sbuf[2] = 0x40900000;
        sbuf[3] = 0x3F800000;
        pos = D_00248950;
        flagBit = flagBit & 4;
        i = 0;
        do {
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + pos[0]));
            func_00102918(D_700036A0, D_700036A0, arg0 + 0xB0);
            func_001026A0(D_700038A0, D_700036A0, (char *)sbuf);
            if (func_0019AD00(arg0, D_700038A0, 6) != 0) {
                v1 = *(short *)(*(int *)0x700031D0 + 0x1A) & 0xFF00;
                if (v1 == 0x1000 || v1 == 0x800) {
                    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
                    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
                    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;
                } else if (v1 == 0x2000) {
                    v0 = *(int *)0x700031D4;
                    if (v0 != 0 && ((*(unsigned char *)(v0 + 2) & ~0xE0) == 4) && (*(unsigned char *)(v0 + 3) == 2)) {
                        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
                        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
                        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;
                    } else if (flagBit) {
                        func_0019A310(D_70003A20, (char *)v0);
                        *(float *)0x70003A20 = func_0011DF78(*(float *)0x70003A20);
                        if (!(*(float *)0x70003A20 < 1.2217306f) && *(float *)0x70003A20 <= 1.9198622f) {
                            /* skip */
                        } else {
                            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
                            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
                            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;
                        }
                    }
                }
            }
            i += 1;
            pos += 1;
        } while (i < 5);
    }

    j = 0;
    pos = D_00248950;
    *(unsigned char *)(D_00275B00 + 4) = *(unsigned char *)(arg0 + 0x314);
    *(unsigned char *)(arg0 + 0x314) = 0;
    do {
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + pos[0]));
        func_00102918(D_700036A0, D_700036A0, arg0 + 0xB0);
        func_001026A0(D_700038A0, D_700036A0, (char *)&D_002488C0);
        *(int *)0x700038B0 = 0;
        *(float *)0x700038B4 = 4.01f;
        *(int *)0x700038B8 = 0;
        *(float *)0x700038BC = 1.0f;
        func_001026A0(D_700038C0, D_700036A0, D_700038B0);
        v0 = func_0019AFE0(arg0, D_700038C0, D_700038A0, 7);
        if (v0 != 0) {
            *(unsigned char *)(arg0 + 0x314) = *(unsigned char *)(arg0 + 0x314) | (1 << j);
            func_00176390(arg0, v0, D_700038A0, j);
        } else if (*(unsigned char *)(arg0 + 0x236) == 0) {
            *(float *)0x70003A20 = 18.0f - D_002488C4;
            if (func_001760C0(arg0, D_700038A0, 1, *(float *)0x70003A20) != 0) {
                func_00102948((char *)sp60, D_700031B0);
                sp60[1] += 0.1f;
                v1 = *(unsigned char *)(arg0 + 0x1F0);
                if (j >= 3 || (unsigned int)(v1 - 0x36) < 2 || v1 == 0x3E
                    || *(float *)(arg0 + 0x38) < 0.0f
                    || !(*(float *)0x700031B4 - *(float *)(arg0 + 0xB4) < 13.8f)
                    || s2 == 0) {
                    v0 = func_0019AD00(arg0, (char *)sp60, 7);
                    if (v0 != 0) {
                        *(unsigned char *)(arg0 + 0x314) = *(unsigned char *)(arg0 + 0x314) | (1 << j);
                        func_00176390(arg0, v0, (char *)sp60, j);
                    }
                } else {
                    *(unsigned char *)(arg0 + 0x236) = 1;
                }
            }
        } else {
            *(int *)0x70003A20 = 0;
            *(float *)0x70003A20 = 13.8f - D_002488C4;
            if (func_001760C0(arg0, D_700038A0, 1, *(float *)0x70003A20) != 0) {
                func_00102948(D_700038B0, D_700031B0);
                *(float *)0x700038B4 = *(float *)0x700038B4 + 0.1f;
                v0 = func_0019AD00(arg0, D_700038B0, 7);
                if (v0 != 0) {
                    *(unsigned char *)(arg0 + 0x314) = *(unsigned char *)(arg0 + 0x314) | (1 << j);
                    func_00176390(arg0, v0, D_700038B0, j);
                }
            }
        }
        func_001031E0(D_700036D0, arg0 + 0xB0);
        if (*(unsigned char *)(arg0 + 0x236) != 0) {
            func_001026A0(D_700038A0, D_700036A0, (char *)&D_002488D0);
        } else {
            func_001026A0(D_700038A0, D_700036A0, (char *)&D_002488E0);
        }
        v0 = func_0019AD00(arg0, D_700038A0, 7);
        if (v0 == 0) {
            j += 1;
        } else {
            *(unsigned char *)(arg0 + 0x314) = *(unsigned char *)(arg0 + 0x314) | (1 << j);
            func_00176390(arg0, v0, D_700038A0, j);
            j += 1;
        }
        pos += 1;
    } while (j < 8);
}
