// NEARMISS func_00217090  (vram 0x00217090, 0x71C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001FCF10(void);
extern void func_00207D00(int slot, int mode);
extern void func_0020A7A0(long long tag);
extern void func_0020AE40(void *arg0, void *p, int flags);
extern void func_0020B0D0(int arg0, char *arg1);
extern int func_0020B210(void *arg0, void *p, long long tag, int flags);
extern void func_0020BBE0(char *arg0, int arg1);
extern int func_0020BC50(void *arg0, void *p, long long tag, int flags);
extern void func_0020BF20(void *p, int a1, int a2);
extern void func_0020CCB0(unsigned char *p);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020CDA0(void);

extern char D_002660E0[];
extern char D_00266150[];
extern char D_00266190[];
extern int D_002821B0;
extern int D_00282240;
extern int D_002821B4;
extern int D_002821B8;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_00810C69;
extern unsigned char D_00810C6A;
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA6;
extern signed char D_00810CA7;
extern unsigned short D_00810E74;

void func_00217090(char *arg0) {
    unsigned char st;
    unsigned char cnt;
    unsigned char idx;

    st = *(unsigned char *)(arg0 + 5);

    switch (st) {
    case 0:
        *(char *)(arg0 + 0x17) = 0;
        *(unsigned char *)(arg0 + 0x19) = 0;
        *(unsigned char *)(arg0 + 0x18) = 0;
        *(unsigned char *)(arg0 + 0x1A) = 0;
        D_002821B0 = 4;
        D_002821B4 = 0;
        D_00282240 = 3;
        *(unsigned char *)(arg0 + 7) = 0;
        *(short *)(arg0 + 0x1E) = 5;

        if (D_00810CA5 != 0xFF) {
            *(unsigned char *)(arg0 + 0x12) = D_00810CA5;
        } else {
            *(unsigned char *)(arg0 + 0x12) = 0xFF;
        }

        if (D_00810C69 != 0) {
            idx = *(unsigned char *)(arg0 + 0x18);
            *(unsigned char *)(arg0 + 0x18) = idx + 1;
            arg0[idx + 0x50] = 0;
        }
        if (D_00810C6A != 0) {
            unsigned char one = 1;
            idx = *(unsigned char *)(arg0 + 0x18);
            *(unsigned char *)(arg0 + 0x18) = idx + 1;
            arg0[idx + 0x50] = one;
        }

        if (D_008106B0 != 0) {
            int i;
            *(unsigned char *)(arg0 + 0x1B) = (unsigned char) (D_008106B1 - 5);
            for (i = 0; i < *(unsigned char *)(arg0 + 0x18); i++) {
                if (*(unsigned char *)(arg0 + i + 0x50) == *(unsigned char *)(arg0 + 0x1B)) {
                    *(char *)(arg0 + 0x17) = (char) i;
                    D_00282240 = 4;
                    break;
                }
            }
            D_008106B0 = 0;
            *(unsigned char *)(arg0 + 5) = 3;
            *(unsigned char *)(arg0 + 6) = 0xF0;
            break;
        }

        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        /* fall through */
    case 1:
        if (D_00810E74 & 0x20) {
            func_0020CD60();
            D_002821B4 = 2;
            *(char *)(arg0 + 1) = 3;
            *(char *)(arg0 + 2) = 0;
            *(char *)(arg0 + 3) = 0;
            *(char *)(arg0 + 4) = 0;
            *(unsigned char *)(arg0 + 5) = 0;
        } else {
            func_0020A7A0(0x200419E5LL << 32 | 0x9D4220A0u);
            func_0020AE40(arg0, D_002660E0, 0x10);
            func_0020BF20(D_00266150, 1, 0);
            if (func_0020B210(arg0, D_00266190, 0x20041A05LL << 32 | 0xA1321F80u, 0x10) != 0) {
                *(short *)(arg0 + 0x1C) = 0;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_0020BBE0(arg0, *(unsigned char *)(arg0 + 0x1A));
            } else if (*(unsigned char *)(arg0 + 0x18) != 0 && (D_00810E74 & 0x40)) {
                if (D_00810CA5 != (unsigned char) (*(unsigned char *)(arg0 + *(unsigned char *)(arg0 + 0x17) + 0x50) + 5)) {
                    func_0020CD40();
                    *(unsigned char *)(arg0 + 5) = 4;
                    *(unsigned char *)(arg0 + 6) = 1;
                } else if (*(unsigned char *)(arg0 + 7) == 0) {
                    func_0020CD80();
                    *(unsigned char *)(arg0 + 7) = 0x10;
                }
            }
            func_0020B0D0((int) arg0, D_002660E0);
        }
        break;

    case 2:
        func_0020A7A0(0x200419E5LL << 32 | 0x9D4220A0u);
        func_0020AE40(arg0, D_002660E0, 0x10);
        func_0020BF20(D_00266150, 1, 0);
        if (func_0020BC50(arg0, D_00266190, 0x20041A05LL << 32 | 0xA1321F80u, 0x10) != 0) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) - 1;
        }
        func_0020B0D0((int) arg0, D_002660E0);
        break;

    case 3:
        func_0020A7A0(0x200419E5LL << 32 | 0x9D4220A0u);
        func_0020AE40(arg0, D_002660E0, 0x10);
        func_0020BF20(D_00266150, 1, 0);
        func_0020B210(arg0, D_00266190, 0x20041A05LL << 32 | 0xA1321F80u, 0x410);
        func_0020B0D0((int) arg0, D_002660E0);
        cnt = *(unsigned char *)(arg0 + 6) - 1;
        *(unsigned char *)(arg0 + 6) = cnt;
        if (cnt == 0 || (D_00810E74 & 0x5060)) {
            if (D_00810E74 & 0x5060) {
                func_0020CD60();
            }
            D_008106B0 = 0;
            *(unsigned char *)(arg0 + 5) = 1;
            D_00282240 = 3;
        }
        break;

    case 4:
        func_0020A7A0(0x200419E5LL << 32 | 0x9D4220A0u);
        func_0020AE40(arg0, D_002660E0, 0x10);
        func_0020BF20(D_00266150, 1, 0);
        func_0020B210(arg0, D_00266190, 0x20041A05LL << 32 | 0xA1321F80u, 0x410);
        func_0020B0D0((int) arg0, D_002660E0);
        D_002821B4 = 1;
        D_00282240 = 5;
        D_002821B8 = 4;
        func_001FCF10();
        func_00207D00(1, 3);

        if (D_00810E74 & 0x8000) {
            unsigned char v = *(unsigned char *)(arg0 + 6);
            if (v != 0) {
                *(unsigned char *)(arg0 + 6) = v - 1;
                func_0020CDA0();
            }
        } else if (D_00810E74 & 0x2000) {
            unsigned char v = *(unsigned char *)(arg0 + 6);
            if (v == 0) {
                *(unsigned char *)(arg0 + 6) = v + 1;
                func_0020CDA0();
            }
        }
        func_0020CCB0((unsigned char *) arg0);

        if (D_00810E74 & 0x40) {
            if (*(unsigned char *)(arg0 + 6) != 0) {
                D_002821B4 = 0;
                D_00282240 = 3;
                *(unsigned char *)(arg0 + 5) = 1;
                func_0020CD60();
            } else {
                unsigned char sum;
                short base;
                func_001FB9F0(0x17B, 0x1000, 0x1000, 0x1000);
                base = *(short *)(arg0 + 0x1E);
                sum = (unsigned char) (*(unsigned char *)(arg0 + *(unsigned char *)(arg0 + 0x19) + *(unsigned char *)(arg0 + 0x17) + 0x50) + base);
                D_00810CA5 = sum;
                *(unsigned char *)(arg0 + 0x12) = D_00810CA5;
                if (D_00810CA4 != 0xFF) {
                    if (D_00810CA4 == 2) {
                        D_00810CA6 = 0;
                        D_00810CA7 = 7;
                    } else {
                        D_00810CA7 = 7;
                        if (D_00810CA6 == 0xFF) {
                            D_00810CA6 = 0;
                        }
                    }
                    D_00810CA4 = 0xFF;
                }
                D_002821B4 = 2;
                *(char *)(arg0 + 1) = 3;
                *(char *)(arg0 + 2) = 0;
                *(char *)(arg0 + 3) = 0;
                *(char *)(arg0 + 4) = 0;
                *(unsigned char *)(arg0 + 5) = 0;
            }
        } else if (D_00810E74 & 0x20) {
            func_0020CD60();
            D_002821B4 = 0;
            D_00282240 = 3;
            *(unsigned char *)(arg0 + 5) = 1;
        }
        break;
    }

    cnt = *(unsigned char *)(arg0 + 7);
    if (cnt != 0) {
        *(unsigned char *)(arg0 + 7) = cnt - 1;
    }
}
