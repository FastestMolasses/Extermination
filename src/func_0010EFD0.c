// NEARMISS func_0010EFD0  (vram 0x0010EFD0, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.67% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc hand-coded-asm delay-loop wall. Fully decompiled: calls func_0010E088(0); retry-loop calling func_0010E6F8(&D_00279210,0x80000001,0) until *(D_00279210+0x24)!=0 (busy-wait between tries); clears field+4 across the 0x100-byte table at D_00279110 in 0x10 strides; func_0010DFD8(&D_00279080,0x40); func_0010E8A8(&D...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010E088(int);
extern int func_0010E6F8(void *, unsigned int, int);
extern int func_0010DFD8(void *, int);
extern int func_0010E8A8(void *, int, int, int, int, int *, int);

extern unsigned char D_00279210[];
extern unsigned char D_00279110[];
extern unsigned char D_00279040[];
extern unsigned char D_00279080[];
extern unsigned char D_00279238[];
extern int D_00241C88;

int func_0010EFD0(void)
{
    int ret;

    func_0010E088(0);

    for (;;) {
        ret = func_0010E6F8(D_00279210, 0x80000001u, 0);
        if (ret < 0)
            return -1;
        if (*(int *)(D_00279210 + 0x24) != 0)
            break;
        {
            volatile int i;
            for (i = 0xFFFFF0000; i != -1; i++)
                ;
        }
    }

    {
        unsigned char *p = D_00279110;
        unsigned char *end = p + 0x100;
        if (p < end) {
            do {
                *(int *)(p + 4) = 0;
                p += 0x10;
            } while (p < end);
        }
    }

    func_0010DFD8(D_00279080, 0x40);

    if (func_0010E8A8(D_00279210, 0xFF, 0, 0, 0, (int *)D_00279040, 4) < 0)
        return (int)0xFFFEFFFF;

    *(int *)(D_00279238) = *(int *)(D_00279040);
    D_00241C88 = 1;
    return 0;
}
