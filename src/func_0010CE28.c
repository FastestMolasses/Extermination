// NEARMISS func_0010CE28  (vram 0x0010CE28, 0xBC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.64% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Logic/frame/externs all correct (store-reload of D_00276E10 reproduced via volatile read; field-store order matches). Residual diff is pure ee-gcc register-coloring (0x20000000 mask held in v1 vs a0; D_00276F80/D_00276E40 bases swapped between a0/v0) plus one delay-slot scheduling swap (sw 0x8(v0) vs sh 0x2(v0) fill...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int DisableDmacHandler(int chan);
extern int func_0010C7E8(int a0, void *a1, void *a2);
extern int func_0010C9C8(int a0);
extern void sub_TTY_receive_error(void);
extern int D_00276E10;
extern int D_00276E40;
extern int D_00276F80;

int func_0010CE28(void) {
    int *s1;
    char *p;
    char *q;

    DisableDmacHandler(0);
    D_00276E10 = func_0010C7E8(0x210, &D_00276E10, sub_TTY_receive_error);
    if (*(volatile int *)&D_00276E10 < 0) {
        return 0;
    }
    s1 = &D_00276E10;
    s1[3] = 0;
    q = (char *)((int)&D_00276F80 | 0x20000000);
    p = (char *)((int)&D_00276E40 | 0x20000000);
    s1[1] = 0;
    s1[2] = 0;
    s1[5] = (int)q;
    s1[4] = (int)p;
    *(short *)(p + 4) = 0x210;
    *(char *)(p + 6) = 0x45;
    *(char *)(p + 7) = 0x48;
    *(int *)(p + 8) = 0;
    *(short *)(p + 2) = 0;
    s1[6] = func_0010C9C8(0x100);
    return 1;
}
