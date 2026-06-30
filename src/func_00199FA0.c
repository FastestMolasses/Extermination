// NEARMISS func_00199FA0  (vram 0x00199FA0, 0x1DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Permuter-class register coloring on a fully-recovered body. Branch/early-exit structure matches, but the target keeps the 0x700031D0 state pointer (v1) and entry value (v0) in those regs throughout while mwcc colors into a2/a3/v0/v1 differently, cascading DIFF_ARG_MISMATCH across the guard compar...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern char D_700030B0;

int func_00199FA0(char *arg0, char *arg1) {
    int r;
    int v1;
    char *a2;
    int t;
    int base;

    r = 0;
    v1 = *(int *)0x700031D0;
    if (v1 == 0) {
        goto end;
    }
    if (v1 == (int)&D_700030B0) {
        a2 = *(char **)0x700031D4;
        if (a2 == 0) {
            goto end;
        }
        if (*(int *)0x700031D8 != 2) {
            goto end;
        }
        t = (int)*(unsigned short *)(a2 + 0xE) >> 8;
        if (t == 0xFF) {
            goto end;
        }
        base = *(int *)0x70003250;
        t = *(int *)(base + t * 4 + 4);
        if (t != 0) {
            v1 = base + t;
            *(float *)(arg0 + 0) = *(float *)(v1 + 0);
            *(float *)(arg0 + 4) = *(float *)(v1 + 4);
            *(float *)(arg0 + 8) = *(float *)(v1 + 8);
            *(float *)(arg1 + 0) = *(float *)(v1 + 0xC);
            *(float *)(arg1 + 4) = *(float *)(v1 + 0x10);
            *(float *)(arg1 + 8) = *(float *)(v1 + 0x14);
            r = 1;
        }
        goto end;
    }
    r = 1;
    *(float *)(arg0 + 0) = *(float *)(*(int *)0x700031FC + *(short *)(v1 + 0) * 0xC);
    *(float *)(arg1 + 0) = *(float *)(*(int *)0x700031FC + *(short *)(*(int *)0x700031D0 + 2) * 0xC);
    *(float *)(arg0 + 4) = *(float *)(*(short *)(*(int *)0x700031D0 + 4) * 0xC + *(int *)0x700031FC + 4);
    *(float *)(arg1 + 4) = *(float *)(*(short *)(*(int *)0x700031D0 + 6) * 0xC + *(int *)0x700031FC + 4);
    *(float *)(arg0 + 8) = *(float *)(*(short *)(*(int *)0x700031D0 + 8) * 0xC + *(int *)0x700031FC + 8);
    *(float *)(arg1 + 8) = *(float *)(*(short *)(*(int *)0x700031D0 + 0xA) * 0xC + *(int *)0x700031FC + 8);
end:
    return r;
}
