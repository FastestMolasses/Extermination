// NEARMISS func_00197390  (vram 0x00197390, 0xF4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling / delay-slot-fill: (1) the func_00102948 call sets its two args in the opposite emission order (target a0=&D_008105D0 first, mwcc a1=self+0x10 first) -- arg-eval-order permutation; (2) the trailing 3-way beq cascade on *(int*)(other+0x230) -- target packs the NEXT `addiu v1...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Spawn/place helper. Calls func_001916C0(self, other, 0), then copies three
// floats from per-type tables indexed by the global type byte D_00810702: row
// stride is 12 bytes (idx*3 words), D_0024A6C8[idx*3] -> self+0x10,
// D_0024A6CC[idx*3] -> self+0x14, D_0024A6D0[idx*3] -> self+0x18 (an x/y/z or
// offset vector). Calls func_0018D7B0(self, 5), then
// func_00102948(&D_008105D0, &self+0x10) (apply the vector via a matrix/xform).
// Finally, unless other's state at +0x230 is one of 9 / 8 / 0x2C, it clears the
// bytes at self+6, self+1, self+2.
//
// NEARMISS 96.3% (mwcc 2.3.3; 991202 = 93.2%). Logic fully recovered. The two
// residuals are scheduling artifacts: (1) func_00102948's two args are emitted
// in the opposite order (arg-eval-order permutation), and (2) the trailing
// 3-way beq cascade on *(int*)(other+0x230) -- the target packs each next
// `addiu v1,N` compare-constant into the preceding beq's delay slot, while
// mwcc leaves nops in the slots. Both are CW-vs-mwcc delay-slot/scheduling
// differences (permuter class), not a logic gap.
extern void func_001916C0(char *self, char *other, int c);
extern float D_0024A6C8[];
extern float D_0024A6CC[];
extern float D_0024A6D0[];
extern unsigned char D_00810702;
extern void func_0018D7B0(char *self, int n);
extern void func_00102948(char *p, float *q);
extern char D_008105D0;

void func_00197390(char *arg0, char *arg1) {
    char *self = arg0;
    char *other = arg1;

    func_001916C0(self, other, 0);
    *(float *)(self + 0x10) = D_0024A6C8[D_00810702 * 3];
    *(float *)(self + 0x14) = D_0024A6CC[D_00810702 * 3];
    *(float *)(self + 0x18) = D_0024A6D0[D_00810702 * 3];
    func_0018D7B0(self, 5);
    func_00102948(&D_008105D0, (float *)(self + 0x10));
    {
        int s = *(int *)(other + 0x230);
        if (s != 9 && s != 8 && s != 0x2C) {
            *(char *)(self + 6) = 0;
            *(char *)(self + 1) = 0;
            *(char *)(self + 2) = 0;
        }
    }
}
