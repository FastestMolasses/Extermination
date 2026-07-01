// NEARMISS func_002267A0  (vram 0x002267A0, 0x3EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 62.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/logic fully recovered (verified instruction-by-instruction against the raw .s: the outer field+0x48 dispatch, the field+7 sub-state dispatch with two shapes -- unified v==0/-3/-2 vs separate v==0 arm -- targeting fields +0x28/+0x2C, and the func_001FECB0 arg0 0/1 selection all match). Residu...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_001FCBD0(int, int, int, int, int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, char *, char *);
extern int func_001FECB0(int, int, char *, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_00226010(char *);
extern int D_00275C58;
extern unsigned short D_00810E74[8];

int func_002267A0(char *arg0) {
    int v;

    if (*(int *)(arg0 + 0x48) != 0) {
        func_001FCBD0(0x3C, 0x9B, 7, 0xF, 0x70808080);
    } else {
        func_001FCBD0(0x3C, 0x9B, 7, 0xE, 0x70808080);
    }

    if (D_00810E74[0] & 0x40) {
        func_0020CD40();
        func_001FE8D0();
        return 1;
    }
    if (D_00810E74[0] & 0x20) {
        func_0020CD60();
        func_001FE8D0();
        return 2;
    }
    if (D_00810E74[0] & 0x10) {
        func_0020CD60();
        func_001FE8D0();
        return 2;
    }

    *(int *)(arg0 + 0x10) = *(int *)(arg0 + 0x10) + 1;
    if (D_00275C58 != 0) {
        func_00226010(arg0);
        return 0;
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }

    if (*(int *)(arg0 + 0x48) != 0) {
        if (*(unsigned char *)(arg0 + 7) == 0) {
            /* target 0x2C; v==0 unified with v==-3/-2 */
            v = func_001FECB0(1, 0, arg0 + 0x58, 0, 0);
            if (v == -1) {
                return 0;
            }
            switch (v) {
            case 0:
            case -3:
            case -2:
                if (*(int *)(arg0 + 0x58) == 2) {
                    func_001FE8D0();
                    *(int *)(arg0 + 0x2C) = 1;
                    return 1;
                }
                *(unsigned char *)(arg0 + 7) = 1;
                return 0;
            }
            *(int *)(arg0 + 0x2C) = 0;
            *(unsigned char *)(arg0 + 7) = 1;
            return 0;
        }
        if (*(unsigned char *)(arg0 + 7) == 1) {
            /* target 0x28; v==0 has its own arm (no func_001FE8D0/early return) */
            v = func_001FECB0(0, 0, arg0 + 0x58, 0, 0);
            if (v == -1) {
                return 0;
            }
            if (v == 0) {
                *(int *)(arg0 + 0x28) = (*(int *)(arg0 + 0x58) == 2) ? 1 : 0;
                *(unsigned char *)(arg0 + 7) = 0;
                return 0;
            }
            if (v == -3 || v == -2) {
                if (*(int *)(arg0 + 0x58) == 2) {
                    func_001FE8D0();
                    *(int *)(arg0 + 0x28) = 1;
                    return 1;
                }
                *(unsigned char *)(arg0 + 7) = 0;
                return 0;
            }
            *(int *)(arg0 + 0x28) = 0;
            *(unsigned char *)(arg0 + 7) = 0;
            return 0;
        }
        return 0;
    } else {
        if (*(unsigned char *)(arg0 + 7) == 1) {
            /* target 0x2C; v==0 has its own arm (no func_001FE8D0/early return) */
            v = func_001FECB0(1, 0, arg0 + 0x58, 0, 0);
            if (v == -1) {
                return 0;
            }
            if (v == 0) {
                *(int *)(arg0 + 0x2C) = (*(int *)(arg0 + 0x58) == 2) ? 1 : 0;
                *(unsigned char *)(arg0 + 7) = 0;
                return 0;
            }
            if (v == -3 || v == -2) {
                if (*(int *)(arg0 + 0x58) == 2) {
                    func_001FE8D0();
                    *(int *)(arg0 + 0x2C) = 1;
                    return 1;
                }
                *(unsigned char *)(arg0 + 7) = 0;
                return 0;
            }
            *(int *)(arg0 + 0x2C) = 0;
            *(unsigned char *)(arg0 + 7) = 0;
            return 0;
        }
        if (*(unsigned char *)(arg0 + 7) == 0) {
            /* target 0x28; v==0 unified with v==-3/-2 */
            v = func_001FECB0(0, 0, arg0 + 0x58, 0, 0);
            if (v == -1) {
                return 0;
            }
            switch (v) {
            case 0:
            case -3:
            case -2:
                if (*(int *)(arg0 + 0x58) == 2) {
                    func_001FE8D0();
                    *(int *)(arg0 + 0x28) = 1;
                    return 1;
                }
                *(unsigned char *)(arg0 + 7) = 1;
                return 0;
            }
            *(int *)(arg0 + 0x28) = 0;
            *(unsigned char *)(arg0 + 7) = 1;
            return 0;
        }
        return 0;
    }
}
