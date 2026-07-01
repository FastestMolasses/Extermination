// NEARMISS func_0019CB60  (vram 0x0019CB60, 0x3F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.49% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Logic fully recovered from raw disasm: the two float-compare flag setups (D_70003190<=D_700031A0, D_70003198<=D_700031A8 packing flagA/flagB with 1/2/0x10/0x20), the func_0019F1A0 double-dispatch, the local[4] short->int widen loop, the 6-iteration best-candidate scan over D_70003228/D_70003240 w...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019ED80(char *a0, char *a1);
extern void func_0019F1A0(char *arg0, int arg1);

extern int D_70003210[2];
extern int D_70003228[6];
extern short D_70003240[6];

int func_0019CB60(void) {
    float local[4];
    short *pT6;
    int *pT3;
    int *pS0;
    int i;
    int flagA, flagB;
    int lo, hi, best;
    int cnt;
    int status;
    short *s0;
    int *dst;
    short *src;
    char *e;
    int a0v;
    int ok;
    int j;

    status = 0;
    if (!(*(float *)0x70003190 <= *(float *)0x700031A0)) {
        flagA = 1;
        flagB = 2;
    } else {
        flagA = 2;
        flagB = 1;
    }
    if (!(*(float *)0x70003198 <= *(float *)0x700031A8)) {
        flagA |= 0x10;
        flagB |= 0x20;
    } else {
        flagA |= 0x20;
        flagB |= 0x10;
    }

    func_0019F1A0((char *)0x70003190, flagB);
    func_0019F1A0((char *)0x700031A0, flagA);

    dst = (int *)local;
    src = D_70003240;
    i = 0;
    do {
        i += 1;
        *dst = (int)*src;
        src += 2;
        dst += 4;
    } while (i < 6);

    func_0019F1A0((char *)0x70003190, flagA);
    func_0019F1A0((char *)0x700031A0, flagB);

    cnt = *(int *)0x7000320C;
    lo = 0;
    hi = 0;
    best = 0;

    pT6 = D_70003240;
    pT3 = D_70003228;
    pS0 = (int *)local;
    for (i = 0; i < 6; i++) {
        if ((unsigned int)(i - 2) >= 2u) {
            if (i & 1) {
                *(short *)0x70003B86 = *pT6;
                *(short *)0x70003B88 = (short) * (short *)(*pT3 + (*pS0 * 2));
            } else {
                *(short *)0x70003B86 = (short) * (short *)(*pT3 + (*pS0 * 2));
                *(short *)0x70003B88 = *pT6;
                *(short *)0x70003B88 = *(short *)0x70003B88 + 1;
            }
            {
                int a3 = *(short *)0x70003B88;
                int a2 = *(short *)0x70003B86;
                int t0 = a3 - a2;
                if (t0 < cnt) {
                    cnt = t0;
                    lo = a2;
                    hi = a3;
                    best = i;
                }
            }
        }
        pT6 += 2;
        pT3 += 4;
        pS0 += 4;
    }

    s0 = (short *)(D_70003210[best] + lo * 2);
    if (lo < hi) {
        do {
            e = (char *)(*(int *)0x70003208 + (*s0 << 6));
            s0 += 2;
            if (*(short *)0x70003240 >= *(short *)(e + 0xC) &&
                *(short *)(e + 0xE) >= *(short *)0x70003242 &&
                *(short *)0x70003248 >= *(short *)(e + 0x14) &&
                *(short *)(e + 0x16) >= *(short *)0x7000324A) {
                *(short *)0x70003B88 = *(unsigned char *)(e + 0x1A);
                a0v = *(short *)0x70003B88;
                if (a0v < 0x5A) {
                    ok = 0;
                    if (a0v == 0x51) {
                        if (*(short *)0x7000324E == 0) {
                            ok = 1;
                        }
                    } else if (a0v == 0x52) {
                        if (*(short *)0x7000324E == 2) {
                            ok = 1;
                        }
                    } else {
                        if (a0v != 0x53 || *(short *)0x7000324E != -1) {
                            ok = 1;
                        }
                    }
                    if (ok && func_0019ED80((char *)0x70003190, e) != 0) {
                        *(float *)0x700031A0 = *(float *)0x700031B0;
                        *(float *)0x700031A8 = *(float *)0x700031B8;
                        for (j = 0; j < 3; j++) {
                            local[j] = *(float *)(0x70003190 + j * 4 + 0x20);
                        }
                        status = *(int *)0x700031D0;
                    }
                }
            }
            lo += 1;
        } while (lo < hi);
    }

    if (status != 0) {
        *(int *)0x700031D0 = status;
        for (j = 0; j < 3; j++) {
            *(float *)(0x70003190 + j * 4 + 0x20) = local[j];
        }
        return 0;
    }
    return 1;
}
