// NEARMISS func_001FE920  (vram 0x001FE920, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.74% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Calling-convention/regalloc permutation: control flow, 0x20 frame with s0 save, and switch dispatch are byte-identical. Residuals: switch discriminant colored a0 vs target a2, and the func_00114988 call materializes a2/a3/t0=0 while leaving a0/a1 as live leftovers (target emits 3 arg-zeroing padd...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 92.7% (mwcc233). State machine on the gp-rel global D_00275C5C:
// clamp the state to -1 when it is non-negative, then dispatch:
//   state == -1 -> advance to state -2 and call func_00114988 (returns -1)
//   state == -2 -> reset D_00275C58 and D_00275C5C to 0, return 0
//   otherwise   -> return -1
// The default/-1 result (-1) lives in callee-saved s0 across the call. Control
// flow, the 0x20 frame with the s0 save, and the switch dispatch all match
// byte-for-byte. Residuals are compiler artifacts: the discriminant is colored
// a0 vs target's a2, and the func_00114988 call zeroes a2/a3/t0 while leaving
// a0/a1 as live register leftovers (target = 3 arg paddubs, mwcc = 5); neither
// is expressible from straight C. Permuter (regalloc/arg-setup).
extern int D_00275C58;
extern int D_00275C5C;
extern void func_00114988(int a0, int a1, int a2, int a3, int t0);

int func_001FE920(void)
{
    int s0;

    s0 = -1;
    if (D_00275C5C >= 0) {
        D_00275C5C = s0;
    }
    switch (D_00275C5C) {
    case -1:
        D_00275C5C = -2;
        func_00114988(0, 0, 0, 0, 0);
        break;
    case -2:
        D_00275C58 = 0;
        D_00275C5C = 0;
        s0 = 0;
        break;
    }
    return s0;
}
