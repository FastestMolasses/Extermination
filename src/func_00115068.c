// NEARMISS func_00115068  (vram 0x00115068, 0xFC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.83% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler adjacent-op swap wall (deterministic, no permuter lever). Body is otherwise byte-identical -- confirmed by raw objdump: every instruction from offset 0x98 onward is bit-for-bit equal (the t0-t3/a4-a7 region objdiff flags is the SAME physical regs $8-$11, only cosmetically relabeled, non-counting...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void func_00123418(char *dst, int a1, int a2);
extern int DisableDmacHandler(int ch);
extern int func_0010E8A8(int a0,int a1,int a2,int a3,int t0,int t1,int t2,int t3,int s0);
extern int D_00241D68;
extern int D_0027B0C0[];
extern char D_0027B170[];
extern char D_0027B120[];
extern int D_0027C680;

int func_00115068(int a0, int a1, int a2, int a3) {
    int s2 = a3;
    int s3v;
    char *s1;
    char *s0;
    int v0 = D_00241D68;

    if (v0 != 0) {
        return v0;
    }
    s3v = (int)D_0027B0C0;
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }

    s1 = D_0027B170;
    *(int *)D_0027B170 = a0;
    *(int *)(s1 + 4) = a1;
    *(int *)(s1 + 8) = 0x10;
    func_00123418(s1 + 0x14, a2, 0x3FF);

    s0 = D_0027B120;
    *(char *)(s1 + 0x413) = 0;
    func_00123418(s0, s2, 0x20);
    s0 -= 0x20;
    *(char *)(s0 + 0x3F) = 0;
    *(int *)(s1 + 0x10) = (int)s0;
    DisableDmacHandler(0);

    {
        int v1 = func_0010E8A8(s3v, 0xE, 1, (int)s1, 0x414, (int)&D_0027C680, 4, 0, 0);
        if (v1 == 0) {
            D_00241D68 = 0x13;
        }
        return v1;
    }
}
