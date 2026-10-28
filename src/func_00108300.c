// NEARMISS func_00108300  (vram 0x00108300, 0x1B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.88% via ee-gcc 2.9-991111-01 (-O2, 2.96 gas). The LOGIC and STRUCTURE are faithful.
// Fixes (matching wave 2026-10-27, was 87.21%): func_00109B20 / func_0010C3C8 / func_0010C360 /
// func_0010A3A8 return int (as their byte-matched callers declare them); the wait on the
// completion counter D_00241228[0] (bumped by the DMAC handler func_00108248) is a volatile
// re-read every iteration; the four queue words are stored in field order 0,1,2,3.
// Remaining differences: prologue only — the hi half of D_00241228 and the 0xFFC0 constant are
// coloured a1/a2 swapped versus the original (and the following 0x10002010 base follows it);
// declaration order, store order, struct/array/pointer spellings, parameter types and a
// 4000-iteration permuter run did not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// EEGCC_AS: 2.96
// (2.96 gas: matching wave 2026-10-27 sweep; it fills call/return delay slots as the original does)
// CFLAGS: -O2

extern int D_00241228[];
extern int D_00241220;
extern void *D_002412F4;
extern int func_00108248(void);
extern int AddDmacHandler(int chan, void *handler, int arg);
extern int func_0010C3C8(int chan);
extern int func_00109B20(int chan, void *p);
extern int func_0010A3A8(char *p);
extern int func_0010C360(int chan);
extern int RemoveDmacHandler(int chan, int id);
extern int D_0026B578;

void func_00108300(unsigned int a0, int a1) {
    int sp[8];
    int s0 = a1 - 0x3FF;
    int *t = D_00241228;
    int s3;

    t[0] = 0;
    t[1] = ((unsigned int)(a0 + (s0 << 10))) & 0xFFFFFFF;
    t[2] = 0xFFC0;
    t[3] = 0x3FF;
    D_00241220 = 0;

    while (*(volatile int *)0x10002010 < 0) {
    }

    s3 = AddDmacHandler(3, func_00108248, 0);
    *(volatile int *)0x1000E010 = 8;
    func_0010C3C8(3);

    *(volatile int *)0x1000B010 = a0 & 0xFFFFFFF;
    *(volatile int *)0x1000B020 = s0 << 6;
    *(volatile int *)0x1000B000 = 0x100;
    *(volatile int *)0x10002000 = s0 | 0x70000000;

    sp[0] = 4;
    func_00109B20((int)D_002412F4, sp);

    while (*(volatile int *)&D_00241228[0] < 2) {
    }

    if (D_00241220 != 0)
        func_0010A3A8((char *)&D_0026B578);

    while (*(volatile int *)0x10002010 < 0) {
    }

    func_0010C360(3);
    RemoveDmacHandler(3, s3);
}
