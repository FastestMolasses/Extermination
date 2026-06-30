// NEARMISS func_00101BE0  (vram 0x00101BE0, 0xDC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.36% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc reg-alloc + list-scheduler wall. Body structurally identical (88.4%). Two remaining diff classes: (1) register coloring of the two parallel arrays D_00241090/D_00241050 differs (expected a2/v1, ours v1/a1) propagating ARG_MISMATCH through the whole zeroing loop; (2) the volatile store `*(0x1000E010) &= 0xFF1F0...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00101B80(void *, int);
extern int func_00101CD0(void *);
extern int D_00241090[];
extern int *D_00241050[];

int func_00101BE0(int arg0) {
    int buf[8];
    int ret;
    int i;
    int *q = D_00241090;
    int **p = D_00241050;

    ret = *(volatile int *)0x1000E000 & 1;

    for (i = 9; i >= 0; i--) {
        if (q[i] != 0) {
            int *e = p[i];
            e[0x80 / 4] = 0;
            e[0] = 0;
            e[0x30 / 4] = 0;
            e[0x10 / 4] = 0;
            e[0x50 / 4] = 0;
            e[0x40 / 4] = 0;
        }
    }

    *(volatile int *)0x1000E010 = 0xFF1F;
    *(volatile int *)0x1000E010 = *(volatile int *)0x1000E010 & 0xFF1F0000;

    func_00101B80(buf, 20);
    func_00101CD0(buf);

    if (arg0 == 1) {
        *(volatile int *)0x1000E000 = *(volatile int *)0x1000E000 | 1;
    }
    return ret;
}
