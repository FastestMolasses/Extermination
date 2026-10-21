// NEARMISS func_0019BA80  (vram 0x0019BA80, 0x1B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.40% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Locals reordered (fixes the saved-register rotation). Left: mainly idiom-32 scratchpad lui delay-slot fills.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Decomp lane round 5 (2026-10-20): locals reordered by a declaration-order sweep
// against the target's saved registers.

// Loop1 broadcasts arg1[0..2] into a local float[3] and into the
// D_70003190 block (offsets 0/0x10); a bias of +/-0.001f (from sign of
// *(arg2+4)) is folded into the global at 0x70003194 around the func_001A3980 /
// func_0019E930 dispatch (gated by arg3 bits 2/4) which selects a result r in
// {0,2,4}; if r!=0 loop2 copies the saved locals back and computes a delta
// (0x20-0x10 -> 0x30), else clears 0x700031D0; result stored at 0x700031D8.
// The saved-register rotation recorded earlier is fixed by the declaration order;
// most of the rest is the scratchpad upper-half `lui` the target leaves out of branch
// delay slots (MATCHING_GUIDE idiom-32).
extern int func_001A3980();
extern int func_0019E930();
extern char D_70003190[];

int func_0019BA80(char *arg0, float *arg1, char *arg2, int arg3) {
    int i;
    char *d;
    float bias;
    int r;
    char *d2;
    float *q;
    float *t;
    float local[3];
    float *p;

    q = local;
    r = 0;
    p = arg1;
    t = q;
    d = D_70003190;
    i = 0;
    do {
        float v = *p;
        i += 1;
        *(float *)(d + 0x10) = v;
        *(float *)(d + 0x0) = v;
        *t = v;
        p += 1;
        d += 4;
        t += 1;
    } while (i < 3);

    *(float *)0x70003194 = *(float *)0x70003194 - *(float *)(arg2 + 4);
    if (*(float *)(arg2 + 4) < 0.0f) {
        bias = 0.001f;
    } else {
        bias = -0.001f;
    }
    *(float *)0x70003194 = *(float *)0x70003194 + bias;
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    *(int *)0x700031D4 = 0;
    *(short *)0x7000324E = *(unsigned char *)(arg0 + 2) & 0x1F;

    if (arg3 & 2) {
        *(int *)0x70003254 = *(int *)(arg0 + 0x14);
        if (func_001A3980() == 0) {
            r = 2;
        }
    }
    if ((arg3 & 4) && func_0019E930() == 0) {
        r = 4;
    }
    *(float *)0x70003194 = *(float *)0x70003194 - bias;

    if (r != 0) {
        d2 = D_70003190;
        i = 0;
        do {
            i += 1;
            *(float *)(d2 + 0x10) = *q;
            q += 1;
            *(float *)(d2 + 0x30) = *(float *)(d2 + 0x20) - *(float *)(d2 + 0x10);
            d2 += 4;
        } while (i < 3);
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = r;
    return r;
}
