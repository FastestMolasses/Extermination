// NEARMISS func_0014AE80  (vram 0x0014AE80, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.54% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring permutation: target carries the decayed timer t in $f0 and freshly-loaded operands in $f1; mwcc colors them swapped (add.s at +0xB4 and the 0x4C+0x50 add load order). Body/control-flow/call-ABI/float-const all match. Permuter class.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 99.54% (mwcc 2.3.3; 991202 reaches 98.09%). Logic fully recovered.
// Per-frame update on an entity (arg0) driven by a sub-object (arg1). If the
// low-7 bits of the flag byte at arg1+0x80 are set, bail. Otherwise decay the
// timer at arg1+0x48 by 0.1 and fold it into arg0+0xB4. If the byte at arg1+0x8C
// is nonzero, stop. Then: if the 0x80 flag is clear, the value at arg1+0x68 is
// <= 10.0, and func_001B1560(self,&D_00810360,1.13446f) succeeds, OR the entity
// into +0x52 and stop. Otherwise advance arg1+0x4C by arg1+0x50 and accumulate
// scaled cos/sin (func_0011E2A8/func_0011DE90 of the angle at arg0+0xC4) into the
// position fields arg0+0xB0 / arg0+0xB8.
//
// WALL: sole residual is FP register coloring -- the target carries the decayed
// timer `t` in $f0 and freshly-loaded memory operands in $f1, while mwcc colors
// them swapped (insns: add.s at +0xB4, and the 0x4C+0x50 add load order). Body,
// control flow, call ABI and the 1.13446f float constant all match. This is a
// coherent FP-coloring permutation across the function -> permuter class.
extern int func_001B1560(char *a0, char *a1, float f);
extern float func_0011E2A8(float);
extern float func_0011DE90(float);
extern char D_00810360;

void func_0014AE80(char *arg0, char *arg1) {
    float t;

    if (*(char *)(arg1 + 0x80) & 0x7F) {
        return;
    }
    t = *(float *)(arg1 + 0x48) + -0.1f;
    *(float *)(arg1 + 0x48) = t;
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + t;
    if (*(char *)(arg1 + 0x8C) != 0) {
        return;
    }
    if (!(*(char *)(arg1 + 0x80) & 0x80) && *(float *)(arg1 + 0x68) <= 10.0f
            && func_001B1560(arg0, &D_00810360, 1.1344640254974365f) != 0) {
        *(unsigned short *)(arg0 + 0x52) = *(unsigned short *)(arg0 + 0x52) | 1;
        return;
    }
    *(float *)(arg1 + 0x4C) = *(float *)(arg1 + 0x4C) + *(float *)(arg1 + 0x50);
    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0)
            + *(float *)(arg1 + 0x4C) * func_0011E2A8(*(float *)(arg0 + 0xC4));
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8)
            + *(float *)(arg1 + 0x4C) * func_0011DE90(*(float *)(arg0 + 0xC4));
}
