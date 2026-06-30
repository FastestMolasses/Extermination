// NEARMISS func_001B30E0  (vram 0x001B30E0, 0x168 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation. Body/structure/control-flow/branch-senses/FP-compare ordering all match the target 1:1 (no inserted/deleted/reordered instructions). Sole residual is a consistent saved-register renaming: target colors the three induction pointers s2/s3/s4 and the two scalars (set...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011DF78(float);
extern float D_00282250;
extern float D_700030F0;
extern unsigned short D_70003170;

int func_001B30E0(char *arg0, float *arg1) {
    unsigned short *pf;
    float *pa;
    float *pt;
    int set;
    int i;

    if (*(int *)0x700031E0 == 0) {
        return 0;
    }
    pf = &D_70003170;
    pa = &D_00282250;
    pt = &D_700030F0;
    set = 0;
    for (i = 0; i < *(int *)0x700031E0; i++) {
        if (*pf & 1) {
            if (func_0011DF78(*pa) <= 1.0471976f) {
                if (set == 0) {
                    set = 1;
                    *arg1 = *pt;
                } else if (*arg1 < *pt) {
                    if (*(float *)(arg0 + 4) < *pt) {
                        return 1;
                    }
                    *arg1 = *pt;
                }
            }
        } else {
            if (set != 0) {
                if (*(float *)(arg0 + 4) <= *pt) {
                    return 1;
                }
            }
        }
        pf++;
        pa++;
        pt++;
    }
    return set;
}
