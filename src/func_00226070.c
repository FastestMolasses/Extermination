// NEARMISS func_00226070  (vram 0x00226070, 0x730 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Larger weapon-swap FSM (state on arg0+1, dispatched via 5-way beq chain matching switch-descending idiom; nested inner sub-switches on arg0+2 targeting fields 0x28/0x2C via func_001FECB0(0/1,0,arg0+0x58,0,0), sibling family of func_002267A0/func_00217FA0). Body/logic fully recovered and verified ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int float_to_int(float);
extern void func_001FCBD0(int, int, int, int, int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, char *, char *);
extern int func_001FECB0(int, int, char *, int, int);
extern void func_00207F80(int, int, int, int, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void);
extern void func_00226010(char *);
extern int func_002267A0(char *);

extern int D_00275C58;
extern unsigned short D_00810E74;

int func_00226070(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 1);

    switch (st) {
    case 0: {
        int v;
        func_001FCBD0(0x3C, 0x9B, 7, 9, 0x70808080);
        if (D_00275C58 != 0) {
            func_00226010(arg0);
            return 0;
        }
        if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
            return 0;
        }
        if (*(unsigned char *)(arg0 + 2) == 0) {
            v = func_001FECB0(0, 0, arg0 + 0x58, 0, 0);
            if (v == 0) {
                *(unsigned char *)(arg0 + 2) = 1;
                *(int *)(arg0 + 0x28) = (*(int *)(arg0 + 0x58) == 2) ? 1 : 0;
                return 0;
            }
            if (v == -1) {
                return 0;
            }
            if (v == -2) {
                func_001FE8D0();
                return 0;
            }
            if (v == -3) {
                func_001FE8D0();
                return 0;
            }
            *(unsigned char *)(arg0 + 2) = 1;
            *(int *)(arg0 + 0x28) = 0;
            return 0;
        }
        if (*(unsigned char *)(arg0 + 2) == 1) {
            v = func_001FECB0(1, 0, arg0 + 0x58, 0, 0);
            if (v == 0) {
                *(unsigned char *)(arg0 + 1) = 2;
                *(unsigned char *)(arg0 + 2) = 0;
                *(int *)(arg0 + 0x2C) = (*(int *)(arg0 + 0x58) == 2) ? 1 : 0;
                return 0;
            }
            if (v == -1) {
                return 0;
            }
            if (v == -2) {
                func_001FE8D0();
                return 0;
            }
            if (v == -3) {
                func_001FE8D0();
                return 0;
            }
            *(unsigned char *)(arg0 + 1) = 2;
            *(int *)(arg0 + 0x2C) = 0;
            return 0;
        }
        return 0;
    }

    case 1: {
        int v = func_002267A0(arg0);
        if (v == 0) {
            return 0;
        }
        if (v == 1) {
            *(unsigned char *)(arg0 + 1) = 2;
            return 0;
        }
        if (v == 2) {
            return 2;
        }
        return v;
    }

    case 2: {
        int flag28 = *(int *)(arg0 + 0x28);
        int flag2c;

        if (flag28 == 0) {
            flag2c = *(int *)(arg0 + 0x2C);
            if (flag2c == 0) {
                func_001FCBD0(0x3C, 0x9B, 7, 0xB, 0x70808080);
            } else {
                goto shared_10a;
            }
        } else {
shared_10a:
            func_001FCBD0(0x3C, 0x9B, 7, 0xA, 0x70808080);
            if (*(int *)(arg0 + 0x28) != 0) {
                func_001FCBD0(0x64, 0xAF, 7, 3, 0x70808080);
                if (*(int *)(arg0 + 0x48) == 0) {
                    int c0 = float_to_int(29952.0f);
                    int c1 = float_to_int(33808.0f);
                    int c2 = float_to_int(30144.0f);
                    int c3 = float_to_int(33904.0f);
                    func_00207F80(1, c0, c1, c2, c3, 0x80CE6000);
                }
            }
            if (*(int *)(arg0 + 0x2C) != 0) {
                func_001FCBD0(0x64, 0xB9, 7, 4, 0x70808080);
                if (*(int *)(arg0 + 0x48) == 1) {
                    int c0 = float_to_int(29952.0f);
                    int c1 = float_to_int(33968.0f);
                    int c2 = float_to_int(30144.0f);
                    int c3 = float_to_int(34064.0f);
                    func_00207F80(1, c0, c1, c2, c3, 0x80CE6000);
                }
            }
        }

        flag28 = *(int *)(arg0 + 0x28);
        flag2c = *(int *)(arg0 + 0x2C);
        if (flag28 != 0 && flag2c != 0) {
            if ((D_00810E74 & 0x1000) && *(int *)(arg0 + 0x48) > 0) {
                func_0020CDA0();
                *(int *)(arg0 + 0x48) = *(int *)(arg0 + 0x48) - 1;
            }
            if ((D_00810E74 & 0x4000) && *(int *)(arg0 + 0x48) <= 0) {
                func_0020CDA0();
                *(int *)(arg0 + 0x48) = *(int *)(arg0 + 0x48) + 1;
            }
        } else if (flag28 != 0) {
            *(int *)(arg0 + 0x48) = 0;
        } else if (flag2c != 0) {
            *(int *)(arg0 + 0x48) = 1;
        }

        if (D_00810E74 & 0x40) {
            *(unsigned char *)(arg0 + 0x17) = 0x40;
        } else if (D_00810E74 & 0x20) {
            *(unsigned char *)(arg0 + 0x17) = 0x20;
        } else if (D_00810E74 & 0x10) {
            *(unsigned char *)(arg0 + 0x17) = 0x10;
        }

        if (D_00275C58 != 0) {
            func_00226010(arg0);
            return 0;
        }
        if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
            return 0;
        }

        {
            unsigned char flags17 = *(unsigned char *)(arg0 + 0x17);
            int v;

            if (flags17 & 0x20) {
                func_0020CD60();
                func_001FE8D0();
                *(unsigned char *)(arg0 + 0x17) = 0;
                return 2;
            }
            if (flags17 & 0x10) {
                func_0020CD60();
                func_001FE8D0();
                *(unsigned char *)(arg0 + 0x17) = 0;
                return 2;
            }
            if (flags17 & 0x40) {
                func_0020CD40();
                *(unsigned char *)(arg0 + 0x17) = 0;
                if (*(int *)(arg0 + 0x28) != 0 || *(int *)(arg0 + 0x2C) != 0) {
                    func_001FE8D0();
                    return 1;
                }
            }

            if (*(int *)(arg0 + 0x24) & 1) {
                v = func_001FECB0(1, 0, arg0 + 0x58, 0, 0);
                if (v != -1) {
                    if (v == 0 || v == -3 || v == -2) {
                        if (v == 0) {
                            *(int *)(arg0 + 0x2C) = (*(int *)(arg0 + 0x58) == 2) ? 1 : 0;
                        } else {
                            func_001FE8D0();
                        }
                    } else {
                        *(int *)(arg0 + 0x2C) = 0;
                    }
                }
            } else {
                v = func_001FECB0(0, 0, arg0 + 0x58, 0, 0);
                if (v != -1) {
                    if (v == 0 || v == -3 || v == -2) {
                        if (v == 0) {
                            *(int *)(arg0 + 0x28) = (*(int *)(arg0 + 0x58) == 2) ? 1 : 0;
                        } else {
                            func_001FE8D0();
                        }
                    } else {
                        *(int *)(arg0 + 0x28) = 0;
                    }
                }
            }

            if (v != -1) {
                *(int *)(arg0 + 0x24) = *(int *)(arg0 + 0x24) + 1;
            }
            return 0;
        }
    }

    case 3:
        return 0;
    case 4:
        return 0;
    default:
        return 0;
    }
}
