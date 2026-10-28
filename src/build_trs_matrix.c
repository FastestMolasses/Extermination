// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 3.15% readable
// companion; replaces the asm body).
// Fixes read from the original instructions:
//  - The scale step is VU0 macro code inlined in the original: rows 0..3 of m
//    and the scale vector are loaded into VU0, rows 0..2 have their x, y, z
//    lanes multiplied by scale.x, scale.y and scale.z respectively (one
//    lane-broadcast multiply per row), and all four rows are stored back (row 3
//    unchanged). It is written as inline asm statements, one instruction each,
//    instead of nine FPU multiplies.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
//
// Builds a bone's local matrix m (4x4 floats, rows of four) from a
// translation, Euler rotation and scale: identity (func_001029C0), rotate
// about X, Y, then Z by rot[0..2] (func_00102B08, func_00102BB0,
// func_00102A60, each m = R * m), scale the first three rows' xyz by
// scale[0..2], then translate by pos (func_00102918).
extern void func_001029C0(float *m);
extern void func_00102B08(float *dst, float *src, float angle);
extern void func_00102BB0(float *dst, float *src, float angle);
extern void func_00102A60(float *dst, float *src, float angle);
extern void func_00102918(float *dst, float *src, float *v);

void build_trs_matrix(float *m, float *pos, float *rot, float *scale) {
    func_001029C0(m);
    func_00102B08(m, m, rot[0]);
    func_00102BB0(m, m, rot[1]);
    func_00102A60(m, m, rot[2]);

    /* VU0: rows 0..3 and the scale vector in, row i's xyz scaled by
       scale[i] for i = 0..2, all four rows back out */
    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf5, 0x10(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf6, 0x20(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf7, 0x30(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf8, 0x0(%0)" : : "r"(scale) : "memory");
    asm __volatile__("vmulx.xyz vf4, vf4, vf8x" : : : "memory");
    asm __volatile__("vmuly.xyz vf5, vf5, vf8y" : : : "memory");
    asm __volatile__("vmulz.xyz vf6, vf6, vf8z" : : : "memory");
    asm __volatile__("sqc2 vf4, 0x0(%0)" : : "r"(m) : "memory");
    asm __volatile__("sqc2 vf5, 0x10(%0)" : : "r"(m) : "memory");
    asm __volatile__("sqc2 vf6, 0x20(%0)" : : "r"(m) : "memory");
    asm __volatile__("sqc2 vf7, 0x30(%0)" : : "r"(m) : "memory");

    func_00102918(m, m, pos);
}
