// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0
// SPAD: 0x700031D4
// SPAD: 0x700031D8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.29% NEARMISS).
// Fixes read from the original instructions:
// - func_001A6AD0 takes one argument (its definition in src/), not three.
// - result is cleared before the copy loop, and the restore loop walks a
//   pointer q = tmp (kept in a saved register) instead of indexing tmp[i].
// - Each value from a1 is loaded once into a float temp, then stored to both
//   tmp[] and the block.
// - Local declaration order (result, flags, q first; i before p) fixes the
//   saved/temporary register assignment.
// - The scratchpad words 0x700031D0 / 0x700031D4 / 0x700031D8 are volatile
//   relocated externs (idiom-32); that keeps their stores out of branch
//   delay slots, as in the original.

// Hardware/effect-block setup at 0x70003190. Copies 3 floats from a0 into the
// block (+0) and 3 floats from a1 into both a local scratch (tmp[0..2]) and the
// block (+0x10); writes 1.0 into three slots (0x700031AC/0x7000319C/tmp[3]) and
// clears 0x700031D4. Then runs up to three optional subsystem inits gated by
// mode bits 1/2/4 (func_001A6AD0 / func_001A1390 / func_0019D770), recording the
// highest-priority success in result (1, 2 or 4). On bit-1 success it mirrors
// block +0x20 -> +0x10. If any init succeeded it restores tmp[] into block +0x10,
// else clears the object pointer at 0x700031D0. Stores result to 0x700031D8 and
// returns it.
extern int func_001A6AD0(int);
extern int func_001A1390(void);
extern int func_0019D770(void);
extern char D_70003190[];
extern char * volatile D_700031D0;
extern volatile int D_700031D4;
extern volatile int D_700031D8;

int func_0019A910(float *a0, float *a1, int mode) {
    int result;
    int flags;
    float *q;
    float tmp[4];
    int i;
    char *p;
    float f;

    p = D_70003190;
    q = tmp;
    result = 0;
    for (i = 0; i < 3; i++) {
        *(float *)p = *a0;
        a0++;
        f = *a1;
        tmp[i] = f;
        *(float *)(p + 0x10) = f;
        p += 4;
        a1++;
    }
    *(float *)0x700031AC = 1.0f;
    *(float *)0x7000319C = 1.0f;
    flags = mode & 0xFF;
    tmp[3] = 1.0f;
    D_700031D4 = 0;
    if (flags & 1) {
        if (func_001A6AD0(0x40) != 0) {
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
            *(float *)(p + 0x10) = *q;
            q++;
            p += 4;
        }
    } else {
        D_700031D0 = 0;
    }
    D_700031D8 = result;
    return result;
}
