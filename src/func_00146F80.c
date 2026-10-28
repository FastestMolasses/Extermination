// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D4 0x700031D8

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 87.65% NEARMISS).
// Fixes read from the original instructions:
// - FIDELITY: a hit from the first move-probe always returns 1; the poly-type tests
//   only decide whether the hit record's +0x36 flag is set (the old C returned 0 for
//   a hit on any other surface). The type tests are one || chain.
// - FIDELITY: the angle threshold is pi/4 (0.7853982, 45 degrees), not 1.2217305,
//   and the angle output is preset to pi/2 before the flag test, so the callee may
//   leave it untouched.
// - the hit word 0x700031D8 and hit-object pointer 0x700031D4 are scratchpad externs
//   (idiom-32, volatile); the poly type is read into an int.
// - the two-ray tail is one `if (probe hit) { ... }` followed by a shared `return 0`.

// Ground-slope probe. Seeds scratchpad D_700038A0 = (0,5,15,1), transforms it
// (func_001B2B10), vec-adds self+0xB0 (func_001028B8) and move-probes toward it
// (func_0019AD00, mask 2). On a hit it returns 1, first marking the hit record's
// +0x36 flag when the hit is a fixed-geometry surface (hit kind 2, object type 4)
// with poly-type 6, or the poly-type is 0x1C or in [0x1E,0x1F]. On a miss it runs
// an explicit two-ray probe using a per-modulo table row from D_00245A00
// (idx = (D_70003B68 + D_70003B8A) % 3, 0x20-byte stride): row -> D_700038A0,
// row+0x10 -> D_700038B0 (func_00102948 qword copies), each transformed and
// vec-added to self+0xB0, then segment-probed (func_0019AFE0, mask 4). A miss
// returns 0; on a hit, the hit record's +0x1A flag bit 0x2000 returns 1 outright,
// else func_0019A310 measures the hit's angle (preset to pi/2) and the probe
// returns 1 when it is steeper than pi/4 (45 degrees), else 0.

extern void func_001B2B10(char *self, void *a1, void *a2);
extern void func_001028B8(void *dst, void *a, void *b);
extern int func_0019AD00(char *actor, void *target, int mask);
extern int func_0019AFE0(char *actor, void *from, void *to, int mask);
extern void func_00102948(void *dst, void *src);
extern void func_0019A310(float *out);

extern volatile int D_700031D8;
extern unsigned char *volatile D_700031D4;
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_00245A00[];
extern int D_70003B68;
extern short D_70003B8A;

int func_00146F80(char *self) {
    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 5.0f;
    *(float *)0x700038A8 = 15.0f;
    *(float *)0x700038AC = 1.0f;

    func_001B2B10(self, D_700038A0, D_700038A0);
    func_001028B8(D_700038A0, self + 0xB0, D_700038A0);

    if (func_0019AD00(self, D_700038A0, 2) != 0) {
        /* hit: always succeeds; the special surfaces also get their flag */
        unsigned char *hitObj;
        int polyType;

        if ((D_700031D8 == 2 && (hitObj = D_700031D4) != 0 &&
             hitObj[2] == 4 && hitObj[3] == 6) ||
            (polyType = D_700031D4[3]) == 0x1C ||
            (unsigned int)(polyType - 0x1E) < 2) {
            *(short *)(D_700031D4 + 0x36) = 1;
        }
        return 1;
    } else {
        int idx = (D_70003B68 + D_70003B8A) % 3;
        char *row = D_00245A00 + idx * 0x20;
        float ang;

        func_00102948(D_700038A0, row);
        func_00102948(D_700038B0, row + 0x10);
        func_001B2B10(self, D_700038A0, D_700038A0);
        func_001B2B10(self, D_700038B0, D_700038B0);
        func_001028B8(D_700038A0, self + 0xB0, D_700038A0);
        func_001028B8(D_700038B0, self + 0xB0, D_700038B0);

        if (func_0019AFE0(self, D_700038A0, D_700038B0, 4) != 0) {
            ang = 1.5707964f;
            if ((*(short *)(*(int *)0x700031D0 + 0x1A) & 0x2000) != 0)
                return 1;

            func_0019A310(&ang);
            if (ang > 0.7853982f)
                return 1;
        }
        return 0;
    }
}
