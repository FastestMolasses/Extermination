// NEARMISS func_00201F70  (vram 0x00201F70, 0xC2C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.33% on mwcc 2.3.3 (991202 only 82.34%). Body/structure fully recovered; two independent residuals, 15 differing instructions of 781. (a) 3 sites (idx 19/20, 271/274, 518/521): the target leaves a conditional-branch delay slot as `nop` while BOTH mwcc builds speculate the following `lui $at, 0x...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Controller-config / calibration screen tick. Reads the pad edge/held masks
// (D_00810E70 = held, D_00810E74 = edge, D_00810E78 = repeat), moves a 2D
// cursor stored at 0x70003B94 (X) / 0x70003B96 (Y) clamped to +-0x14, draws the
// four corner quads + the arrow sprites + the two numeric readouts, and returns
// 0 = still editing, 1 = accept/cancel-with-save, 2 = cancel-restore.
extern int func_0020A7A0(long long);
extern void func_001FCE30(int, int, int, int, unsigned int);
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, unsigned int, long long);
extern int float_to_int(float);
extern void func_0020CDA0(void);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_00123168(void *, void *);
extern char *func_001C5FB0(int, int, int);
extern void func_00122EF0(void *, char *);
extern void func_001CBA50(int, int, int, int, int, void *, void *);
extern int D_00282240;
extern unsigned short D_00810E70;
extern unsigned short D_00810E74;
extern unsigned short D_00810E78;
extern char D_002862C0[];
extern char D_00273320[];
extern char D_00273328[];
extern char D_00265510[];

int func_00201F70(unsigned char *arg0) {
    int x;
    int ret;

    func_0020A7A0(((long long)0x20043525 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2180);
    switch (*(unsigned char *)(*(unsigned char **)0x70003B6C + 0xD)) {
    case 0: {
        unsigned char *g;
        *(short *)(arg0 + 8) = *(short *)0x70003B94;
        *(short *)(arg0 + 0xA) = *(short *)0x70003B96;
        g = *(unsigned char **)0x70003B6C;
        g[0xD] = g[0xD] + 1;
        break;
    }
    case 1:
        break;
    }
    if (D_00810E78 & 0x1000) {
        *(short *)0x70003B96 = *(short *)0x70003B96 + 1;
        if (*(short *)0x70003B96 > 0x14) {
            *(short *)0x70003B96 = 0x14;
        } else {
            func_0020CDA0();
        }
    } else if (D_00810E78 & 0x4000) {
        *(short *)0x70003B96 = *(short *)0x70003B96 - 1;
        if (*(short *)0x70003B96 < -0x14) {
            *(short *)0x70003B96 = -0x14;
        } else {
            func_0020CDA0();
        }
    }
    if (D_00810E78 & 0x2000) {
        *(short *)0x70003B94 = *(short *)0x70003B94 - 1;
        if (*(short *)0x70003B94 < -0x14) {
            *(short *)0x70003B94 = -0x14;
        } else {
            func_0020CDA0();
        }
    } else if (D_00810E78 & 0x8000) {
        *(short *)0x70003B94 = *(short *)0x70003B94 + 1;
        if (*(short *)0x70003B94 > 0x14) {
            *(short *)0x70003B94 = 0x14;
        } else {
            func_0020CDA0();
        }
    }
    func_001FCE30(0x98, 0x2A, D_00282240, 0x13, 0x80808080);
    func_00207D00(1, 0);
    func_00207E40(1, 0x7000, 0x7900, 0x100, 0x100, 0x66808080,
                  ((long long)0x200438E6 << 0x20) | 0x21421D40);
    func_00207E40(1, 0x7000, 0x8000, 0x100, 0x100, 0x66808080,
                  ((long long)0x20043A66 << 0x20) | 0x21421F00);
    func_00207E40(1, 0x8000, 0x7900, 0x100, 0x100, 0x66808080,
                  ((long long)0x20043A46 << 0x20) | 0x21421E40);
    func_00207E40(1, 0x8000, 0x8000, 0x100, 0x100, 0x66808080,
                  ((long long)0x20043AC6 << 0x20) | 0x21421F40);
    if (D_00810E70 & 0x2000) {
        x = float_to_int(16.0f * (float)(0x8CC - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x200438C5 << 0x20) | 0x554221C4);
        x = float_to_int(16.0f * (float)(0x714 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221B0);
    } else if (D_00810E70 & 0x8000) {
        x = float_to_int(16.0f * (float)(0x8CC - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221A4);
        x = float_to_int(16.0f * (float)(0x714 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x200438C5 << 0x20) | 0x554221D0);
    } else {
        x = float_to_int(16.0f * (float)(0x8CC - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221A4);
        x = float_to_int(16.0f * (float)(0x714 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221B0);
    }
    if (D_00810E70 & 0x1000) {
        x = float_to_int(16.0f * (float)(0x7F0 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x15 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x200438C5 << 0x20) | 0x554221D4);
        x = float_to_int(16.0f * (float)(0x7F0 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x18B - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221C0);
    } else if (D_00810E70 & 0x4000) {
        x = float_to_int(16.0f * (float)(0x7F0 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x15 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221B4);
        x = float_to_int(16.0f * (float)(0x7F0 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x18B - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x200438C5 << 0x20) | 0x554221E0);
    } else {
        x = float_to_int(16.0f * (float)(0x7F0 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x15 - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221B4);
        x = float_to_int(16.0f * (float)(0x7F0 - *(short *)0x70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x18B - *(short *)0x70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221C0);
    }
    func_00123168(D_002862C0, D_00273320);
    func_00122EF0(D_002862C0, func_001C5FB0(*(short *)0x70003B94, 2, 1));
    func_001CBA50(1, 0x7C8, 0x7E0, 0x10, 0x10, D_002862C0, D_00265510);
    func_00123168(D_002862C0, D_00273328);
    func_00122EF0(D_002862C0, func_001C5FB0(*(short *)0x70003B96, 2, 1));
    func_001CBA50(1, 0x7C8, 0x7EA, 0x10, 0x10, D_002862C0, D_00265510);
    func_00207E40(1, 0x7BD0, 0x8010, 0x80, 0x80, 0x80808080,
                  ((long long)0x20043845 << 0x20) | ((long long)0xDD42 << 0x10) | 0x2100);
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        *(short *)(arg0 + 8) = *(short *)0x70003B94;
        *(short *)(arg0 + 0xA) = *(short *)0x70003B96;
        return 1;
    } else if (D_00810E74 & 0x30) {
        func_0020CD60();
        ret = (D_00810E74 & 0x10) ? 2 : 1;
        *(short *)0x70003B94 = *(short *)(arg0 + 8);
        *(short *)0x70003B96 = *(short *)(arg0 + 0xA);
        return ret;
    }
    return 0;
}
