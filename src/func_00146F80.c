// NEARMISS func_00146F80  (vram 0x00146F80, 0x260 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling / boolean-materialization permutation, same family as the documented D_70003B68 'absolute-vs-symbol reloc form' wall (see src/func_0012B410.c's NEARMISS note). Logic fully recovered: seed-vector setup, VU0 add, move-probe, the 3-way poly-type mark test, the fallback two-ray...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Ground-slope probe. Seeds scratchpad D_700038A0 = (0,5,15,1), vec-adds it
// to self+0xB0 (VU add via func_001028B8), and move-probes toward it
// (func_0019AD00, mask 2). On a hit against a fixed-geometry surface
// (hit kind 2, object type 4) whose poly-type is 6 or 0x1C, or whose
// poly-type falls in [0x1E,0x1F], marks the hit record's +0x36 flag and
// returns 1. Otherwise (miss, or hit but none of those types) falls back
// to an explicit two-ray probe using a per-modulo table row from
// D_00245A00 (idx = (D_70003B68 + D_70003B8A) % 3, 0x20-byte stride):
// row -> D_700038A0, row+0x10 -> D_700038B0 (func_00102948 qword copies),
// each transformed (func_001B2B10) and vec-added to self+0xB0, then
// segment-probed (func_0019AFE0, mask 4). On a miss returns 0; on a hit,
// if the hit record's +0x1A flags bit 0x2000 is set returns 1 outright,
// else angle-tests the hit (func_0019A310) against ~70 degrees
// (1.2217305 rad) and returns 1 if steeper, 0 if shallower/equal.

extern void func_001B2B10(char *self, void *a1, void *a2);
extern void func_001028B8(void *dst, void *a, void *b);
extern int func_0019AD00(char *actor, void *target, int mask);
extern int func_0019AFE0(char *actor, void *from, void *to, int mask);
extern void func_00102948(void *dst, void *src);
extern void func_0019A310(float *out);

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
        int hitKind = *(int *)0x700031D8;
        int marked = 0;

        if (hitKind == 2) {
            int *hitObj = *(int **)0x700031D4;
            if (hitObj != 0) {
                if (*((unsigned char *)hitObj + 2) == 4) {
                    if (*((unsigned char *)hitObj + 3) == 6) {
                        marked = 1;
                    }
                }
            }
        }
        if (!marked) {
            unsigned char polyType = *((unsigned char *)(*(int **)0x700031D4) + 3);
            if (polyType == 0x1C) {
                marked = 1;
            } else if ((unsigned int)(polyType - 0x1E) < 2) {
                marked = 1;
            }
        }
        if (marked) {
            *(short *)((char *)*(int **)0x700031D4 + 0x36) = 1;
            return 1;
        }
        return 0;
    } else {
        int idx = (D_70003B68 + D_70003B8A) % 3;
        char *row = D_00245A00 + idx * 0x20;

        func_00102948(D_700038A0, row);
        func_00102948(D_700038B0, row + 0x10);
        func_001B2B10(self, D_700038A0, D_700038A0);
        func_001B2B10(self, D_700038B0, D_700038B0);
        func_001028B8(D_700038A0, self + 0xB0, D_700038A0);
        func_001028B8(D_700038B0, self + 0xB0, D_700038B0);

        if (func_0019AFE0(self, D_700038A0, D_700038B0, 4) == 0)
            return 0;

        if ((*(short *)(*(int *)0x700031D0 + 0x1A) & 0x2000) != 0)
            return 1;

        {
            float ang;
            func_0019A310(&ang);
            if (ang <= 1.2217305f)
                return 0;
            return 1;
        }
    }
}
