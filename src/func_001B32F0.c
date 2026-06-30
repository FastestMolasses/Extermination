// NEARMISS func_001B32F0  (vram 0x001B32F0, 0xA0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.88% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Delay-slot move scheduling on the first call result. Entire tail (3 if-guards, c.le.s test, both return-0 dead blocks, func_001028B8 call, return r) is byte-identical. Sole residual: after `jal func_0019AD00`, CW tests the raw return register `bnez v0` and fills its delay slot with `paddub s0,v0`...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_0019AD00(int a0, int a1, int a2);
extern int func_0019A310(int a0);
extern int func_001028B8(int a0, int a1, int a2);
extern int D_700031C0;

int func_001B32F0(int a0, int *a1, float f) {
    int r;
    float t;
    *(int *)((char *)a1 + 0xC) = 0x3F800000;
    r = func_0019AD00(a0, (int)a1, 7);
    if (r == 0) {
        return 0;
    }
    if (func_0019A310((int)((char *)&t)) == 0) {
        return 0;
    }
    if (t <= f) {
        return 0;
    }
    func_001028B8(a0 + 0xB0, a0 + 0xB0, (int)&D_700031C0);
    return r;
}
