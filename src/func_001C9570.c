// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Builds a scale-rotate-translate matrix in place: identity (func_001029C0),
// rotate by the Euler angles at rot (func_00102C58: z, then y, then x), then on VU0
// scale rows 0, 1 and 2 (xyz only) by scale.x, scale.y and scale.z (the libvu0
// scale-matrix pattern, inlined), and finally translate by trans
// (func_00102918(m, m, trans)).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): one VU0 macro
// instruction per asm statement; everything else is C.
typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; } Mtx4;

extern void func_001029C0(Mtx4 *m);
extern void func_00102C58(Mtx4 *dst, Mtx4 *src, Vec4 *rot);
extern void func_00102918(Mtx4 *dst, Mtx4 *src, Vec4 *trans);

void func_001C9570(Mtx4 *m, Vec4 *trans, Vec4 *rot, Vec4 *scale) {
    func_001029C0(m);
    func_00102C58(m, m, rot);
    asm("lqc2 vf4, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf5, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf6, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf7, 0x30(%0)" : : "r"(m));
    asm("lqc2 vf8, 0x0(%0)" : : "r"(scale));
    asm("vmulx.xyz vf4, vf4, vf8x");
    asm("vmuly.xyz vf5, vf5, vf8y");
    asm("vmulz.xyz vf6, vf6, vf8z");
    asm("sqc2 vf4, 0x0(%0)" : : "r"(m) : "memory");
    asm("sqc2 vf5, 0x10(%0)" : : "r"(m) : "memory");
    asm("sqc2 vf6, 0x20(%0)" : : "r"(m) : "memory");
    asm("sqc2 vf7, 0x30(%0)" : : "r"(m) : "memory");
    func_00102918(m, m, trans);
}
