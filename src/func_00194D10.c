// NEARMISS func_00194D10  (vram 0x00194D10, 0x94 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-compare branch-lowering wall + arg-eval scheduling. Logic fully recovered. Residual 1: target schedules the a2 pointer setup (addu a2,v0,s0) before a1 setup (addiu a1,s1,0xa0); mwcc emits a1 first. Residual 2: CW leaves the `bc1f` delay slot as a nop and trails a dead `b/paddub v0,zero` block;...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (233: 84.7%, 991: 69.0%) — logic fully recovered; sole residuals
// are compiler artifacts. Tests entity a1 against animation table entry a2:
// runs func_001B1EA0(0, a1+0xA0, &D_0024A5F0[a2], 4); if that succeeds,
// computes func_0011DF78(*(float*)(a1+0xA4) - D_0024A5F4[a2]) and returns 1
// when the result is below 4.0f, else 0; returns 0 if the probe fails.
// D_0024A5F0/D_0024A5F4 are two fields (off +0x00 / +0x04) of a stride-0x40
// table indexed by a2.
//
// Residual 1: target schedules the a2 pointer setup (addu a2,v0,s0) before
// the a1 setup (addiu a1,s1,0xa0); mwcc emits a1 first — arg-eval ordering.
// Residual 2: the FP-compare tail. CW leaves the `bc1f` delay slot as a nop
// and trails a dead `b/paddub v0,zero` block; both mwcc builds (incl. 2.3.3)
// fill the bc1f slot with the false-arm store and merge the block. This is
// the mwcc-vs-CW FP-compare branch-lowering wall (unlike the integer-compare
// case in anim_clip_arbiter, 2.3.3 does NOT emit the dead block here).
extern int func_001B1EA0(int, int, int, int);
extern float func_0011DF78(float);
extern char D_0024A5F0[];
extern char D_0024A5F4[];

int func_00194D10(int a0, char *a1, int a2) {
    int off = a2 * 0x40;
    float d;

    if (func_001B1EA0(0, (int)(a1 + 0xA0), (int)(D_0024A5F0 + off), 4) != 0) {
        d = func_0011DF78(*(float *)(a1 + 0xA4) - *(float *)(D_0024A5F4 + off));
        if (d < 4.0f) {
            return 1;
        }
        return 0;
    }
    return 0;
}
