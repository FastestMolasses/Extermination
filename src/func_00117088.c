// NEARMISS func_00117088  (vram 0x00117088, 0x22C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 55.16% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc coloring/reg-alloc wall (confirmed s84). Function fully decoded to logically-correct C (leaf, returns 0/1, manipulates D_00281AC0 packet struct from D_0027C6C0[t3->0x24*12+4]). GPR coloring diverges at the very first instruction (expected lhu v1,36/li a1,12 vs ours lhu v0,36/li a0,12) and cascades through the ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027C6C0[];
extern unsigned char D_00281AC0[];

int func_00117088(unsigned char *t3) {
    unsigned char *t4;
    unsigned char *a0p;
    int t1, t2;
    unsigned int a1b;
    unsigned char v1c;
    int base14;

    t2 = *(int *)(t3 + 0xC);
    t1 = *(int *)(t3 + 0x8);
    *(int *)(D_00281AC0 + 0x14) = t2;

    t4 = *(unsigned char **)(D_0027C6C0 + (*(unsigned short *)(t3 + 0x24)) * 0xC + 4);
    a0p = (unsigned char *)((*(unsigned short *)(t3 + 0x32) << 4) + (int)t4);

    *(unsigned char **)(D_00281AC0 + 0x10) = t4;
    *(int *)(D_00281AC0 + 0x18) = (int)t4 + *(int *)(t4 + 0x14);
    *(int *)(D_00281AC0 + 0x20) = (int)t4 + *(int *)(a0p + 0x10);

    a1b = *(unsigned char *)(t2 + t1);
    if (a1b & 0x80) {
        t3[1] = a1b;
        v1c = *(unsigned char *)(*(int *)(D_00281AC0 + 0x14) + t1);
    } else {
        v1c = t3[1];
        t1 = t1 - 1;
        *(int *)(t3 + 0x8) = t1;
    }
    t3[0] = v1c;

    base14 = *(int *)(D_00281AC0 + 0x14);
    t3[2] = *(unsigned char *)(t1 + base14 + 1);
    t3[3] = *(unsigned char *)(t1 + base14 + 2);
    t3[4] = *(unsigned char *)(t1 + base14 + 3);

    if (*(unsigned short *)(t3 + 0x32) == 1) {
        unsigned char *t0 = *(unsigned char **)(D_00281AC0 + 0x10);
        int base = *(int *)(D_00281AC0 + 0x14);
        unsigned char *a1p = (unsigned char *)(t1 + base);
        int a3 = (int)t4 + 8;
        int t1g = *(int *)(D_00281AC0 + 0x20);
        int t2g = *(int *)(t0 + 0x24);
        int a0v = (*(int *)(t3 + 0x18) << 4) + 0x10;
        int v0a;

        v0a = (int)(*(unsigned char *)(a1p + 3) << 1) + t1g;
        *(int *)(D_00281AC0 + 0x00) =
            ((int)t4 + t2g) + *(unsigned short *)(v0a + 0x312);

        v0a = (int)t4 + *(int *)(t0 + 0x20);
        *(int *)(D_00281AC0 + 0x08) = v0a;
        *(int *)(D_00281AC0 + 0x0C) = v0a + a0v;

        v0a = (int)(*(unsigned char *)(a1p + 3) << 1) + t1g;
        *(int *)(D_00281AC0 + 0x04) =
            a3 + (*(unsigned short *)(v0a + 0x312) + t2g);
        return 1;
    } else {
        unsigned int v0 = t3[0];
        int a1h = (v0 & 0xF) << 4;
        unsigned char *a3;
        int base, t1i, t0v, a1v, p;
        unsigned char *a2p, *v1p;

        if (v0 < 0xA0) {
            int b14 = *(int *)(D_00281AC0 + 0x14);
            int a3v = *(int *)(D_00281AC0 + 0x20);
            unsigned char *q = (unsigned char *)(a1h + b14);
            int pp = (*(unsigned char *)(q + 0x12) << 1) + a3v;

            if (*(unsigned short *)(pp + 2) == 0xFFFF) {
                *(int *)(t3 + 0x8) = t1 + 3;
                return 0;
            }
            if (*(unsigned short *)(a3v + 0) < *(unsigned char *)(q + 0x12)) {
                *(int *)(t3 + 0x8) = t1 + 3;
                return 0;
            }
            if (*(int *)(*(int *)(D_00281AC0 + 0x10) + 0x10) == -1) {
                *(int *)(t3 + 0x8) = t1 + 3;
                return 0;
            }
            a3 = D_00281AC0;
        } else {
            a3 = D_00281AC0;
        }

        base = *(int *)(a3 + 0x14);
        t1i = *(int *)(a3 + 0x20);
        a2p = (unsigned char *)(a1h + base);
        v1p = *(unsigned char **)(a3 + 0x10);
        a1v = *(int *)(v1p + 0x10);
        t0v = t2 + (a1h + 0x10);

        p = (*(unsigned char *)(a2p + 0x12) << 1) + t1i;
        *(int *)(D_00281AC0 + 0x00) =
            (a1v + (int)t4) + *(unsigned short *)(p + 2);
        *(int *)(a3 + 0x0C) = t0v;
        *(int *)(a3 + 0x08) = t2;

        p = (*(unsigned char *)(a2p + 0x12) << 1) + t1i;
        *(int *)(a3 + 0x04) =
            (a1v + 8) + (*(unsigned short *)(p + 2) + (int)t4);
        return 1;
    }
}
