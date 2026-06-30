// NEARMISS func_001B1630  (vram 0x001B1630, 0x16C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP instruction-scheduling / register-coloring permutation of the three independent anchor-delta loads/subs/stores at the top: mwcc 2.3.3 reorders the constant loads (D_008105D0/D4/D8) and reuses FP regs differently from CW, which keeps all three deltas live and stores them in source order. Tail c...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS (96.9% mwcc 2.3.3 / 81.8% 991202): in-cone visibility / range test.
// Writes the delta of the caller's (x,y,z) against an anchor (D_008105D0..D8)
// into the EE scratchpad workspace at 0x70003600 (with +0xC = 1.0f), takes the
// distance via func_0011E748(sumsq) (sqrt-like). If distance > 350 -> 0. Else
// orients the scratchpad vector (func_00102760) and dots it against a reference
// direction D_00810600 (func_00102738 -> d): closer than 35 always passes;
// 35..45 passes when d>=0; beyond 45 passes only when d>=0.7. Logic + the whole
// classification tail match 100%; sole residual is mwcc's FP scheduling /
// register coloring of the three independent anchor-delta loads/subs/stores
// (it reorders the constant loads vs CW). Pure scheduling permutation ->
// permuter territory. Note the func_001B15D0 idiom: the calls pass the symbol
// D_70003600 while the float accesses use the raw literal 0x70003600.
extern float func_0011E748(float);
extern void func_00102760(float *, float *);
extern float func_00102738(float *, float *);
extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern float D_00810600;
extern float D_70003600[4];

int func_001B1630(float fx, float fy, float fz) {
    float *p = (float *)0x70003600;
    float d;
    float r;
    float a, b, c;
    a = fx - D_008105D0;
    b = fy - D_008105D4;
    c = fz - D_008105D8;
    p[0] = a;
    p[1] = b;
    p[2] = c;
    *(int *)0x7000360C = 0x3F800000;
    r = func_0011E748((p[0] * p[0]) + (p[1] * p[1]) + (p[2] * p[2]));
    if (!(r <= 350.0f)) {
        return 0;
    }
    func_00102760(D_70003600, D_70003600);
    d = func_00102738(D_70003600, &D_00810600);
    if (r < 35.0f) {
        return 1;
    }
    if (r < 45.0f) {
        if (d < 0.0f) {
            return 0;
        }
    } else if (d < 0.7f) {
        return 0;
    }
    return 1;
}
