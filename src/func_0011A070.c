// NEARMISS func_0011A070  (vram 0x0011A070, 0x124 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.68% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc coloring + list-scheduler wall (confirmed s84, no permuter lever). The loop body is byte-identical; residual is entirely (a) prologue sd-store ordering (s5/s6 swapped), (b) first-block temporary GPR coloring (base in v1 vs a1, mult dest a2 vs a0, lhu v1 vs v0) for the D_0027E0C0[idx*0x78] address/func_00121A28...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027E0C0[];
extern unsigned char D_0027CCC0[];
extern unsigned char D_0027F740[];

extern void func_00121A28(void *p, int a1, int a2);
extern void func_001157F0(int a0, int a1, int a2, int a3);

void func_0011A070(int arg0) {
    unsigned int idx = (unsigned int)(arg0 & 0x7FFF);
    unsigned char *base = D_0027E0C0;

    if (idx < 0x30) {
        unsigned char *e = base + idx * 0x78;
        if (*(unsigned short *)(e + 0x32) == 1 && *(unsigned short *)(e + 0x2E) == 0) {
            func_00121A28(e, 0, 0x78);
        }
    }

    {
        int flag = arg0 & 0x8000;
        unsigned char *p = D_0027CCC0;
        unsigned char *st = D_0027F740;
        long mask = 1;
        int i = 0;
        do {
            if (*(unsigned short *)(p + 0x1A) == 2 &&
                *(unsigned short *)(p + 6) == idx) {
                if (flag) {
                    func_001157F0(3, i, 0, 0);
                    *(unsigned short *)p = 0;
                }
                *(long *)(st + 0x28) |= mask;
                *(unsigned short *)(p + 8) = 1;
            }
            p += 0x6A;
            i++;
            mask <<= 1;
        } while (i < 0x30);
    }
}
