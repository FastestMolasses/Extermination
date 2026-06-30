// NEARMISS func_001DA080  (vram 0x001DA080, 0x158 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.81% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring permutation: body and structure are byte-correct except for a clean 3-way cyclic rename of the callee-saved regs (idx_a/idx_b/loop-counter -> mwcc colors s0/s1/s2, target colors s1/s2/s0). All 12 residual diffs are DIFF_ARG_MISMATCH register renames (no missing/extra/...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Picks two indices from an array of sub-objects at arg2: idx_a = the entry
// (>=1) whose func_00102738 distance against workspace `sp90` (built from arg3
// via func_00102948) is smallest, and idx_b = the entry whose distance against
// the global frame D_00817FC0 is largest. Entries are 4-byte pointers starting
// at arg2+0x110; the count is the byte at arg2+9; each entry's transform is at
// (*entry)+0xC0. Finally writes the two chosen transforms into arg0 and arg1
// via func_00102948.
//
// NEARMISS (mwcc233 98.8%, 991202 85.9%): body byte-identical; sole residual is
// a 3-register cyclic coloring permutation of the callee-saved regs
// (idx_a/idx_b/i). Permuter-class; left as documented near-miss.
extern float func_00102738(void *p, int a);
extern void func_00102948(void *p, int a);
extern int D_00817FC0;

void func_001DA080(void *arg0, void *arg1, char *arg2, int arg3) {
    char sp90[16];
    float best_a;
    float best_b;
    int idx_a;
    int idx_b;
    int i;
    char *p;

    func_00102948(sp90, arg3);
    idx_a = 1;
    best_a = func_00102738(sp90, *(int *)(arg2 + 0x114) + 0xC0);
    idx_b = 1;
    best_b = func_00102738(&D_00817FC0, *(int *)(arg2 + 0x114) + 0xC0);
    p = arg2 + 8;
    for (i = 2; i < (int)*(unsigned char *)(arg2 + 9); i++) {
        float va = func_00102738(sp90, *(int *)(p + 0x110) + 0xC0);
        if (va < best_a) {
            best_a = va;
            idx_a = i;
        }
        {
            float vb = func_00102738(&D_00817FC0, *(int *)(p + 0x110) + 0xC0);
            if (!(vb <= best_b)) {
                best_b = vb;
                idx_b = i;
            }
        }
        p += 4;
    }
    func_00102948(arg0, *(int *)(idx_a * 4 + arg2 + 0x110) + 0xC0);
    func_00102948(arg1, *(int *)(idx_b * 4 + arg2 + 0x110) + 0xC0);
}
