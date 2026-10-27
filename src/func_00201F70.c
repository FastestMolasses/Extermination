// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B94 0x70003B96

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.33% NEARMISS).
// Fixes read from the original instructions:
//  - The scratchpad cursor halfwords 0x70003B94/0x70003B96 are relocated
//    externs (idiom-32, opted in with // SPAD:), and the state pointer at
//    0x70003B6C uses the global D_70003B6C. As relocated symbols mwcc no longer
//    speculates the scratchpad upper half into the three branch delay slots
//    that the original leaves empty.
//  - Cancel path: the cursor is restored from arg0+8/+0xA BEFORE the result
//    (2 when the 0x10 edge bit is set, else 1) is computed and returned; the
//    original reloads the edge mask first and selects 2/1 after the stores.
//
// Controller-config / calibration screen tick. Reads the pad edge/held masks
// (D_00810E70 = held, D_00810E74 = edge, D_00810E78 = repeat), moves a 2D
// cursor stored at D_70003B94 (X) / D_70003B96 (Y) clamped to +-0x14, draws the
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
extern unsigned char *D_70003B6C;
extern short D_70003B94;
extern short D_70003B96;
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

    func_0020A7A0(((long long)0x20043525 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2180);
    switch (*(unsigned char *)(D_70003B6C + 0xD)) {
    case 0: {
        unsigned char *g;
        *(short *)(arg0 + 8) = D_70003B94;
        *(short *)(arg0 + 0xA) = D_70003B96;
        g = D_70003B6C;
        g[0xD] = g[0xD] + 1;
        break;
    }
    case 1:
        break;
    }
    if (D_00810E78 & 0x1000) {
        D_70003B96 = D_70003B96 + 1;
        if (D_70003B96 > 0x14) {
            D_70003B96 = 0x14;
        } else {
            func_0020CDA0();
        }
    } else if (D_00810E78 & 0x4000) {
        D_70003B96 = D_70003B96 - 1;
        if (D_70003B96 < -0x14) {
            D_70003B96 = -0x14;
        } else {
            func_0020CDA0();
        }
    }
    if (D_00810E78 & 0x2000) {
        D_70003B94 = D_70003B94 - 1;
        if (D_70003B94 < -0x14) {
            D_70003B94 = -0x14;
        } else {
            func_0020CDA0();
        }
    } else if (D_00810E78 & 0x8000) {
        D_70003B94 = D_70003B94 + 1;
        if (D_70003B94 > 0x14) {
            D_70003B94 = 0x14;
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
        x = float_to_int(16.0f * (float)(0x8CC - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x200438C5 << 0x20) | 0x554221C4);
        x = float_to_int(16.0f * (float)(0x714 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221B0);
    } else if (D_00810E70 & 0x8000) {
        x = float_to_int(16.0f * (float)(0x8CC - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221A4);
        x = float_to_int(16.0f * (float)(0x714 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x200438C5 << 0x20) | 0x554221D0);
    } else {
        x = float_to_int(16.0f * (float)(0x8CC - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221A4);
        x = float_to_int(16.0f * (float)(0x714 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0xD0 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221B0);
    }
    if (D_00810E70 & 0x1000) {
        x = float_to_int(16.0f * (float)(0x7F0 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x15 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x200438C5 << 0x20) | 0x554221D4);
        x = float_to_int(16.0f * (float)(0x7F0 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x18B - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221C0);
    } else if (D_00810E70 & 0x4000) {
        x = float_to_int(16.0f * (float)(0x7F0 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x15 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221B4);
        x = float_to_int(16.0f * (float)(0x7F0 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x18B - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x200438C5 << 0x20) | 0x554221E0);
    } else {
        x = float_to_int(16.0f * (float)(0x7F0 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x15 - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221B4);
        x = float_to_int(16.0f * (float)(0x7F0 - D_70003B94));
        func_00207E40(1, x, float_to_int(16.0f * (float)(((0x18B - D_70003B96) >> 1) + 0x790)),
                      0x20, 0x20, 0x80808080, ((long long)0x20043865 << 0x20) | 0x554221C0);
    }
    func_00123168(D_002862C0, D_00273320);
    func_00122EF0(D_002862C0, func_001C5FB0(D_70003B94, 2, 1));
    func_001CBA50(1, 0x7C8, 0x7E0, 0x10, 0x10, D_002862C0, D_00265510);
    func_00123168(D_002862C0, D_00273328);
    func_00122EF0(D_002862C0, func_001C5FB0(D_70003B96, 2, 1));
    func_001CBA50(1, 0x7C8, 0x7EA, 0x10, 0x10, D_002862C0, D_00265510);
    func_00207E40(1, 0x7BD0, 0x8010, 0x80, 0x80, 0x80808080,
                  ((long long)0x20043845 << 0x20) | ((long long)0xDD42 << 0x10) | 0x2100);
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        *(short *)(arg0 + 8) = D_70003B94;
        *(short *)(arg0 + 0xA) = D_70003B96;
        return 1;
    } else if (D_00810E74 & 0x30) {
        func_0020CD60();
        D_70003B94 = *(short *)(arg0 + 8);
        D_70003B96 = *(short *)(arg0 + 0xA);
        return (D_00810E74 & 0x10) ? 2 : 1;
    }
    return 0;
}
