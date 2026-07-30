// NEARMISS func_001A9C40  (vram 0x001A9C40, 0xE0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.02% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table external-dispatch wall (proven s84) — local @14 vs external jtbl_0026DB30 on the dispatch lw. PLUS one residual: FP register allocation for dy/dz is swapped ($f2<->$f3) vs the target, costing 4 instructions (the two sub.s dests, the second mul.s operand, the madd.s operand). 9 variants t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: proximity/awareness test between `self` and `other`.
// Skipped entirely when bit 1 of other[0] is set. Otherwise the 3D distance
// between the two position vectors (floats at +0xB0/+0xB4/+0xB8) is measured
// with func_0011E748 (sqrtf), and compared against a per-kind alert radius
// selected from other[3] via the 8-entry jr-table at 0x0026DB30:
//   kinds 0,4        -> 15.0f
//   kinds 1,5,6,7    -> 20.0f
//   kinds 2,3, >=8   -> 0.0f (radius 0 == "never alerts", early out)
// Within radius, bit 0 of other[0xA] is set (the "noticed/alerted" flag).
//
// NEARMISS 99.0% — jr-table external-dispatch wall (proven s84): the original
// consolidated all jump tables into an external rodata TU, so mwcc's local
// @14 table is a permanent reloc mismatch. Additional residual: the dy/dz
// sub.s destinations are allocated $f2/$f3 swapped relative to the target
// (4 instructions); logic and instruction sequence are otherwise identical.

extern float func_0011E748(float x);

void func_001A9C40(char *self, char *other) {
    float dist;
    float radius;

    if (*(unsigned char *)other & 2) {
        return;
    }
    {
        float dx = *(float *)(self + 0xB0) - *(float *)(other + 0xB0);
        float dy = *(float *)(self + 0xB4) - *(float *)(other + 0xB4);
        float dz = *(float *)(self + 0xB8) - *(float *)(other + 0xB8);
        dist = func_0011E748(((dx * dx) + (dy * dy)) + (dz * dz));
    }
    radius = 0.0f;
    switch (*(unsigned char *)(other + 3)) {
    case 0:
    case 4:
        radius = 15.0f;
        break;
    case 1:
    case 5:
    case 6:
    case 7:
        radius = 20.0f;
        break;
    }
    if ((radius != 0.0f) && (dist <= radius)) {
        *(unsigned char *)(other + 0xA) |= 1;
    }
}
