// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: per-frame state machine for the object passed in `p`.
//   p[4]  = state, dispatched through jtbl_002736B0 (13 entries, 0..12; out of range = return)
//   p[5]  = sub-step counter inside a state
//   p[6]  = "next state" / countdown byte
//   p[0x10] = result code written as 0x63
//   p[0x11] = selection index, dispatched through jtbl_00273690 (7 entries)
//   p[0x15] = latched copy of p[0x11], dispatched through jtbl_00273670 (7 entries)
//   p[0x16] = id passed to func_001FF080
//   short p[0x1C] = target count (D_00810C63 * 30), short p[0x1E] = current count,
//   short p[0x3C] = frame countdown.
// D_00810E74 is a bitmask polled with 0x10/0x20/0x30/0x40/0x2000/0x8000.
// D_002821B0/B4/B8 and D_00282240 are ordered state words -> declared volatile so
// mwcc keeps the stores in source order (this is what the target does).

extern int func_001C5FB0();
extern void func_00123168();
extern void func_001CBA50();
extern void func_001FB9F0();
extern void func_001FCF10();
extern void func_001FF080();
extern void func_00207D00();
extern void func_0020BF20();
extern void func_0020CCB0();
extern void func_0020CD40();
extern void func_0020CD60();
extern void func_0020CDA0();
extern void func_0020E020();
extern void func_002121A0();
extern void func_002125B0();
extern void func_00212B60();
extern void func_00217090();
extern void func_002177B0();
extern void func_00217FA0();
extern void func_00218640();
extern void func_00218D90();

extern char D_00265510[];
extern char D_00265980[];
extern unsigned char D_00275BD8;
extern volatile int D_002821B0[];
extern volatile int D_002821B4[];
extern volatile int D_002821B8[];
extern volatile int D_00282240[];
extern char D_002862C0[];
extern unsigned char D_008106B0[];
extern unsigned char D_008106B1[];
extern unsigned char D_008106C5[];
extern unsigned char D_00810C62[];
extern unsigned char D_00810C63[];
extern short D_00810CB4[];
extern unsigned short D_00810E74[];

void func_00211970(unsigned char *p)
{
    switch (p[4]) {
    case 0:
        D_002821B0[0] = 4;
        D_002821B4[0] = 0;
        D_00282240[0] = 2;
        p[5] = 0;
        func_0020E020();
        if (D_008106B0[0] == 5) {
            *(short *)(p + 0x1C) = D_00810C63[0] * 30;
            *(short *)(p + 0x1E) = D_00810CB4[0];
            if (*(short *)(p + 0x1C) == *(short *)(p + 0x1E)) {
                *(short *)(p + 0x3C) = 0xF0;
                p[4] = 12;
                D_002821B4[0] = 1;
                D_00282240[0] = 5;
                D_002821B8[0] = 5;
            } else {
                p[4] = 10;
                p[6] = 1;
            }
        } else if (D_008106B0[0] != 0) {
            D_008106B0[0] = 0;
            D_00282240[0] = 4;
            D_002821B8[0] = D_008106B1[0];
            p[6] = 0xF0;
            p[4] = 9;
        } else {
            p[4] = p[4] + 1;
        }
        break;
    case 1:
        func_002121A0(0);
        func_002125B0(p, 0);
        func_00212B60(p);
        func_0020BF20(D_00265980, 0, 0);
        switch (p[0x11]) {
        case 1:
            D_002821B4[0] = 1;
            D_002821B8[0] = 5;
            break;
        case 2:
            D_002821B4[0] = 1;
            D_002821B8[0] = 3;
            break;
        case 3:
            D_002821B4[0] = 1;
            D_002821B8[0] = 1;
            break;
        case 4:
            D_002821B4[0] = 1;
            D_002821B8[0] = 4;
            break;
        case 5:
            D_002821B4[0] = 1;
            D_002821B8[0] = 0;
            break;
        case 6:
            D_002821B4[0] = 1;
            D_002821B8[0] = 2;
            break;
        default:
            D_002821B4[0] = 0;
            break;
        }
        if (D_00810E74[0] & 0x20) {
            func_0020CD60();
            p[0x10] = 0x63;
        } else if (p[0x11] != 0 && (D_00810E74[0] & 0x40)) {
            func_0020CD40();
            D_002821B4[0] = 0;
            p[0x15] = p[0x11];
            p[4] = p[4] + 1;
        }
        break;
    case 2:
        switch (p[0x15]) {
        case 1:
            p[0x10] = 0x63;
            break;
        case 2:
            D_00275BD8 = 1;
            p[6] = 5;
            p[4] = p[4] + 1;
            p[0x16] = 0x2E;
            break;
        case 3:
            D_00275BD8 = 1;
            p[6] = 7;
            p[4] = p[4] + 1;
            p[0x16] = 0x30;
            break;
        case 4:
            D_00275BD8 = 1;
            p[6] = 8;
            p[4] = p[4] + 1;
            p[0x16] = 0x31;
            break;
        case 5:
            D_00275BD8 = 1;
            p[6] = 6;
            p[4] = p[4] + 1;
            p[0x16] = 0x2F;
            break;
        case 6:
            D_00275BD8 = 1;
            p[6] = 4;
            p[4] = p[4] + 1;
            p[0x16] = 0x2D;
            break;
        }
        break;
    case 3:
        if (p[5] == 0) {
            func_001FF080(0, p[0x16]);
            p[5] = p[5] + 1;
        } else if (D_00275BD8 == 0) {
            p[4] = p[6];
            p[5] = 0;
        }
        break;
    case 4:
        func_00218D90(p);
        break;
    case 5:
        func_00217090(p);
        break;
    case 6:
        func_00218640(p);
        break;
    case 7:
        func_002177B0(p);
        break;
    case 8:
        func_00217FA0(p);
        break;
    case 9:
        D_002821B4[0] = 1;
        if (D_008106B1[0] == 0x10) {
            func_002121A0(1);
        } else {
            func_002121A0(0);
        }
        func_002125B0(p, 1);
        func_00212B60(p);
        func_0020BF20(D_00265980, 2, D_008106B1[0]);
        if ((D_00810E74[0] & 0x60) == 0) {
            if (--p[6] != 0) {
                break;
            }
        }
        if (D_00810E74[0] & 0x60) {
            func_0020CD40();
        }
        D_002821B4[0] = 0;
        D_00282240[0] = 2;
        p[4] = 1;
        p[5] = 0;
        break;
    case 10:
        D_002821B4[0] = 1;
        D_00282240[0] = 5;
        func_002121A0(1);
        func_002125B0(p, 1);
        func_00212B60(p);
        func_0020BF20(D_00265980, 0, 0);
        D_002821B8[0] = 0x15;
        func_001FCF10();
        func_00207D00(1, 3);
        if (D_00810E74[0] & 0x8000) {
            if (p[6] != 0) {
                p[6] = p[6] - 1;
                func_0020CDA0();
            }
        } else if (D_00810E74[0] & 0x2000) {
            if (p[6] == 0) {
                p[6] = p[6] + 1;
                func_0020CDA0();
            }
        }
        func_0020CCB0(p);
        if (D_00810E74[0] & 0x40) {
            D_002821B4[0] = 0;
            if (p[6] != 0) {
                D_008106C5[0] = 0xFF;
                func_0020CD60();
            } else {
                func_0020CD40();
                p[4] = 0xB;
            }
        } else if (D_00810E74[0] & 0x30) {
            D_002821B4[0] = 0;
            func_0020CD60();
            D_008106C5[0] = 0xFF;
        }
        break;
    case 11:
        func_002121A0(3);
        func_002125B0(p, 1);
        func_00212B60(p);
        func_0020BF20(D_00265980, 0, 0);
        *(short *)(p + 0x1E) = *(short *)(p + 0x1E) + 3;
        if ((*(int *)0x70003B64 % 10) == 0) {
            func_001FB9F0(0x182, 0x1000, 0x1000, 0x1000);
        }
        if ((D_00810E74[0] & 0x70) || *(short *)(p + 0x1E) >= *(short *)(p + 0x1C)) {
            if (D_00810E74[0] & 0x70) {
                func_0020CD40();
            }
            *(short *)(p + 0x1E) = *(short *)(p + 0x1C);
            D_00810C62[0] = 30;
            D_00810CB4[0] = D_00810C63[0] * 30;
            p[4] = 12;
            *(short *)(p + 0x3C) = 0x78;
        }
        func_00123168(D_002862C0, func_001C5FB0(*(short *)(p + 0x1E), 4, 1));
        func_001CBA50(1, 0x8AC, 0x79F, 0x10, 0x10, D_002862C0, D_00265510);
        break;
    case 12:
        func_002121A0(1);
        func_002125B0(p, 1);
        func_00212B60(p);
        func_0020BF20(D_00265980, 2, D_008106B1[0]);
        if ((D_00810E74[0] & 0x70) == 0) {
            if (--*(short *)(p + 0x3C) != 0) {
                break;
            }
        }
        if (D_00810E74[0] & 0x70) {
            func_0020CD40();
        }
        D_002821B4[0] = 0;
        D_008106C5[0] = 0xFF;
        break;
    }
}
