// NEARMISS func_00198D90  (vram 0x00198D90, 0x174 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP add.s operand coloring (+3.0f: target f1+f0 vs mwcc f0+f1) and case-0 branch-likely speculative-slot duplication (target duplicates addiu v0,a0,1 into the beqzl slot, mwcc emits once). Both regalloc/scheduling artifacts, not the clean-store nop. 98.8% on 2.3.3.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 98.8% (mwcc 2.3.3; 991202 reaches 94.5%). Per-state transform/animation
// step for an entity. Builds a transform from the matrix at arg0+0x10 and the
// source at arg1+0xA0, advances the rotation field at arg0+0x14 by 3.0f, rebuilds
// the working matrix block at globals D_70003600.. (zeroing two words and storing
// the 5.0f bit-pattern 0x40A00000), composes it via func_001026A0/func_001028B8,
// then dispatches on the state byte at arg0+1: state 0 bumps the state, clears
// arg0+2, and re-seeds the two matrices from D_008105E0/D_008105D0; state 1 (when
// arg1+0x230 == 0x12) latches arg0+6=0xB and resets the state, then nudges the
// matrices toward D_008105E0 by 0.2f (func_0018C4B0/func_0018C6A0).
//
// Body and structure are byte-correct. The two residual deltas are pure compiler
// artifacts: (1) add.s operand coloring on the +3.0f (target `add.s f0,f1,f0` vs
// mwcc `add.s f0,f0,f1` -- commutative, mwcc canonicalizes the register order
// regardless of source operand order), and (2) the case-0 branch-likely slot:
// the target duplicates `addiu v0,a0,1` into the beqzl delay slot AND at the
// fallthrough, which mwcc emits only once. Both are regalloc/scheduling class.
extern void func_001026A0(char *p, float *m, int *v);
extern void func_001028B8(char *a, char *b, char *c);
extern void func_00102948(float *m, char *v);
extern void func_001029C0(float *m);
extern void func_00102C58(float *a, float *b, char *c);
extern void func_0018C4B0(float *m, float a, float b);
extern void func_0018C6A0(char *p, float *m, float a);
extern float D_008105D0;
extern float D_008105E0;
extern float D_70003400;
extern int D_70003600;

void func_00198D90(char *arg0, char *arg1) {
    unsigned char st;

    func_00102948((float *)(arg0 + 0x10), arg1 + 0xA0);
    *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x14) + 3.0f;
    func_001029C0(&D_70003400);
    func_00102C58(&D_70003400, &D_70003400, arg1 + 0xC0);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(int *)0x70003608 = 0x40A00000;
    *(int *)0x7000360C = 0;
    func_001026A0(arg0 + 0x20, &D_70003400, &D_70003600);
    func_001028B8(arg0 + 0x20, arg0 + 0x20, arg0 + 0x10);
    st = *(unsigned char *)(arg0 + 1);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 1) = st + 1;
        *(char *)(arg0 + 2) = 0;
        func_00102948(&D_008105E0, arg0 + 0x20);
        func_00102948(&D_008105D0, arg0 + 0x10);
        return;
    case 1:
        if (*(int *)(arg1 + 0x230) == 0x12) {
            *(char *)(arg0 + 6) = 0xB;
            *(unsigned char *)(arg0 + 1) = 0;
        }
        func_0018C4B0(&D_008105E0, *(float *)(arg0 + 0x24), 0.2f);
        func_0018C6A0(arg0 + 0x20, &D_008105E0, 0.2f);
        func_00102948(&D_008105D0, arg0 + 0x10);
        return;
    }
}
