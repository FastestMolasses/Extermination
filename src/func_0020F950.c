// NEARMISS func_0020F950  (vram 0x0020F950, 0x6D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 3-state actor-tick dispatcher (mode 0 init / mode 1 setup / mode 2 render-config), gated by arg0->byte3. Body/structure/logic fully recovered including the exact GS/GIF constant packing for func_00207E40 (7-arg incl. 64-bit packed tag 0x200429859d322000) and func_00207D90 (5-arg incl. t0), the D_...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f);
extern char D_70003400[];
extern char D_70003600[];
extern char *func_001AFF10(void);
extern void func_001B0000(void);
extern void func_00207D00(int a, int b);
extern void func_00207D90(int slot, int b0, int b2, int b1, int b3);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CD80(void);
extern void func_0020CDA0(void);
extern void func_00210A00(int a);
extern void func_00210C00(int a);
extern void func_00210F30(char *a, int b);
extern void func_00211400(char *a, int b);
extern void func_002101C0(void);
extern float D_002657C0[];
extern int D_00265890[];
extern int D_002821B0;
extern int D_002821B4;
extern int D_002821B8;
extern int D_00282240;
extern float D_00810350;
extern float D_00810358;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_008106CD;
extern unsigned char D_00810700[];
extern unsigned char D_00810CB8[];
extern unsigned short D_00810E74;
extern unsigned short D_00810E78;

void func_0020F950(char *arg0) {
    unsigned char mode;

    mode = *(unsigned char *)(arg0 + 3);
    if (mode != 2) {
        if (mode != 1) {
            int i;
            int total;
            unsigned char *e;

            i = 0;
            if (mode != 0) {
                return;
            }
            do {
                char *p = func_001AFF10();
                *(void **)(p + 0x10) = &func_002101C0;
                p[3] = (char)i;
                i += 1;
                p[0xD] = 0;
            } while (i < 0xB);
            i = 0;
            do {
                char *p = func_001AFF10();
                *(void **)(p + 0x10) = &func_002101C0;
                p[3] = (char)i;
                i += 1;
                p[0xD] = 1;
            } while (i < 0xB);
            *(unsigned char *)(arg0 + 3) = *(unsigned char *)(arg0 + 3) + 1;
            D_002821B0 = 4;
            D_002821B4 = 0;
            D_00282240 = 6;
            e = D_00810700;
            total = 0;
            i = 0;
            do {
                i += 1;
                total += e[0x5B8];
                e += 1;
            } while (i < 0xB);
            if (total != 0) {
                unsigned char idx;
                int cnt;
                *(unsigned char *)(arg0 + 0x12) = D_008106CD & 0xF;
                idx = *(unsigned char *)(arg0 + 0x12);
                if (D_00810CB8[idx] == 0) {
                    cnt = 0;
                    do {
                        idx = *(unsigned char *)(arg0 + 0x12);
                        if (idx == 0) idx = 0xA;
                        else idx = idx - 1;
                        *(unsigned char *)(arg0 + 0x12) = idx;
                        if (D_00810700[idx + 0x5B8] == 0) {
                            cnt += 1;
                            if (cnt >= 0xB) {
                            } else {
                                continue;
                            }
                        }
                        break;
                    } while (1);
                }
            } else {
                *(unsigned char *)(arg0 + 0x12) = 0xFF;
            }
            *(unsigned char *)(arg0 + 0x14) = (D_008106CD & 0x30) >> 4;
            *(float *)(arg0 + 0x2C) = 0.0f;
            *(float *)(arg0 + 0x28) = 0.0f;
            if (D_008106B0 != 0) {
                D_008106B0 = 0;
                *(unsigned char *)(arg0 + 0x12) = D_008106B1;
                *(unsigned char *)(arg0 + 0x13) = 1;
                *(float *)(arg0 + 0x24) = *(float *)((char *)D_002657C0 + (*(unsigned char *)(arg0 + 0x12) * 0x14));
                *(unsigned char *)(arg0 + 3) = 2;
                return;
            }
            *(unsigned char *)(arg0 + 0x13) = 0;
            *(float *)(arg0 + 0x24) = 0.10666667f;
            return;
        }
        func_00210A00(0);
        func_00210C00(0);
        func_00207D00(1, 3);
        func_001B0000();
        func_00211400(arg0, 0);
        func_00207E40(1, 0x7080, 0x7900, 0x80, 0x40, 0x80808080, 0x200429859d322000ULL);
        if (D_00810E74 & 0x20) {
            func_0020CD60();
            *(char *)(arg0 + 0x10) = 0x63;
            D_002821B4 = 0;
            return;
        }
        if (*(unsigned char *)(arg0 + 0x12) != 0xFF) {
            if (*(unsigned char *)(arg0 + 0x13) == 0) {
                if (D_00810E78 & 0x2000) {
                    int cnt = 0;
                    unsigned char idx;
                    do {
                        idx = *(unsigned char *)(arg0 + 0x12) + 1;
                        if ((int)idx >= 0xB) idx = 0;
                        *(unsigned char *)(arg0 + 0x12) = idx;
                        if (D_00810700[idx + 0x5B8] != 0) {
                            func_0020CDA0();
                        } else {
                            cnt += 1;
                            if (cnt >= 0xB) {
                            } else {
                                continue;
                            }
                        }
                        break;
                    } while (1);
                } else if (D_00810E78 & 0x8000) {
                    int cnt = 0;
                    unsigned char idx;
                    do {
                        idx = *(unsigned char *)(arg0 + 0x12);
                        if (idx == 0) idx = 0xA;
                        else idx = idx - 1;
                        *(unsigned char *)(arg0 + 0x12) = idx;
                        if (D_00810700[idx + 0x5B8] != 0) {
                            func_0020CDA0();
                        } else {
                            cnt += 1;
                            if (cnt >= 0xB) {
                            } else {
                                continue;
                            }
                        }
                        break;
                    } while (1);
                }
            }
            if (D_00810E74 & 0x40) {
                if (D_00810CB8[*(unsigned char *)(arg0 + 0x12)] != 0) {
                    int off;
                    unsigned char idx;
                    func_0020CD40();
                    *(unsigned char *)(arg0 + 0x13) = 1;
                    idx = *(unsigned char *)(arg0 + 0x12);
                    off = idx * 0x14;
                    *(float *)(arg0 + 0x24) = *(float *)((char *)D_002657C0 + off);
                    if (D_00810700[0] == 0x12 || D_00810700[0] == 0x15) {
                        *(float *)(arg0 + 0x28) = 0.0f;
                        *(float *)(arg0 + 0x2C) = 0.0f;
                    } else {
                        if ((D_008106CD & 0xF) == idx) {
                            float *base = (float *)D_00265890[idx];
                            float *row = base + (*(unsigned char *)(arg0 + 0x14) * 3);
                            *(float *)0x70003600 = D_00810350 - row[0];
                            *(int *)0x70003604 = 0;
                            *(float *)0x70003608 = D_00810358 - row[1];
                            *(int *)0x7000360C = 0x3F800000;
                            func_001029C0(D_70003400);
                            func_00102BB0(D_70003400, D_70003400, -base[*(unsigned char *)(arg0 + 0x14) * 3 + 2]);
                            func_001026A0(D_70003600, D_70003400, D_70003600);
                            *(float *)(arg0 + 0x2C) = 2.0f * (0.10666667f * *(float *)0x70003600);
                            *(float *)(arg0 + 0x28) = 2.0f * (0.10666667f * -*(float *)0x70003608);
                        } else {
                            *(float *)(arg0 + 0x28) = 0.0f;
                            *(float *)(arg0 + 0x2C) = 0.0f;
                        }
                    }
                    *(unsigned char *)(arg0 + 3) = 2;
                } else {
                    func_0020CD80();
                }
            }
            D_002821B4 = 1;
            D_002821B8 = *(unsigned char *)(arg0 + 0x12);
        }
        return;
    }
    func_00210A00(1);
    func_00207D00(1, 3);
    func_00207D90(1, 0x12, 9, 0x1EF, 0x96);
    func_001B0000();
    if ((D_008106CD & 0xF) == *(unsigned char *)(arg0 + 0x12)) {
        func_00210F30(arg0, 1);
    }
    func_00211400(arg0, 1);
    func_00207D90(1, 0, 0, 0x200, 0xE0);
    func_00207E40(1, 0x7080, 0x7900, 0x80, 0x40, 0x80808080, 0x200429859d322000ULL);
    func_00210C00(1);
    if (D_00810E74 & 0x20) {
        func_0020CD60();
        *(unsigned char *)(arg0 + 0x13) = 0;
        *(float *)(arg0 + 0x2C) = 0.0f;
        *(float *)(arg0 + 0x28) = 0.0f;
        *(unsigned char *)(arg0 + 3) = 1;
    }
    D_002821B4 = 1;
    D_002821B8 = *(unsigned char *)(arg0 + 0x12);
}
