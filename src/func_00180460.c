// NEARMISS func_00180460  (vram 0x00180460, 0xC8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register-coloring on the commutative add.s. Target loads *(float*)0x700038A4 into $f1 and the 4.0f constant into $f0 (add.s $f0,$f1,$f0); both mwcc builds swap the operand registers (mem->$f0, const->$f1) for the identical add.s $f0,$f1,$f0. Only 2 instructions differ and only in fp-register n...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Animation/clip tick. Seeds the shared transform scratch D_700038A0 from the
// actor's +0xB0 block (func_001031E0), bumps the scratch's +4 float by 4.0f, then
// runs func_001760C0(actor, &D_700038A0, 1, 18.0f). If that returns nonzero the
// tick is done -> return 3. Otherwise advance the clip (func_00180420), copy the
// actor's current pos floats (+0x290 -> 0x700038A0, +0x298 -> 0x700038A8), set the
// +4 scratch float to 18.0f + actor->+0xB4, and return func_00180300(actor,
// &D_700038A0, actor->+0xD byte). The three direct float fields are accessed at
// their absolute HW-scratch addresses 0x700038A0/A4/A8 (the target does NOT relocate
// them through the D_700038A0 symbol -- only the pointer args do).
//
// NEARMISS 99.8% vs mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 = 91.4%. Body is
// byte-identical except one commutative add.s: target colors the memory operand
// $f1 and the 4.0f constant $f0; mwcc swaps the two source fp-registers (same
// encoded add.s $f0,$f1,$f0 destination, swapped sources). Genuine FP-register-
// coloring artifact -- not the clean-store delay-slot nop, so 2.3.3 cannot close it.
extern void func_001031E0(void *a, void *b);
extern int func_001760C0(void *a, void *b, int c, float f);
extern void func_00180420(char *p);
extern int func_00180300(char *a, char *b, unsigned char c);
extern float D_700038A0[];

int func_00180460(char *arg0) {
    func_001031E0(D_700038A0, arg0 + 0xB0);
    *(float *)0x700038A4 = *(float *)0x700038A4 + 4.0f;
    if (func_001760C0(arg0, D_700038A0, 1, 18.0f) == 0) {
        func_00180420(arg0);
        *(float *)0x700038A0 = *(float *)(arg0 + 0x290);
        *(float *)0x700038A8 = *(float *)(arg0 + 0x298);
        *(float *)0x700038A4 = 18.0f + *(float *)(arg0 + 0xB4);
        return func_00180300(arg0, (char *)D_700038A0, *(unsigned char *)(arg0 + 0xD));
    }
    return 3;
}
