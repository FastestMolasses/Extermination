// NEARMISS func_0010F3B0  (vram 0x0010F3B0, 0xE0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 64.12% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely wall (confirmed s84). Decoded as a wrapper: s0=func_0010EFA8(); early-outs return -1 if D_00241C88==0, -9 if s0==0 or s0[1]==0; fills the D_00278C00 struct ([0]=s0[0], [+4]=a1, [+8]=a2); calls func_0010E8A8(&D_00279210, 4, ((u8*)s0)[5]>>7, &D_00278C00, 0xC, &D_00279040, 4, 0, stack0=0); o...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00278C00;
extern int D_00241C88;
extern int D_00279040;
extern int D_00279210;

extern void *func_0010EFA8(void);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int t0, void *t1, int t2, int t3, int s0);

int func_0010F3B0(int dummy, int a1, int a2)
{
    int *s0 = (int *)func_0010EFA8();

    if (D_00241C88 == 0)
        return -1;
    if (s0 == 0)
        return -9;
    if (s0[1] == 0)
        return -9;

    *(int *)((char *)&D_00278C00 + 4) = a1;
    D_00278C00 = s0[0];
    *(int *)((char *)&D_00278C00 + 8) = a2;

    {
        int r = func_0010E8A8(&D_00279210, 4, ((unsigned char *)s0)[5] >> 7,
                              &D_00278C00, 0xC, &D_00279040, 4, 0, 0);
        if (r < 0)
            return -1;
        if (s0[1] & 0x8000)
            return 0;
        return D_00279040;
    }
}
