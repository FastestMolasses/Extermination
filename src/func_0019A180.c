// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031E0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.83% NEARMISS). Fixes read from the original instructions:
// - Not an FP-colouring wall: the if/else arms were swapped. The original tests
//   v >= 0 first with the non-negative arm in the fall-through, writes the upper
//   test as v > 1.7320508 (0x2000 first) and the negative outer test as
//   v > -0.70020753 (-0x8000 first).
// - The active count at 0x700031E0 is a relocated scratchpad extern (idiom-32),
//   so its upper half is no longer speculated into the first guard's delay slot.
//
// Angle/octant classifier. For entity index arg1 (arg0 must be 0):
//   - bounds-check arg1 < D_700031E0 (active count), else return 0.
//   - if D_70003170[arg1] (u16 flag table) has 0x8000 set:
//       read float v = D_00282250[arg1]; classify v into a signed octant bitmask
//       (0x4000/0x2000/0x1000/0x800/-0x8000) by sign and the thresholds
//       +-0.70020753 (~cos45) and +-1.7320508 (sqrt 3 ~= tan60); OR it with the
//       u8 field at +0x54 of the struct pointed to by D_70003130[arg1].
//   - else return the s16 field at +0x1A of that struct.

extern float D_00282250[];
extern int D_70003130[];
extern unsigned short D_70003170[];
extern int D_700031E0;

short func_0019A180(int arg0, int arg1) {
    float v;
    short bits;

    if (arg0 != 0) {
        return 0;
    }
    if (arg1 < D_700031E0) {
        if (D_70003170[arg1] & 0x8000) {
            v = D_00282250[arg1];
            if (v >= 0.0f) {
                if (v < 0.70020753f) {
                    bits = 0x4000;
                } else if (v > 1.7320508f) {
                    bits = 0x2000;
                } else {
                    bits = 0x1000;
                }
            } else if (v > -0.70020753f) {
                bits = -0x8000;
            } else if (v < -1.7320508f) {
                bits = 0x2000;
            } else {
                bits = 0x800;
            }
            return bits | *(unsigned char *)((char *)D_70003130[arg1] + 0x54);
        }
        return *(short *)((char *)D_70003130[arg1] + 0x1A);
    }
    return 0;
}
