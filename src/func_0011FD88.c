// NEARMISS func_0011FD88  (vram 0x0011FD88, 0x104 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.66% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc register-coloring wall. Logic fully recovered and body byte-identical APART from a systematic register-pair swap: expected uses s1=fp(arg)/s2=p(write-ptr)/v1=q/v1=cur, our ee-gcc 2.9-991111-01 emits the mirror (s2=fp/s1=p/v0=q/v0=cur). Nearly every residual DIFF line is exactly an s1<->s2 or v0<->v1 swap, plus...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_0024295C;
extern int func_001209B0(int, int (*)());
extern void func_00120058(int);

int func_0011FD88(int fp)
{
    int q;
    int p;
    int n;
    int cur;

    if (fp == 0) {
        return func_001209B0(D_0024295C, func_0011FD88);
    }

    q = *(int *)(fp + 0x54);
    if (q == 0) {
        q = D_0024295C;
        *(int *)(fp + 0x54) = q;
    }

    if (*(int *)(q + 0x38) == 0) {
        func_00120058(*(short *)(fp + 0xC));
    }

    if ((*(short *)(fp + 0xC) & 0x8) == 0) {
        return 0;
    }

    p = *(int *)(fp + 0x10);
    if (p == 0) {
        return 0;
    }

    n = *(int *)(fp + 0x0) - p;
    *(int *)(fp + 0x0) = p;
    if ((*(short *)(fp + 0xC) & 0x3) == 0) {
        p = *(int *)(fp + 0x14);
    }
    *(int *)(fp + 0x8) = p;

    while (n > 0) {
        cur = (*(int (**)(int, int, int))(fp + 0x24))(*(int *)(fp + 0x1C), p, n);
        if (cur <= 0) {
            *(short *)(fp + 0xC) = *(unsigned short *)(fp + 0xC) | 0x40;
            return -1;
        }
        n -= cur;
        p += cur;
    }
    return 0;
}
