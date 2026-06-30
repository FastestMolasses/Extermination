// NEARMISS func_001E7440  (vram 0x001E7440, 0x128 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register coloring: same uniform ~2 FP-register shift as func_001E7310 (target f1/f2/f4 vs ours f3/f5/f6) + func_001DEE80 a0/a1 arg-setup order. mwcc FP-allocator/scheduling artifact, not the clean-store nop. Permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.11% (mwcc 2.3.3). Structural twin of func_001E7310, different tuning
// constants. Entity sub-state machine on state byte self[4]. case 0: init +0x60=1.5,
// +0x64=128.0, func_001D2830(9,1) + func_001DEE80(9,&D_0026E980), bump to 1, fall
// through. case 1: func_001DEEC0(9, float_to_int(+0x64)), func_001DF5A0(0.2*+0x60),
// +0x64 *= 0.98, ease +0x60 via +0x60 += (0.05*-+0x60 - 0.145); when +0x60 <= 0 set
// state 3. case 2/3: func_001D2830(9,0) + func_001AFC10(self). Sole residual is the
// same uniform FP register-coloring shift + func_001DEE80 arg order as func_001E7310;
// mwcc FP-allocator/scheduling artifact (permuter), not the clean-store nop.
extern int float_to_int(float);
extern void func_001D2830(int, int);
extern void func_001DEE80(int, int);
extern void func_001DEEC0(int, int);
extern void func_001DF5A0(float);
extern void func_001AFC10(unsigned char *);
extern unsigned char D_0026E980;

void func_001E7440(unsigned char *self) {
    switch (self[4]) {
    case 0:
        self[4] = 1;
        *(float *)(self + 0x60) = 1.5f;
        *(float *)(self + 0x64) = 128.0f;
        func_001D2830(9, 1);
        func_001DEE80(9, (int)&D_0026E980);
        /* fallthrough */
    case 1:
        func_001DEEC0(9, float_to_int(*(float *)(self + 0x64)));
        func_001DF5A0(0.2f * *(float *)(self + 0x60));
        *(float *)(self + 0x64) = *(float *)(self + 0x64) * 0.98f;
        {
            float x = *(float *)(self + 0x60);
            *(float *)(self + 0x60) = x + (0.05f * -x - 0.145f);
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
