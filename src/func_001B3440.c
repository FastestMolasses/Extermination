// NEARMISS func_001B3440  (vram 0x001B3440, 0xA4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.07% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Delay-slot move scheduling on first call result: CW tests raw return reg (bnez v0) with `paddub s0,v0` in the delay slot; mwcc hoists the callee-saved copy before the branch (bnez s0) and fills the slot with the next call's arg setup. Body/structure byte-identical; permuter-class scheduling artif...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019AB20(int a0, int a1, int a2, int a3);
extern int func_0019A310(int a0);
extern int func_001028B8(int a0, int a1, int a2);
extern int D_700031C0;

int func_001B3440(int a0, int *a1, int *a2, float f) {
    int r;
    float t;

    *(int *)((char *)a2 + 0xC) = 0x3F800000;
    *(int *)((char *)a1 + 0xC) = 0x3F800000;
    r = func_0019AB20(a0, (int)a1, (int)a2, 7);
    if (r == 0) {
        return 0;
    }
    if (func_0019A310((int)((char *)&t)) == 0) {
        return 0;
    }
    if (!(t <= f)) {
        return 0;
    }
    func_001028B8(a0 + 0xB0, a0 + 0xB0, (int)&D_700031C0);
    return r;
}
