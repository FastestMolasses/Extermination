// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: per-frame state machine for the controller / memory-card prompt task; t[4] is
// the 8-way state selector (jtbl_00273650), t[0x15] the latched menu selection (jtbl_00273630),
// states 4..7 forward the task to one of four sub-machines.
extern unsigned char D_00275BD8;
extern volatile int D_002821B0[16];
extern volatile int D_002821B4[16];
extern volatile int D_002821B8[16];
extern volatile int D_00282240[16];
extern unsigned short D_00810E74[8];

extern void func_0020E020(void);
extern void func_0020F170(void);
extern void func_0020F2A0(unsigned char *);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_001FF080(int, int);
extern void func_00214570(unsigned char *);
extern void func_002149F0(unsigned char *);
extern void func_00215870(unsigned char *);
extern void func_002160B0(unsigned char *);

void func_0020EE50(unsigned char *t)
{
    unsigned char sel;

    switch (t[4]) {
    case 0:
        D_002821B0[0] = 4;
        D_002821B4[0] = 0;
        D_00282240[0] = 1;
        t[4] = t[4] + 1;
        t[5] = 0;
        func_0020E020();
        break;
    case 1:
        func_0020F170();
        func_0020F2A0(t);
        sel = t[0x11];
        switch (sel) {
        case 1:
            D_002821B4[0] = 1;
            D_002821B8[0] = 1;
            break;
        case 2:
            D_002821B4[0] = 1;
            D_002821B8[0] = 3;
            break;
        case 3:
            D_002821B4[0] = 1;
            D_002821B8[0] = 0;
            break;
        case 4:
            D_002821B4[0] = 1;
            D_002821B8[0] = 2;
            break;
        case 5:
            D_002821B4[0] = 1;
            D_002821B8[0] = 4;
            break;
        default:
            D_002821B4[0] = 0;
            break;
        }
        if (D_00810E74[0] & 0x20) {
            func_0020CD60();
            t[0x10] = 0x63;
            break;
        }
        if (t[0x11] != 0 && (D_00810E74[0] & 0x40)) {
            func_0020CD40();
            D_002821B4[0] = 0;
            t[0x15] = t[0x11];
            t[4] = t[4] + 1;
        }
        break;
    case 2:
        switch (t[0x15]) {
        case 0:
            break;
        case 1:
            D_00275BD8 = 1;
            t[6] = 4;
            t[4] = t[4] + 1;
            t[0x16] = 0x20;
            break;
        case 2:
            t[0x10] = 0x63;
            break;
        case 3:
            D_00275BD8 = 1;
            t[6] = 5;
            t[4] = t[4] + 1;
            t[0x16] = 0x21;
            break;
        case 4:
            D_00275BD8 = 1;
            t[6] = 6;
            t[4] = t[4] + 1;
            t[0x16] = 0x22;
            break;
        case 5:
            D_00275BD8 = 1;
            t[6] = 7;
            t[4] = t[4] + 1;
            t[0x16] = 0x23;
            break;
        }
        break;
    case 3:
        if (t[5] == 0) {
            func_001FF080(0, t[0x16]);
            t[5] = t[5] + 1;
        } else if (D_00275BD8 == 0) {
            t[4] = t[6];
            t[5] = 0;
        }
        break;
    case 4:
        func_00214570(t);
        break;
    case 5:
        func_002149F0(t);
        break;
    case 6:
        func_00215870(t);
        break;
    case 7:
        func_002160B0(t);
        break;
    }
}
