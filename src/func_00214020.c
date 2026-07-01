// NEARMISS func_00214020  (vram 0x00214020, 0x548 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// UI/screen-state manager: a 5-state (0-4) machine gating sound-effect selection (func_00213F30), audio SFX ID cycling on trigger flags 0x2000/0x8000, and a ring-buffer id table (D_002659C0). Body fully recovered; several calls to func_00213F30/func_0020CD40/func_0020CDA0 take FEWER args than their...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001FB9F0(int sel, int size, int a2, int a3);
extern void func_0020A7A0(unsigned long long arg0);
extern int func_0020BEF0(unsigned char *a0);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020CDA0(void);
extern void func_002131B0(unsigned char *p, int arg1);
extern void func_002134C0(unsigned char *p, int arg1);
extern int func_00213A00(char *arg0, int arg1);
extern void func_00213C50(unsigned char *p, int sel);
extern int func_00213CC0(char *arg0);
void func_00213F30();

extern int D_002659C0[];
extern int D_002821B0;
extern int D_002821B4;
extern int D_002821B8;
extern int D_00282240;
extern int D_00282244;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned short D_00810E74;
extern unsigned short D_00810E78;

void func_00214020(char *arg0) {
    char *s0;
    int i;

    s0 = arg0;
    switch (*(unsigned char *)(s0 + 3)) {
    case 0:
        *(char *)(s0 + 0x17) = 0;
        *(unsigned char *)(s0 + 0x19) = 0;
        *(unsigned char *)(s0 + 0x1A) = 0;
        *(short *)(s0 + 0x1E) = 0;
        D_002821B0 = 4;
        D_002821B4 = 0;
        D_00282240 = 0x64;
        if (D_008106B0 != 0) {
            int v1;
            *(unsigned char *)(s0 + 0x1B) = D_008106B1;
            v1 = *(unsigned char *)(s0 + 0x1B);
            if (v1 < 0x20) {
                *(unsigned char *)(s0 + 0x12) = 0;
            } else if (v1 < 0x32) {
                *(unsigned char *)(s0 + 0x12) = 1;
            } else if (v1 < 0x48) {
                *(unsigned char *)(s0 + 0x12) = 2;
            } else if (v1 < 0x5D) {
                *(unsigned char *)(s0 + 0x12) = 3;
            } else {
                *(unsigned char *)(s0 + 0x12) = 4;
            }
            func_00213F30(s0);
            for (i = 0; i < (int)*(unsigned char *)(s0 + 0x18); i++) {
                if (*(unsigned char *)(s0 + i + 0x50) == *(unsigned char *)(s0 + 0x1B)) {
                    unsigned char temp_a0;
                    *(unsigned char *)(s0 + 0x19) = (unsigned char)(i - 4);
                    temp_a0 = *(unsigned char *)(s0 + 0x19);
                    if (((long long)((long long)temp_a0 << 0x38) >> 0x38) < 0) {
                        *(unsigned char *)(s0 + 0x19) = *(unsigned char *)(s0 + 0x18) + temp_a0;
                    }
                    *(char *)(s0 + 0x17) = 4;
                    D_00282240 = 0x64;
                    break;
                }
            }
            D_00282244 = 0;
            D_002821B8 = *(unsigned char *)(s0 + 0x1B);
            *(short *)(s0 + 0x1C) = 0;
            *(unsigned char *)(s0 + 3) = 4;
            D_008106B0 = 0;
            break;
        }
        *(unsigned char *)(s0 + 3) = *(unsigned char *)(s0 + 3) + 1;
        *(unsigned char *)(s0 + 0x12) = 0;
        func_00213F30(s0);
        /* fall through */
    case 1:
        if (D_00810E74 & 0x20) {
            func_0020CD60();
            *(char *)(s0 + 0x10) = 0x63;
            D_002821B4 = 0;
            break;
        }
        func_0020A7A0(((unsigned long long)0x200450A5 << 0x20) | ((unsigned long long)0x9D42 << 0x10) | 0x2200);
        if (D_00810E78 & 0x2000) {
            *(unsigned char *)(s0 + 0x12) = *(unsigned char *)(s0 + 0x12) + 1;
            if ((int)*(unsigned char *)(s0 + 0x12) >= 5) {
                *(unsigned char *)(s0 + 0x12) = 0;
            }
            func_001FB9F0(0xF, 0x1000, 0x1000, 0x1000);
            func_00213F30(s0);
        } else if (D_00810E78 & 0x8000) {
            *(unsigned char *)(s0 + 0x12) = *(unsigned char *)(s0 + 0x12) - 1;
            if ((signed char)*(unsigned char *)(s0 + 0x12) < 0) {
                *(unsigned char *)(s0 + 0x12) = 4;
            }
            func_001FB9F0(0xF, 0x1000, 0x1000, 0x1000);
            func_00213F30(s0);
        }
        func_002131B0((unsigned char *)s0, 0);
        func_002134C0((unsigned char *)s0, 0);
        if (func_00213A00(s0, 0) != 0) {
            *(short *)(s0 + 0x1C) = 0;
            *(unsigned char *)(s0 + 3) = *(unsigned char *)(s0 + 3) + 1;
            func_00213C50((unsigned char *)s0, *(unsigned char *)(s0 + 0x1A));
            break;
        }
        if (D_00810E74 & 0x40) {
            *(unsigned char *)(s0 + 0x1B) = *(unsigned char *)(s0 + func_0020BEF0((unsigned char *)s0) + 0x50);
            if (*(unsigned char *)(s0 + 0x1B) != 0x6D) {
                *(unsigned char *)(s0 + 3) = 4;
                func_0020CD40();
                D_00282244 = 0;
                D_002821B8 = *(unsigned char *)(s0 + 0x1B);
                *(short *)(s0 + 0x1C) = 0;
                break;
            }
            func_0020CD80();
            *(unsigned char *)(s0 + 3) = 3;
            *(unsigned char *)(s0 + 4) = 8;
        }
        break;
    case 2: {
        int v1;
        func_0020A7A0(((unsigned long long)0x200450A5 << 0x20) | ((unsigned long long)0x9D42 << 0x10) | 0x2200);
        func_002131B0((unsigned char *)s0, 0);
        func_002134C0((unsigned char *)s0, 0);
        if (func_00213CC0(s0) != 0) {
            v1 = *(unsigned char *)(s0 + 3) - 1;
            *(unsigned char *)(s0 + 3) = v1;
        }
        break;
    }
    case 3: {
        unsigned char v1;
        func_0020A7A0(((unsigned long long)0x200450A5 << 0x20) | ((unsigned long long)0x9D42 << 0x10) | 0x2200);
        func_002131B0((unsigned char *)s0, 0);
        func_002134C0((unsigned char *)s0, 0);
        func_00213A00(s0, 0x400);
        v1 = *(unsigned char *)(s0 + 4) - 1;
        *(unsigned char *)(s0 + 4) = v1;
        if (v1 == 0) {
            *(unsigned char *)(s0 + 3) = 1;
            D_008106B0 = 0;
        }
        break;
    }
    case 4: {
        int *tbl;
        int count;
        short cur;
        func_0020A7A0(((unsigned long long)0x200450A5 << 0x20) | ((unsigned long long)0x9D42 << 0x10) | 0x2200);
        func_002131B0((unsigned char *)s0, 1);
        func_002134C0((unsigned char *)s0, 1);
        D_002821B4 = 1;
        tbl = &D_002659C0[*(unsigned char *)(s0 + 0x1B)];
        count = *tbl;
        if (count >= 2) {
            if (D_00810E78 & 0x2000) {
                cur = *(short *)(s0 + 0x1C);
                if (cur >= count - 1) {
                    *(short *)(s0 + 0x1C) = (short)(*tbl - 1);
                } else {
                    func_0020CDA0();
                    *(short *)(s0 + 0x1C) = *(short *)(s0 + 0x1C) + 1;
                }
            } else if (D_00810E78 & 0x8000) {
                cur = *(short *)(s0 + 0x1C);
                if (cur != 0) {
                    *(short *)(s0 + 0x1C) = cur - 1;
                    func_0020CDA0();
                }
            }
        }
        D_00282244 = *(short *)(s0 + 0x1C);
        if (D_00810E74 & 0x60) {
            D_002821B4 = 0;
            func_0020CD40();
            *(unsigned char *)(s0 + 3) = 1;
        }
        break;
    }
    }
}
