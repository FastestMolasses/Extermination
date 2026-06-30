// NEARMISS func_001F1DD0  (vram 0x001F1DD0, 0x190 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling near-miss (permuter territory). Body and control flow byte-identical except 3 commutative-operand/scheduling artifacts: (1) the D_00275B40 gp-relative load is scheduled one issue-slot later than the target (mwcc placed it after the 0x70003A24 store instead of right ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (97.69% mwcc233; body byte-identical, residual = addu operand-order
// canonicalization + one gp-load schedule slot -- register-alloc/scheduling wall).
//
// Per-frame animation/oscillation tick. arg0+0xD selects a damping constant written
// to scratchpad 0x70003A20 (0.6 for state 1, 0.4 for state 0). arg1 is a {row,frame}
// counter pair: frame (arg1+4) advances 0..9 wrapping, and on wrap row (arg1+0)
// advances 0..5 wrapping. 0x70003A24 gets a random phase in [-pi, pi] from
// func_00122BB8()/2^31 scaled by 2*pi. base = D_00275B40[row]; the current frame slot
// base + frame*0x10 is initialized (func_00102948), then nudged in x/y by
// damping * sin/cos(phase) (func_0011E2A8 / func_0011DE90), and its +0xA0 word cleared.
extern void func_00102948(int, int);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern int func_00122BB8(void);
extern int D_00275B40;

void func_001F1DD0(char *arg0, char *arg1, int arg2) {
    int base;
    int st;
    float t;

    st = *(unsigned char *)(arg0 + 0xD);
    switch (st) {
    case 0:
        *(float *)0x70003A20 = 0.4f;
        break;
    case 1:
        *(float *)0x70003A20 = 0.6f;
        break;
    }

    *(int *)(arg1 + 4) = *(int *)(arg1 + 4) + 1;
    if (*(int *)(arg1 + 4) >= 0xA) {
        *(int *)(arg1 + 4) = 0;
        *(int *)(arg1 + 0) = *(int *)(arg1 + 0) + 1;
        if (*(int *)(arg1 + 0) >= 6) {
            *(int *)(arg1 + 0) = 0;
        }
    }

    *(float *)0x70003A24 = (6.2831855f * ((float) func_00122BB8() / 2147483648.0f)) - 3.1415927f;

    base = *(int *)(D_00275B40 + (*(int *)(arg1 + 0) * 4));
    func_00102948(base + (*(int *)(arg1 + 4) * 0x10), arg2);

    t = *(float *)0x70003A20 * func_0011E2A8(*(float *)0x70003A24);
    *(float *)(base + (*(int *)(arg1 + 4) * 0x10)) += t;

    t = *(float *)0x70003A20 * func_0011DE90(*(float *)0x70003A24);
    *(float *)((*(int *)(arg1 + 4) * 0x10) + base + 4) += t;

    *(int *)((*(int *)(arg1 + 4) * 4) + base + 0xA0) = 0;
}
