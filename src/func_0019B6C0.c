// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0 0x700031D4 0x700031D8

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.83% NEARMISS).
// Fixes read from the original instructions:
// - Rewritten in the spelling of its byte-matched twin func_0019B7D0: plain
//   indexed for-loops over D_70003190[] (a float array) and a local float[4],
//   with a1[i] read once into a temp; no hand-written pointer walkers.
// - The scratchpad words 0x700031D0 / 0x700031D4 / 0x700031D8 are relocated
//   externs (idiom-32, `// SPAD:`), which stops mwcc speculating their upper
//   half into delay slots the original leaves empty.
// - Both callees take no arguments (`(void)`, as their definitions in src/).
// - The 1.0f stack word is written before the 0x700031D4 clear.
//
// Snapshots two 3-component vectors: a0[] into D_70003190[0..2] and a1[] into
// both a local scratch buffer and D_70003190[4..6]. Then resets the state words
// (0x70003254 = 0, 0x700031AC / 0x7000319C = 1.0f, local[3] = 1.0f,
// 0x700031D4 = 0). Probes func_001A2AE0() -> result 2 and func_0019DF10() ->
// result 4 (the later hit overrides). If either fired it re-applies the saved
// a1 snapshot to D_70003190[4..6]; otherwise it clears 0x700031D0. Stores the
// result code to 0x700031D8 and returns it.

extern int D_700031D0;
extern int D_700031D4;
extern int D_700031D8;
extern float D_70003190[];
extern int func_001A2AE0(void);
extern int func_0019DF10(void);

int func_0019B6C0(float *a0, float *a1) {
    float local[4];
    int i;
    int ret = 0;

    for (i = 0; i < 3; i++) {
        float t;
        D_70003190[i] = a0[i];
        t = a1[i];
        local[i] = t;
        D_70003190[i + 4] = t;
    }

    *(int *)0x70003254 = 0;
    *(float *)0x700031AC = 1.0f;
    *(float *)0x7000319C = 1.0f;
    local[3] = 1.0f;
    D_700031D4 = 0;

    if (func_001A2AE0()) {
        ret = 2;
    }
    if (func_0019DF10()) {
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
