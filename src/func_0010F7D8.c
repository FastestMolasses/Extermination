// NEARMISS func_0010F7D8  (vram 0x0010F7D8, 0x98 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.79% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eabi frame-size stride wall: body is byte-identical (branch-likely region, all loads/stores/branches match exactly). Only diff is the frame immediate: expected reserves 0x40 with saves s0@0x10/s1@0x20/ra@0x30 (16 bytes outgoing-arg space below saves, with NO stack store), mine emits 0x30. No matched eegcc func in tr...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void *func_0010EFA8(int);
extern int func_0010EA60(void *);

extern int D_00241C88;
extern char D_00279210;
extern int D_00279040;

int func_0010F7D8(int a0, int a1, int *a2) {
    int *p;
    int flags;

    p = (int *)func_0010EFA8(a0);
    if (D_00241C88 == 0)
        return -1;
    if (p == 0)
        return -9;
    flags = p[1];
    if (flags == 0)
        return -9;
    if (a1 == 1) {
        if (func_0010EA60(&D_00279210) == 0)
            *a2 = 0;
        else
            *a2 = a1;
        return 0;
    }
    if ((flags & 0x8000) != 0)
        return 0;
    return D_00279040;
}
