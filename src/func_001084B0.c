// NEARMISS func_001084B0  (vram 0x001084B0, 0x154 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.31% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc coloring/reg-alloc wall (confirmed s84). Decoded to correct C (DMA setup: wait on 0x10002010 sign bit, write 0x1000B410/B420/B400 GIF regs, dispatch func_00108160 vs func_00108300 on n<0x400, bracketed by func_00109B20(D_002412F4,&buf) with buf[0]=2 then 3). Frame size fixed to 0x80 (buf[8]). Residual diffs ar...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_002412F4;

extern void func_00109B20(int, int *);
extern void func_001063B8(int);
extern void func_00108160(int, int);
extern void func_00108300(int, int);

void func_001084B0(int *a0) {
    int buf[8];
    int n;
    int small;
    int dev;

    dev = *(int *)(D_002412F4 + 0x40);
    n = a0[3] * a0[4];
    buf[0] = 2;
    func_00109B20(D_002412F4, buf);

    if (*(volatile unsigned int *)0x10002010 & 0x4000) {
        *(volatile unsigned int *)0x10002010 = 0x40000000;
    }

    small = n < 0x400;

    while (*(volatile int *)0x10002010 < 0)
        ;
    func_001063B8(0);

    while (*(volatile int *)0x10002010 < 0)
        ;

    *(volatile unsigned int *)0x1000B410 = (unsigned int)a0[0] & 0x0FFFFFFF;
    *(volatile unsigned int *)0x1000B420 = n * 0x18;
    *(volatile unsigned int *)0x1000B400 = 0x101;

    if (small) {
        func_00108160(*(int *)(dev + 0xD8), n);
    } else {
        func_00108300(D_002412F4, n);
    }

    buf[0] = 3;
    func_00109B20(D_002412F4, buf);
}
