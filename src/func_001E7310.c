// NEARMISS func_001E7310  (vram 0x001E7310, 0x128 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register coloring: entire float block uniformly shifted ~2 FP registers (target uses f1/f2/f4, ours f3/f5/f6) + func_001DEE80 a0/a1 arg-setup order. mwcc FP-allocator/scheduling artifact, not the clean-store nop. Permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.11% (mwcc 2.3.3). Logic fully recovered. Entity sub-state machine
// dispatched on the state byte self[4]. case 0: init floats +0x60=0.5, +0x64=64.0,
// kick func_001D2830(9,1) + func_001DEE80(9,&D_0026E970), bump state to 1, fall
// through. case 1: per-frame update -- func_001DEEC0(9, float_to_int(+0x64)),
// func_001DF5A0(0.2*+0x60), decay +0x64 *= 0.98, and ease +0x60 toward 0 via
// +0x60 += (0.05*-+0x60 - 0.015); when +0x60 <= 0 advance state to 3. case 2/3:
// tear down func_001D2830(9,0) + func_001AFC10(self). The case-0 call reuses the
// dispatch literal 1 as func_001D2830's 2nd arg (proven: 93.8->99.1). Sole residual
// is FP register coloring -- the whole float block is uniformly shifted by ~2 FP
// registers (target f1/f2/f4 vs ours f3/f5/f6) plus the func_001DEE80 arg-setup
// order; both are mwcc FP-allocator/scheduling artifacts (permuter territory),
// NOT the clean-store delay-slot nop. Body byte-identical otherwise.
extern int float_to_int(float);
extern void func_001D2830(int, int);
extern void func_001DEE80(int, int);
extern void func_001DEEC0(int, int);
extern void func_001DF5A0(float);
extern void func_001AFC10(unsigned char *);
extern unsigned char D_0026E970;

void func_001E7310(unsigned char *self) {
    switch (self[4]) {
    case 0:
        self[4] = 1;
        *(float *)(self + 0x60) = 0.5f;
        *(float *)(self + 0x64) = 64.0f;
        func_001D2830(9, 1);
        func_001DEE80(9, (int)&D_0026E970);
        /* fallthrough */
    case 1:
        func_001DEEC0(9, float_to_int(*(float *)(self + 0x64)));
        func_001DF5A0(0.2f * *(float *)(self + 0x60));
        *(float *)(self + 0x64) = *(float *)(self + 0x64) * 0.98f;
        {
            float x = *(float *)(self + 0x60);
            *(float *)(self + 0x60) = x + (0.05f * -x - 0.015f);
            if (*(float *)(self + 0x60) <= 0.0f) {
                self[4] = 3;
            }
        }
        break;
    case 2:
    case 3:
        func_001D2830(9, 0);
        func_001AFC10(self);
        break;
    }
}
