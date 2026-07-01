// NEARMISS func_001B18F0  (vram 0x001B18F0, 0x238 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation: mwcc 2.3.3 (and 991202) map the three pointer params to callee-saved regs as arg0->s2, arg1->s1, arg2->s0 (descending), while the CW target maps arg0->s0, arg1->s2, arg2->s1. Body/structure/control-flow are 100% correct (the shared func_001B1B70 tail with th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS wall: register-allocation-order permutation. The logic, structure and
// the shared func_001B1B70 return-1 tail all match 100%; the only residual is the
// arg->callee-saved-register coloring -- mwcc assigns arg0->s2, arg1->s1,
// arg2->s0 whereas the CW target uses arg0->s0, arg1->s2, arg2->s1. No source
// change alters mwcc's fixed descending allocation here (verified). Pure regalloc
// permutation -> permuter territory.
extern void func_001026A0(float *, void *, float *);
extern int func_001B1630(float, float, float);
extern void func_001B1B70(void *);
extern float D_70003600[4];

int func_001B18F0(char *arg0, float *arg1, float *arg2) {
    float *p = (float *)0x70003600;
    int r;

    p[0] = arg1[0];
    p[1] = arg1[1];
    p[2] = arg1[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_70003600, arg0 + 0xD0, D_70003600);
    r = func_001B1630(p[0], p[1], p[2]);
    arg0[1] = r;
    if (r & 0xFF) {
        goto hit;
    }

    p[0] = -arg1[0];
    p[1] = -arg1[1];
    p[2] = -arg1[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_70003600, arg0 + 0xD0, D_70003600);
    r = func_001B1630(p[0], p[1], p[2]);
    arg0[1] = r;
    if (r & 0xFF) {
        goto hit;
    }

    p[0] = arg2[0];
    p[1] = arg2[1];
    p[2] = arg2[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_70003600, arg0 + 0xD0, D_70003600);
    r = func_001B1630(p[0], p[1], p[2]);
    arg0[1] = r;
    if (r & 0xFF) {
        goto hit;
    }

    p[0] = -arg2[0];
    p[1] = -arg2[1];
    p[2] = -arg2[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_70003600, arg0 + 0xD0, D_70003600);
    r = func_001B1630(p[0], p[1], p[2]);
    arg0[1] = r;
    if (!(r & 0xFF)) {
        return 0;
    }

hit:
    func_001B1B70(arg0);
    return 1;
}
