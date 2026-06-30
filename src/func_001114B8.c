// NEARMISS func_001114B8  (vram 0x001114B8, 0x64 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.20% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Branch-likely wall: original emits `bgezl $v0` (branch-likely) in the post-jal sign test, but the rebuilt ee-gcc 2.9-991111-01 cc1 never emits branch-likely (bgezl/bnel/beql) - confirmed across -O0/-O1/-O2/-mips2/-mips3, always produces `bltz` plus an extra `move v0,v1`, giving size 0x68 vs expected 0x70. All other ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00279A80;
extern unsigned char D_00279700[];
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_001114B8(void)
{
    int *s0 = &D_00279A80;

    D_00279A80 = 0x12;
    return func_0010E8A8(D_00279700, 1, 0, s0, 0x80, s0, 0x80, 0, 0) >= 0 ? s0[3] : 0;
}
