// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0 0x700031D8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 83.41% NEARMISS).
// Fixes read from the original instructions:
//   - Not a register-colouring wall: func_001A1B80 takes no arguments (its
//     definition in src/ is (void)); the old body passed three pointers and had
//     no prototype for either callee. Both callees now have (void) prototypes.
//   - Rewritten in the spelling of the byte-matched sibling func_0019B7D0:
//     indexed for-loops over D_70003190 and a local vec4, with 0x700031D0,
//     0x700031D4, 0x700031D8 and 0x70003254 as relocated scratchpad externs.
//
// Collision query front end (sibling of func_0019B7D0 and func_0019A570):
// copies the start vector a0[0..2] to D_70003190[0..2] and the end vector
// a1[0..2] to D_70003190[4..6] (keeping a local copy of the end), sets both
// w components (0x7000319C, 0x700031AC) and the local's w to 1.0f and clears
// 0x700031D4. Mode bits in the low byte of arg2:
//   - bit 1: clear 0x70003254, run func_001A1B80; nonzero -> ret = 2.
//   - bit 2: run func_0019DB50; nonzero -> ret = 4.
// On a hit the end vector is restored from the local copy; on a miss the
// active descriptor 0x700031D0 is cleared. Stores ret at 0x700031D8 and
// returns it.

extern int D_700031D0;
extern int D_700031D4;
extern int D_700031D8;
extern int D_70003254;
extern float D_70003190[];
extern int func_001A1B80(void);
extern int func_0019DB50(void);

int func_0019A440(float *a0, float *a1, int arg2) {
    float local[4];
    int i;
    int ret = 0;
    int m;

    for (i = 0; i < 3; i++) {
        float t;
        D_70003190[i] = a0[i];
        t = a1[i];
        local[i] = t;
        D_70003190[i + 4] = t;
    }

    *(float *)0x700031AC = 1.0f;
    *(float *)0x7000319C = 1.0f;
    m = arg2 & 0xFF;
    local[3] = 1.0f;
    D_700031D4 = 0;

    if (m & 2) {
        D_70003254 = 0;
        if (func_001A1B80()) {
            ret = 2;
        }
    }
    if ((m & 4) && func_0019DB50()) {
        ret = 4;
    }

    if (ret != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[i + 4] = local[i];
        }
    } else {
        D_700031D0 = 0;
    }

    D_700031D8 = ret;
    return ret;
}
