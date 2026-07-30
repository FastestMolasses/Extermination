// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int D_00275610;
extern unsigned char D_00810700[8];
extern unsigned char D_00810702[8];
extern unsigned char D_00810841[];
extern unsigned char D_008107D8[];
extern unsigned char D_0024E1A0[];

extern void func_001B0FD0(unsigned char *);
extern void func_001C6380(unsigned char *);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern void func_00158590(unsigned char *, int, int);
extern int func_001BC740(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *);
extern void func_001B17A0(unsigned char *);
extern void func_001AFC10(unsigned char *);

void func_001BDFC0(unsigned char *p) {
    unsigned char *e;
    unsigned char *sub;
    unsigned char *q;
    short t;
    int idx;

    e = *(unsigned char **)(p + 0x18);
    sub = p + 0x1F0;
    switch (p[4]) {
    case 0:
        func_001B0FD0(p);
        *(int *)(p + 0x30) = (int)&D_00275610;
        func_001C6380(p);
        if (*(unsigned short *)(p + 0x2E) == 0xFF) {
            if (p[3] == 1) {
                if ((D_00810700[0] == 7 && D_00810702[0] == 0) ||
                    (D_00810700[0] == 8 && D_00810702[0] == 0)) {
                    func_001BA1A0(sub, D_0024E1A0);
                    p[4] = 4;
                } else {
                    p[0] = 1;
                }
            } else {
                p[0] = 1;
            }
        } else if (D_00810700[0] == 4) {
            if (D_00810702[0] == 0xA) {
                if (p[3] == 1) {
                    func_001BA1A0(sub, D_0024E1A0);
                    p[4] = 4;
                } else {
                    p[0] = 1;
                }
            }
        } else {
            if (D_00810702[0] == 1) {
                if (p[3] == 1) {
                    func_001BA1A0(sub, D_0024E1A0);
                    p[4] = 4;
                } else {
                    p[0] = 1;
                }
            }
        }
        break;
    case 1:
        switch (p[5]) {
        case 0:
            if (*(unsigned short *)(p + 0x2E) == 0xFF) {
                if (p[3] == 0) {
                    if ((*(unsigned char **)(e + 0x18))[0xB] == 0) {
                        p[0] = 1;
                        func_00158590(p, 1, 1);
                        if (func_001BC740(p, sub)) {
                            p[5] = p[5] + 1;
                        }
                    } else {
                        func_00158590(p, 0, 1);
                        p[0] = 2;
                    }
                } else {
                    if ((*(unsigned char **)(*(unsigned char **)(e + 0x18) + 0x18))[0xB] == 3) {
                        p[0] = 1;
                        func_00158590(p, 1, 1);
                        if (func_001BC740(p, sub)) {
                            p[5] = p[5] + 1;
                        }
                    } else {
                        func_00158590(p, 0, 1);
                        p[0] = 2;
                    }
                }
            } else if (D_00810841[D_00810700[0]] & (1 << *(unsigned short *)(p + 0x2E))) {
                if (p[3] == 0) {
                    if ((*(unsigned char **)(e + 0x18))[0xB] == 0) {
                        p[0] = 1;
                        func_00158590(p, 1, 1);
                        if (func_001BC740(p, sub)) {
                            p[5] = p[5] + 1;
                        }
                    } else {
                        func_00158590(p, 0, 1);
                        p[0] = 2;
                    }
                } else {
                    if ((*(unsigned char **)(*(unsigned char **)(e + 0x18) + 0x18))[0xB] == 3) {
                        p[0] = 1;
                        func_00158590(p, 1, 1);
                        if (func_001BC740(p, sub)) {
                            p[5] = p[5] + 1;
                        }
                    } else {
                        func_00158590(p, 0, 1);
                        p[0] = 2;
                    }
                }
            } else {
                p[0] = 2;
                func_00158590(p, 0, 1);
            }
            break;
        case 1:
            func_00158590(p, 1, 1);
            if (func_001BA1F0(p)) {
                if (*(unsigned short *)(p + 0x2E) == 0xFF) {
                    if (p[3] == 0) {
                        p[5] = 2;
                    } else {
                        p[5] = 6;
                    }
                } else {
                    if (p[3] == 0) {
                        p[5] = 2;
                    } else {
                        p[5] = 6;
                    }
                }
            }
            break;
        case 2:
            if (D_00810700[0] == 4 || D_00810700[0] == 7) {
                idx = 0x5E;
            } else {
                idx = 0x5F;
            }
            if (*(unsigned short *)(p + 0x2E) == 0xFF) {
                if (D_008107D8[idx] != 0) {
                    p[5] = 4;
                } else {
                    *(short *)(p + 0x28) = 300;
                    p[5] = 3;
                }
            } else {
                if (D_008107D8[idx] == 0) {
                    p[5] = 4;
                } else {
                    *(short *)(p + 0x28) = 300;
                    p[5] = 3;
                }
            }
            func_00158590(p, 0, 1);
            break;
        case 3:
            t = *(short *)(p + 0x28) - 1;
            *(short *)(p + 0x28) = t;
            if (t == 0) {
                p[5] = p[5] + 1;
            }
            func_00158590(p, 0, 1);
            break;
        case 4:
            q = *(unsigned char **)(e + 0x18);
            if (q[0xB] == 0) {
                q[0xB] = 2;
                p[5] = p[5] + 1;
            }
            /* fallthrough */
        case 5:
            func_00158590(p, 0, 1);
            break;
        case 6:
            q = *(unsigned char **)(*(unsigned char **)(e + 0x18) + 0x18);
            if (q[0xB] == 3) {
                q[0xB] = 4;
            }
            p[5] = 7;
            /* fallthrough */
        case 7:
            func_00158590(p, 0, 1);
            break;
        }
        func_001B17A0(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
        break;
    case 2:
    case 3:
        func_001AFC10(p);
        break;
    case 4:
        if (func_001BA1F0(p)) {
            p[4] = 1;
            p[0] = 1;
        }
        func_00158590(p, 0, 1);
        func_001B17A0(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
        break;
    }
}
