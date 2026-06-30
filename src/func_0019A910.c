// NEARMISS func_0019A910  (vram 0x0019A910, 0x16C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.29% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring + delay-slot scheduling. Body/structure fully recovered and verified. Target colors result->s0, flags->s1, tmp-base->s2 (all callee-saved across the 3 calls); both mwcc builds pick a different reg assignment for the same SSA values and reorder a few delay slots (no cl...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Hardware/effect-block setup at 0x70003190. Copies 3 floats from a0 into the
// block (+0) and 3 floats from a1 into both a local scratch (tmp[0..2]) and the
// block (+0x10); writes 1.0 into three slots (0x700031AC/0x7000319C/tmp[3]) and
// clears 0x700031D4. Then runs up to three optional subsystem inits gated by
// mode bits 1/2/4 (func_001A6AD0 / func_001A1390 / func_0019D770), recording the
// highest-priority success in result (1, 2 or 4). On bit-1 success it mirrors
// block +0x20 -> +0x10. If any init succeeded it restores tmp[] into block +0x10,
// else clears 0x700031D0. Stores result to 0x700031D8 and returns it.
extern int func_001A6AD0(int, float *, void *);
extern int func_001A1390(void);
extern int func_0019D770(void);

extern char D_70003190[];

int func_0019A910(float *a0, float *a1, int mode) {
    float tmp[4];
    char *p;
    int i;
    int flags;
    int result;

    p = D_70003190;
    for (i = 0; i < 3; i++) {
        *(float *)p = *a0;
        a0++;
        tmp[i] = *a1;
        *(float *)(p + 0x10) = *a1;
        p += 4;
        a1++;
    }
    *(float *)0x700031AC = 1.0f;
    *(float *)0x7000319C = 1.0f;
    flags = mode & 0xFF;
    tmp[3] = 1.0f;
    *(int *)0x700031D4 = 0;
    result = 0;
    if (flags & 1) {
        if (func_001A6AD0(0x40, a1, p) != 0) {
            p = D_70003190;
            for (i = 0; i < 3; i++) {
                *(float *)(p + 0x10) = *(float *)(p + 0x20);
                p += 4;
            }
            result = 1;
        }
    }
    if (flags & 2) {
        *(int *)0x70003254 = 0;
        if (func_001A1390() != 0) {
            result = 2;
        }
    }
    if ((flags & 4) && func_0019D770() != 0) {
        result = 4;
    }
    if (result != 0) {
        p = D_70003190;
        for (i = 0; i < 3; i++) {
            *(float *)(p + 0x10) = tmp[i];
            p += 4;
        }
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = result;
    return result;
}
