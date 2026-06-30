// NEARMISS func_001A9B10  (vram 0x001A9B10, 0x130 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + scheduling permutation (NOT clean-store nop). Body/structure 100% recovered. Target colors the two list pointers outer=s2/inner=s0; mwcc 2.3.3 picks the swap outer=s0/inner=s2. Plus minor delay-slot fill diffs (target nop where mwcc hoists counter-reload lui; inner f[3]==7 tes...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Double-loop interaction pass over two global object lists. Outer list: base D_00275B90, count
// D_00275B98, walked via a global short countdown at 0x70003B86 (seeded from D_00275B98). Inner
// list: base D_00275B80, count D_00275B88, walked via a global short countdown at 0x70003B88
// (seeded from D_00275B88). Runs only if both counts are nonzero. For each outer entry e with
// e[3]==0 whose type field (*(int*)(e+0x2D4) >> 8) is 1, 2, or 3, it walks the inner list: for
// each inner entry f with f[3]==7 and a nonzero float at f+0x38, it calls func_001A99E0(e). Each
// counter is decremented and re-tested per iteration so a nested handler can shorten the walk
// (func_001A99E0 itself zeroes 0x70003B88).
//
// Residual is pure register coloring + scheduling: the target colors the two list pointers as
// outer=s2 / inner=s0; mwcc 2.3.3 picks the swap (outer=s0 / inner=s2). A few branch delay slots
// also differ (target leaves nop where mwcc hoists the counter-reload lui, and the inner f[3]==7
// test lowers to beq+b in the target vs bne here). Not the clean-store nop wall -> permuter-class,
// 2.3.3 cannot reach 100.
extern void func_001A99E0(unsigned char *);
extern unsigned char **D_00275B80;
extern short D_00275B88;
extern unsigned char **D_00275B90;
extern short D_00275B98;

void func_001A9B10(void) {
    int n;
    unsigned char **q;
    unsigned char *e;
    unsigned char **p;
    unsigned char *f;
    int t;

    n = D_00275B98;
    *(short *)0x70003B86 = n;
    if (n == 0) return;
    if (D_00275B88 == 0) return;
    q = D_00275B90;
    while (*(volatile short *)0x70003B86 != 0) {
        e = *q++;
        *(short *)0x70003B86 = *(volatile short *)0x70003B86 - 1;
        if (e[3] != 0) continue;
        t = *(int *)(e + 0x2D4) >> 8;
        switch (t) {
        case 1:
        case 2:
        case 3:
            *(short *)0x70003B88 = D_00275B88;
            p = D_00275B80;
            while (*(volatile short *)0x70003B88 != 0) {
                f = *p++;
                *(short *)0x70003B88 = *(volatile short *)0x70003B88 - 1;
                if (f[3] == 7) {
                    if (*(float *)(f + 0x38) != 0.0f) {
                        func_001A99E0(e);
                    }
                }
            }
            break;
        }
    }
}
