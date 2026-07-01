// NEARMISS func_0021B550  (vram 0x0021B550, 0x2E4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residuals prevent 100.0 on mwcc233: (1) case-2 'st2 != 1 / st2 != 0' guard -- target keeps beqz+nop+explicit-b (3 insns) for the fallthrough-to-default path, mwcc233 collapses the logically-equivalent double-negative test into a single bnez (1 insn shorter); tried nested if, chained if/break,...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// State machine for the loading-screen particle effect driven off the global
// struct at D_00275888 (state byte at +0, sub-state bytes at +1/+2, float
// intensity fields at +8/+0xC/+0x10). State 0: sub-state 0/1 kick particle
// groups 2 and 3 via func_001D2830(group, enable) and bump the sub-state;
// sub-state >=2 resets the three intensity floats and advances to state 1.
// State 1: kicks group 3, ramps each intensity float up (by 0.01/0.008/0.006)
// clamped to 1.0, then updates the particle system via func_0021B1B0/
// func_0021B500. State 2: sub-state 1 kicks group 2, then a further sub-state
// counter at +2 (0/1/2 bump, else -> state 3, "done"); sub-state 0 decays the
// three intensity floats (*0.9/0.92/0.94), kicks group 3, updates via
// func_0021B1B0/func_0021B500, and once all three floats are below 0.01
// advances the sub-state to 1. State 3 (default): kicks group 2 and returns 1
// (effect finished); all other states return 0.
//
// NEARMISS 98.4% (mwcc 2.3.3; 991202 = 82.1%). Body and control flow are
// fully recovered, including the branch-likely dispatch on case 2's
// sub-state (`beql st2==1`, delay slot reusing the outer switch's literal-2
// value for func_001D2830's first arg) and the fresh-reload-after-call shape
// (state bytes are NOT carried across func_001D2830 calls; they are reloaded
// from memory post-call, matching target's register pressure and avoiding a
// spurious $s2 save). The residual is a scheduling/coloring artifact
// (permuter class), not a logic gap.
extern void func_001D2830(int group, int enable);
extern void func_0021B1B0(char *particleState);
extern void func_0021B500(char *particleState);
extern char *D_00275888;

int func_0021B550(void) {
    char *s = D_00275888;
    int ret = 0;
    unsigned char st = *(unsigned char *)(s + 0);
    unsigned char st2;
    unsigned char st3;
    float f;

    switch (st) {
    case 0:
        st2 = *(unsigned char *)(s + 1);
        switch (st2) {
        case 0:
        case 1:
            func_001D2830(2, 0);
            func_001D2830(3, 1);
            *(unsigned char *)(s + 1) = *(unsigned char *)(s + 1) + 1;
            break;
        default:
            *(int *)(s + 0x10) = 0;
            *(int *)(s + 0xC) = 0;
            *(int *)(s + 8) = 0;
            *(unsigned char *)(s + 0) = 1;
            break;
        }
        break;
    case 1:
        func_001D2830(3, 1);
        f = *(float *)(s + 8) + 0.01f;
        f = (f < 1.0f) ? f : 1.0f;
        *(float *)(s + 8) = f;
        f = *(float *)(s + 0xC) + 0.008f;
        f = (f < 1.0f) ? f : 1.0f;
        *(float *)(s + 0xC) = f;
        f = *(float *)(s + 0x10) + 0.006f;
        f = (f < 1.0f) ? f : 1.0f;
        *(float *)(s + 0x10) = f;
        func_0021B1B0(s);
        func_0021B500(s);
        break;
    case 2:
        st2 = *(unsigned char *)(s + 1);
        if (st2 != 1) {
            if (st2 != 0) {
                break;
            }
            *(float *)(s + 8) = *(float *)(s + 8) * 0.9f;
            *(float *)(s + 0xC) = *(float *)(s + 0xC) * 0.92f;
            *(float *)(s + 0x10) = *(float *)(s + 0x10) * 0.94f;
            func_001D2830(3, 1);
            func_0021B1B0(s);
            func_0021B500(s);
            if (*(float *)(s + 8) < 0.01f && *(float *)(s + 0xC) < 0.01f && *(float *)(s + 0x10) < 0.01f) {
                *(unsigned char *)(s + 1) = 1;
            }
            break;
        }
        func_001D2830(2, 0);
        st3 = *(unsigned char *)(s + 2);
        switch (st3) {
        case 0:
        case 1:
        case 2:
            func_001D2830(3, 1);
            *(unsigned char *)(s + 2) = *(unsigned char *)(s + 2) + 1;
            break;
        default:
            *(char *)(s + 0) = 3;
            break;
        }
        break;
    default:
    case 3:
        func_001D2830(2, 0);
        ret = 1;
        break;
    }
    return ret;
}
