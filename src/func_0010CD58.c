// NEARMISS func_0010CD58  (vram 0x0010CD58, 0xD0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.50% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc register-coloring permutation wall (permuter territory). Instruction stream is byte-identical to expected (same ops, same shapes, frame size 96 matched, lbu/lb matched, branch structure matched after restructuring); the ONLY residual diff is GPR coloring: expected holds the loop index in v1 and the two saved t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char *D_00276E28;
extern int D_00276E10;
extern void func_0010CA30(int);

int func_0010CD58(char *buf, int count) {
    int i = 0;
    if (count > 0) {
        do {
            int *base;
            int next = i + 1;
            signed char r;
            while (*(volatile int *)((char *)D_00276E28 + 4) == 0)
                ;
            base = (int *)&D_00276E10;
            buf[i] = *(unsigned char *)(*(int *)(base[6] + 8));
            func_0010CA30(base[6]);
            r = buf[i];
            if (r == 0xA)
                return next;
            if (r == 0xD)
                return next;
            i = next;
        } while (i < count);
    }
    return i;
}
