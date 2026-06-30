// NEARMISS func_0019B6C0  (vram 0x0019B6C0, 0x104 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation ordering in the two 3-element copy loops: the target assigns the loop counter to $v1 and the stack-scratch pointer (the saved $s1 copy) to $a3, but mwcc swaps them (counter->$a3, pointer->$v1). The 0x40 frame with the scratch base held in callee-saved $s1 across the func_001A2...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 81.8% (mwcc 2.3.3; 81.5% on pinned 991202). Body/structure byte-correct
// including the 0x40 frame with the scratch base held in callee-saved $s1 across
// both calls; sole residual is register-allocation ordering (counter vs scratch
// pointer mapped to swapped registers in the copy loops) plus minor store
// scheduling. Permuter/regalloc territory.
//
// Snapshots a 3-component source pair (arg0[] interleaved into the global block
// D_70003190[+0] and arg1[] into both a stack scratch buffer and D_70003190[+0x10]),
// then resets the global state words (0x70003254=0, 0x700031AC/0x7000319C=1.0f,
// stk[3]=1.0f, 0x700031D4=0). Probes two subsystems: func_001A2AE0() -> result 2,
// func_0019DF10() -> result 4 (the later win overrides). If either fired (result
// nonzero) it commits the saved arg1 snapshot into D_70003190[+0x10..]; otherwise
// it clears 0x700031D0. Stores the result code to 0x700031D8 and returns it.
extern int func_001A2AE0();
extern int func_0019DF10();
extern float D_70003190;

int func_0019B6C0(float *arg0, float *arg1) {
    float stk[4];
    float *base;
    float *p;
    float *dst;
    int i;
    int r;

    base = stk;
    dst = &D_70003190;
    r = 0;
    i = 0;
    p = base;
    do {
        i++;
        dst[0] = *arg0;
        *p = *arg1;
        dst[4] = *arg1;
        arg0++;
        dst++;
        arg1++;
        p++;
    } while (i < 3);
    *(int *)0x70003254 = 0;
    *(int *)0x700031AC = 0x3F800000;
    *(int *)0x7000319C = 0x3F800000;
    stk[3] = 1.0f;
    *(int *)0x700031D4 = 0;
    if (func_001A2AE0() != 0) {
        r = 2;
    }
    if (func_0019DF10() != 0) {
        r = 4;
    }
    if (r != 0) {
        float *d2 = &D_70003190;
        int j = 0;
        do {
            j++;
            d2[4] = *base;
            base++;
            d2++;
        } while (j < 3);
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = r;
    return r;
}
