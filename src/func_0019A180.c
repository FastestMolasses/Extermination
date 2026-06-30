// NEARMISS func_0019A180  (vram 0x0019A180, 0x18C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register-coloring permutation: target colors v into f1 + each const into f0 (c.lt.s/c.le.s f1,f0 natural sense); mwcc colors v into f0 + const into f1, emitting inverted c.le.s/c.lt.s with flipped bc1t/bc1f, which also reschedules the per-arm dsll32/dsra32 sign-extend and the top beqz delay sl...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (best 86.8% on mwcc 2.3.3; 73.3% on 991202). Logic fully recovered.
// Semantics: angle/octant classifier. For entity index arg1 (arg0 must be 0):
//   - bounds-check arg1 < *(int*)0x700031E0 (active count), else return 0.
//   - if D_70003170[arg1] (u16 flag table) has 0x8000 set:
//       read float v = D_00282250[arg1]; classify v into a signed octant bitmask
//       (0x4000/0x2000/0x1000/0x800/-0x8000) by sign and the thresholds
//       +-0.70020753 (~cos45) and +-1.7320508 (sqrt 3 ~= tan60); OR it with the
//       u8 field at +0x54 of the struct pointed to by D_70003130[arg1].
//   - else return the s16 field at +0x1A of that struct.
// WALL: not the clean-store nop. Residual is an FP-register-coloring permutation:
//   target keeps v in f1 and loads each constant into f0 (c.lt.s/c.le.s f1,f0,
//   natural branch sense), whereas mwcc colors v into f0 and the constant into f1,
//   emitting the inverted compare (c.le.s where source has '<') with flipped
//   bc1t/bc1f. This also shifts the per-arm dsll32/dsra32 sign-extend scheduling
//   and the top beqz delay-slot fill. Body/structure correct; FP-coloring +
//   scheduling artifact -> permuter territory.

extern float D_00282250[];
extern int D_70003130[];
extern unsigned short D_70003170[];

short func_0019A180(int arg0, int arg1) {
    float v;
    short bits;

    if (arg0 != 0) {
        return 0;
    }
    if (arg1 < *(int *)0x700031E0) {
        if (D_70003170[arg1] & 0x8000) {
            v = D_00282250[arg1];
            if (v < 0.0f) {
                if (v <= -0.70020753f) {
                    if (v < -1.7320508f) {
                        bits = 0x2000;
                    } else {
                        bits = 0x800;
                    }
                } else {
                    bits = -0x8000;
                }
            } else if (v < 0.70020753f) {
                bits = 0x4000;
            } else if (v <= 1.7320508f) {
                bits = 0x1000;
            } else {
                bits = 0x2000;
            }
            return bits | *(unsigned char *)((char *)D_70003130[arg1] + 0x54);
        }
        return *(short *)((char *)D_70003130[arg1] + 0x1A);
    }
    return 0;
}
