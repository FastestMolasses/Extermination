// NEARMISS func_0019A570  (vram 0x0019A570, 0x178 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.72% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation COLORING permuter near-miss (84.72% mwcc 2.3.3). Structure+frame size (0x50, s0/s1/s2 saved) match the target; residual is which callee-saved/temp regs hold the local-array pointer ($s2 vs $s0) and the loop counters, plus prologue materialization order of &D_70003190. No sourc...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019D330(void);
extern int func_001A0B10(void);
extern int func_001A6440(int, float *);
extern char D_70003190[];

int func_0019A570(float *arg0, float *arg1, int arg2, int arg3) {
    float local[4];
    char *t0;
    float *a0;
    float *a1;
    float *s2;
    float *t1;
    int i;
    int flags;
    int ret;

    a0 = arg0;
    a1 = arg1;
    s2 = local;
    t0 = D_70003190;
    ret = 0;
    i = 0;
    t1 = s2;
    do {
        i += 1;
        *(float *)(t0 + 0) = *a0;
        *t1 = *a1;
        *(float *)(t0 + 0x10) = *a1;
        a0 += 1;
        t0 += 4;
        a1 += 1;
        t1 += 1;
    } while (i < 3);
    *(int *)0x700031AC = 0x3F800000;
    *(int *)0x7000319C = 0x3F800000;
    flags = arg2 & 0xFF;
    *(float *)&local[3] = 1.0f;
    *(int *)0x700031D4 = 0;
    if (flags & 1) {
        if (func_001A6440(arg3 & 0xFFFF, a1) != 0) {
            char *q = D_70003190;
            int j = 0;
            do {
                j += 1;
                *(float *)(q + 0x10) = *(float *)(q + 0x20);
                q += 4;
            } while (j < 3);
            ret = 1;
        }
    }
    *(short *)0x7000324E = -1;
    if (flags & 2) {
        *(int *)0x70003254 = 0;
        if (func_001A0B10() != 0) {
            ret = 2;
        }
    }
    if ((flags & 4) && func_0019D330() != 0) {
        ret = 4;
    }
    if (ret != 0) {
        char *q = D_70003190;
        int j = 0;
        do {
            j += 1;
            *(float *)(q + 0x10) = *s2;
            s2 += 1;
            q += 4;
        } while (j < 3);
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = ret;
    return ret;
}
