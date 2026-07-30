// NEARMISS func_001A9C40  (vram 0x001A9C40, 0xE0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Dispatch (lui/addiu %hi/%lo jtbl_0026DB30, sll, addu, lw, jr) byte-matches. Residual = 4 instructions of FP register COLORING only: target `sub.s $f3,$f3,$f2` (dy) / `sub.s $f2,$f1,$f0` (dz) / `mul.s $f0,$f3,$f3` / `madd.s $f12,$f2,$f2`; mwcc233 emits dy->$f2 and dz->$f3 (`sub.s $f2,$f3,$f2`, `su...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
// The jump-table dispatch itself now byte-matches. The only residual is FP
// register COLORING: the target puts dy in $f3 and dz in $f2, mwcc 2.3.3 puts
// dy in $f2 and dz in $f3 (4 instructions: the two sub.s dests, the 2nd mul.s
// operand and the madd.s operand). The instruction sequence, load order and
// summation order are otherwise identical.

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
