// NEARMISS func_00108300  (vram 0x00108300, 0x1B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.50% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall. Body matches 100% from RemoveIntcHandler onward; only the prologue diverges: the deterministic ee-gcc scheduler interleaves the callee-saved sd stores with the four struct stores to D_00241228 in a different order (expected 0xC,0,4,8) and consequently colors the constant/base GPRs differen...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241228[];
extern int D_00241220;
extern void *D_002412F4;
extern void func_00108248(void);
extern int RemoveIntcHandler(int chan, void (*handler)(void), void *arg);
extern void func_0010C3C8(int chan);
extern void func_00109B20(int chan, void *p);
extern void func_0010A3A8(void *p);
extern void func_0010C360(int chan);
extern int AddDmacHandler(int chan, int id);
extern int D_0026B578;

void func_00108300(unsigned int a0, int a1) {
    int sp[8];
    int s0 = a1 - 0x3FF;
    int *t = D_00241228;
    int s3;

    t[3] = 0x3FF;
    t[0] = 0;
    t[1] = ((unsigned int)(a0 + (s0 << 10))) & 0xFFFFFFF;
    t[2] = 0xFFC0;
    D_00241220 = 0;

    while (*(volatile int *)0x10002010 < 0) {
    }

    s3 = RemoveIntcHandler(3, func_00108248, 0);
    *(volatile int *)0x1000E010 = 8;
    func_0010C3C8(3);

    *(volatile int *)0x1000B010 = a0 & 0xFFFFFFF;
    *(volatile int *)0x1000B020 = s0 << 6;
    *(volatile int *)0x1000B000 = 0x100;
    *(volatile int *)0x10002000 = s0 | 0x70000000;

    sp[0] = 4;
    func_00109B20((int)D_002412F4, sp);

    while (D_00241228[0] < 2) {
    }

    if (D_00241220 != 0)
        func_0010A3A8(&D_0026B578);

    while (*(volatile int *)0x10002010 < 0) {
    }

    func_0010C360(3);
    AddDmacHandler(3, s3);
}
