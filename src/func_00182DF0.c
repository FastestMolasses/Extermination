// NEARMISS func_00182DF0  (vram 0x00182DF0, 0x1A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.18% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two artifacts at 99.18% (233): (1) mwcc hoists the lui at,0x7000 of the post-`b` 0x70003B8F store into the branch delay slot; target leaves nop (delay-slot scheduling). (2) FP register coloring on c.eq.s zero compares (target f0,f1 vs mwcc f1,f0). Both permuter-class scheduling/coloring, not the ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.18% (mwcc 2.3.3; 991202=81.59%). Body/structure byte-identical.
// Two residuals, both compiler artifacts: (1) mwcc 2.3.3 hoists the
// `lui at,0x7000` address-load of the 0x70003B8F store into the delay slot of
// the `b` that skips out of the bone-init branch, where the target leaves a
// nop (branch-delay-slot scheduling); (2) FP register coloring on the
// `c.eq.s` zero compares (target c.eq.s f0,f1 vs. mwcc f1,f0). Both are
// scheduling/coloring permutations, NOT the clean-store nop. The double
// `if (cur < 0)` mirrors the target's two `bltz s1` emissions reaching a
// single shared func_00174AB0 call (goto form keeps one call site).

extern signed char func_001C6150(int);
extern void bone_init_default_2(char *, short);
extern int func_00182D40();
extern short func_0017B490(char *, int, unsigned char, int);
extern void func_00174AB0(char *);
extern void func_00174A50(char *, float);

extern int D_0028A580;
extern short D_00248A00[];
extern short D_00248C90[];

void func_00182DF0(char *arg0) {
    short cur;

    if (*(unsigned char *)(arg0 + 0x2F3) != 0) {
        *(unsigned char *)(arg0 + 0x2F3) = 0;
        *(int *)(arg0 + 0x40) = D_0028A580;
        *(signed char *)(arg0 + 0xC) = func_001C6150(*(int *)(arg0 + 0x44));
        *(short *)(arg0 + 0x20C) = D_00248A00[*(unsigned char *)(arg0 + 0x235)];
        bone_init_default_2(arg0, *(short *)(arg0 + 0x20C));
    } else if (func_00182D40() == 0) {
        cur = *(short *)(arg0 + 0x20C);
        if (cur != func_0017B490(arg0, 0, *(unsigned char *)(arg0 + 0x235), 0)) {
            if (cur < 0) {
                goto callAB;
            }
            if (cur < 0) {
                goto domtc;
            }
            if (D_00248C90[cur * 6] != 0) {
                goto domtc;
            }
        callAB:
            func_00174AB0(arg0);
        domtc:
            func_00174A50(arg0, 16.0f);
        }
    }

    *(signed char *)0x70003B8F = 0;
    *(signed char *)(arg0 + 4) = 1;
    *(signed char *)(arg0 + 6) = 0;
    if (func_00182D40(arg0) == 0) {
        *(signed char *)(arg0 + 5) = 0;
        *(unsigned char *)(arg0 + 0x1F0) = 0;
    } else if (*(unsigned char *)(arg0 + 0x1F0) == 0x17) {
        *(signed char *)(arg0 + 5) = 0xC;
    }

    {
        char *o = *(char **)(arg0 + 0x1C);
        if (o != 0) {
            *(signed char *)(o + 4) = 1;
        }
    }

    if (*(float *)(arg0 + 0x224) != 0.0f || *(float *)(arg0 + 0x22C) != 0.0f) {
        *(int *)(arg0 + 0x224) = 0;
        *(int *)(arg0 + 0x22C) = 0;
        *(signed char *)(arg0 + 0) = 1;
    }
}
