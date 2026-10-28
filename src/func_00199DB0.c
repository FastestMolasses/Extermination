// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.55% NEARMISS).
// Fixes read from the original instructions:
//   - The "guard branch-sense wall" was the source shape: the original nests the
//     tests (`if (p != 0) { if (p == lock-on) { if (sub && mode == 2) ... } }`)
//     like the byte-matched twin func_00199FA0, with the 0xFF / zero-offset tests
//     merged into one `||` guard, not a chain of early returns.
//   - The poly-record arm indexes the vertex table as a 12-byte vec3 array
//     (re-reading the table pointer and the active descriptor per axis), as in
//     the twin; that also fixes the 2.0 divisor's register.
//
// Writes the midpoint ("center") of the active target into the caller's vec at
// arg0+0,4,8. Returns 1 on success, 0 if no valid target. The active descriptor
// pointer is *0x700031D0:
//   - null                     -> 0
//   - == &D_700030B0 (lock-on) -> needs sub = *0x700031D4 non-null and
//        *0x700031D8 == 2; idx = (u16 sub[0xE]) >> 8 must not be 0xFF; the record
//        table *0x70003250 gives off = table[idx*4 + 4] (non-zero); the entry at
//        table + off holds two vec3s (lo at +0, hi at +0xC); center = (lo + hi) / 2.
//   - otherwise (a poly/face record): three pairs of s16 vertex indices at
//        +0/+2, +4/+6, +8/+0xA into the vec3 table *0x700031FC; each output axis
//        is the average of that axis of the two indexed vertices.
struct V3 { float x; float y; float z; };

extern char D_700030B0;

int func_00199DB0(char *arg0) {
    int p;
    char *a2;
    int t;
    int base;
    int off;
    float *v;

    p = *(int *)0x700031D0;
    if (p != 0) {
        if (p == (int)&D_700030B0) {
            a2 = *(char **)0x700031D4;
            if (a2 != 0 && *(int *)0x700031D8 == 2) {
                t = (int)*(unsigned short *)(a2 + 0xE) >> 8;
                if (t == 0xFF || (off = *(int *)((base = *(int *)0x70003250) + t * 4 + 4)) == 0) {
                    return 0;
                }
                v = (float *)(base + off);
                *(float *)(arg0 + 0) = (v[0] + v[3]) / 2.0f;
                *(float *)(arg0 + 4) = (v[1] + v[4]) / 2.0f;
                *(float *)(arg0 + 8) = (v[2] + v[5]) / 2.0f;
                return 1;
            }
            return 0;
        }
        *(float *)(arg0 + 0) = ((*(struct V3 **)0x700031FC)[*(short *)(p + 0)].x
                              + (*(struct V3 **)0x700031FC)[*(short *)(p + 2)].x) / 2.0f;
        *(float *)(arg0 + 4) = ((*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 4)].y
                              + (*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 6)].y) / 2.0f;
        *(float *)(arg0 + 8) = ((*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 8)].z
                              + (*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 0xA)].z) / 2.0f;
        return 1;
    }
    return 0;
}
