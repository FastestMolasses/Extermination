// NEARMISS func_00199C50  (vram 0x00199C50, 0x158 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Loop-counter / table-index register coloring + minor scheduling of the 2nd loop's mult setup. Body and table-build logic fully correct (base-pointer reuse and step-hoist already matched). Regalloc-coloring/scheduling permutation class; not the clean-store nop.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 90.9% (mwcc233) / 86.2% (991202). Body/structure fully recovered.
// EE-scratchpad (0x70003xxx) initializer: loads base pointer D_0028A598 once
// (kept in dbase, reused for every '+offset' add, matching the target's single
// a3-held base), reloads it from scratchpad slot 0x700031F8 into 'base' for the
// struct-field reads, then builds two 6-entry tables at D_70003214 / D_70003228
// with stride 'step = *0x7000320C * 2' (hoisted, matching the target). Volatile
// casts reproduce CW's raw $at absolute-addressing (idiom 8). Residual is purely
// loop-counter / index register COLORING and minor schedule of the 2nd-loop mult
// setup -- permuter territory, not the clean-store nop.

extern char *D_0028A598;
extern short *D_0028A5A8;
extern int D_70003214[];
extern int D_70003228[];

void func_00199C50(void) {
    char *dbase;
    char *base;
    int *p;
    int i;
    int j;
    int delta;
    int k;
    int step;

    dbase = D_0028A598;
    *(char *volatile *)0x700031F8 = dbase;
    base = *(char *volatile *)0x700031F8;
    *(char *volatile *)0x700031FC = dbase + *(int *)(base + 0x0);
    *(char *volatile *)0x70003200 = dbase + *(int *)(base + 0x8);
    *(char *volatile *)0x70003204 = dbase + *(int *)(base + 0x10);
    *(char *volatile *)0x70003208 = dbase + *(int *)(base + 0x20);
    *(volatile int *)0x7000320C = *(short *)(base + 0x24);
    step = *(int *)0x7000320C * 2;
    *(char *volatile *)0x70003210 = dbase + *(int *)(base + 0x18);

    p = D_70003214;
    i = 1;
    do {
        i += 1;
        p[0] = p[-1] + step;
        p += 1;
    } while (i < 6);

    if (*(int *)(*(char *volatile *)0x700031F8 + 0x1C) == 0xC) {
        p = D_70003228;
        j = 0;
        step = *(int *)0x7000320C * 2;
        do {
            delta = (j + 1) * step;
            j += 1;
            *p = *(int *)0x70003224 + delta;
            p += 1;
        } while (j < 6);
    } else {
        p = D_70003228;
        k = 0;
        do {
            k += 1;
            *p = 0;
            p += 1;
        } while (k < 6);
    }

    *(short *volatile *)0x70003250 = D_0028A5A8;
    *(volatile short *)0x7000324C = **(short *volatile *)0x70003250;
}
