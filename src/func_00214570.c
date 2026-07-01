// NEARMISS func_00214570  (vram 0x00214570, 0x478 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation / branch-likely-choice permutation only. Logic, globals, and control flow are fully recovered and byte-identical in structure. Residual: (1) the default early-return path emits mwcc's own bnez-to-shared-tail instead of the target's beqzl-direct-to-case0 (idiom-20 branch-likely...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_0020A7A0(unsigned long long v);
extern void func_0020AE40(void *a0, void *a1, int flags);
extern int func_0020B210(void *a0, void *a1, unsigned long long a2, int a3);
extern int func_0020BC50(void *a0, void *a1, unsigned long long a2, int a3);
extern void func_0020B0D0(int a0, void *a1);
extern void func_0020BBE0(char *a0, int a1);
extern void func_0020CD60(void);
extern void func_0020CD80(void);

extern char D_00265B80[];
extern char D_00265BF0[];
extern int D_002821B0;
extern int D_002821B4;
extern int D_00282240;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_00810C60;
extern unsigned char D_00810C7B;
extern unsigned char D_00810C7C;
extern unsigned char D_00810C7D;
extern unsigned char D_00810C7E;
extern unsigned short D_00810E74;

void func_00214570(char *arg0) {
    int st;
    int idx;
    int i;

    st = *(unsigned char *)(arg0 + 5);

    if (st == 3) {
        goto case3;
    }
    if (st == 2) {
        goto case2;
    }
    if (st == 1) {
        goto state1;
    }
    if (st == 0) {
        goto case0;
    }
    return;

case0:
    *(unsigned char *)(arg0 + 0x17) = 0;
    *(unsigned char *)(arg0 + 0x19) = 0;
    *(unsigned char *)(arg0 + 0x18) = 0;
    *(unsigned char *)(arg0 + 0x1A) = 0;
    D_002821B0 = 4;
    D_002821B4 = 0;
    D_00282240 = 3;
    *(short *)(arg0 + 0x1E) = 0x17;

    if (D_00810C7C != 0) {
        idx = *(unsigned char *)(arg0 + 0x18);
        *(unsigned char *)(arg0 + 0x18) = idx + 1;
        arg0[0x50 + idx] = 1;
    } else if (D_00810C7B != 0) {
        idx = *(unsigned char *)(arg0 + 0x18);
        *(unsigned char *)(arg0 + 0x18) = idx + 1;
        arg0[0x50 + idx] = 0;
    }
    if (D_00810C7D != 0) {
        idx = *(unsigned char *)(arg0 + 0x18);
        *(unsigned char *)(arg0 + 0x18) = idx + 1;
        arg0[0x50 + idx] = 2;
    }
    if (D_00810C7E != 0) {
        idx = *(unsigned char *)(arg0 + 0x18);
        *(unsigned char *)(arg0 + 0x18) = idx + 1;
        arg0[0x50 + idx] = 3;
    }

    if (D_008106B0 != 0) {
        *(unsigned char *)(arg0 + 0x1B) = D_008106B1 - 0x17;
        for (i = 0; i < *(unsigned char *)(arg0 + 0x18); i++) {
            if (*(unsigned char *)(arg0 + i + 0x50) == *(unsigned char *)(arg0 + 0x1B)) {
                *(unsigned char *)(arg0 + 0x17) = i;
                D_00282240 = 4;
                if ((D_008106B1 == 0x19) && (D_00810C60 == 1)) {
                    D_00810C60 = 2;
                }
                break;
            }
        }
        D_008106B0 = 0;
        *(unsigned char *)(arg0 + 5) = 3;
        *(unsigned char *)(arg0 + 6) = 0xF0;
        return;
    }

    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
    *(unsigned char *)(arg0 + 6) = 0;

state1:
    if (D_00810E74 & 0x20) {
        func_0020CD60();
        D_002821B4 = 2;
        *(unsigned char *)(arg0 + 1) = 3;
        *(unsigned char *)(arg0 + 2) = 0;
        *(unsigned char *)(arg0 + 3) = 0;
        *(unsigned char *)(arg0 + 4) = 0;
        *(unsigned char *)(arg0 + 5) = 0;
        return;
    }

    if (*(unsigned char *)(arg0 + 6) == 0) {
        if (D_00810E74 & 0x40) {
            func_0020CD80();
            *(unsigned char *)(arg0 + 6) = 0xF;
        }
    } else {
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) - 1;
    }

    func_0020A7A0(((long long)0x20042585 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2100);
    func_0020AE40(arg0, D_00265B80, 1);
    if (func_0020B210(arg0, D_00265BF0, ((long long)0x20042605 << 0x20) | ((long long)0xA132 << 0x10) | 0x1F80, 1) != 0) {
        *(short *)(arg0 + 0x1C) = 0;
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        func_0020BBE0(arg0, *(unsigned char *)(arg0 + 0x1A));
    }
    func_0020B0D0(0, arg0);
    return;

case2:
    func_0020A7A0(((long long)0x20042585 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2100);
    func_0020AE40(arg0, D_00265B80, 1);
    if (func_0020BC50(arg0, D_00265BF0, ((long long)0x20042605 << 0x20) | ((long long)0xA132 << 0x10) | 0x1F80, 1) != 0) {
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) - 1;
    }
    func_0020B0D0(0, arg0);
    return;

case3:
    func_0020A7A0(((long long)0x20042585 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2100);
    func_0020AE40(arg0, D_00265B80, 1);
    func_0020B210(arg0, D_00265BF0, ((long long)0x20042605 << 0x20) | ((long long)0xA132 << 0x10) | 0x1F80, 0x401);
    func_0020B0D0(0, arg0);
    i = *(unsigned char *)(arg0 + 6) - 1;
    *(unsigned char *)(arg0 + 6) = i;
    if (((i & 0xFF) == 0) || (D_00810E74 & 0x5060)) {
        if (D_00810E74 & 0x5060) {
            func_0020CD60();
        }
        D_008106B0 = 0;
        *(unsigned char *)(arg0 + 5) = 1;
        *(unsigned char *)(arg0 + 6) = 0;
        D_00282240 = 3;
    }
}
