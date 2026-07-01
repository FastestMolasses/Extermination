// NEARMISS func_001756E0  (vram 0x001756E0, 0x218 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.34% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body and control flow fully correct (branch-likely beql on func_001760C0 reproduced). Sole residual is a callee-saved register coloring swap: target holds 'old' (the pre-loop arg0[0x236] byte) in $s1 and the D_00248950 walk-pointer in $s2; mwcc assigns them $s2/$s1 respectively. Counter $s0 and s...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 94.3% (mwcc 2.3.3; pinned 991202 build 89.8%). Logic fully recovered.
//
// Per-entity 'wobble/settle' update, keyed on the global game-mode byte
// D_00810700. Early exit path: when mode==0x12 AND arg0[0xB4] >= 165.0f AND
// (arg0[0xA]&0x80) AND *(arg0[0x214]+3)==6, arm the pending flag arg0[0x236]=1,
// set bit 2 of arg0[0x235], optionally kick func_00174A50(self, 12.0f) when
// arg0[4]==1 && arg0[5]==0, and return 1.
// Otherwise, if a wobble was pending (old arg0[0x236]!=0): clear it, build a
// 4-word transform block on the stack (0, 4.01f, 4.0f, 1.0f), then for up to 7
// candidate offsets from D_00248950[] compose func_001029C0/00102BB0/00102918/
// 001026A0 into D_700036A0/38A0 and test placement via func_001760C0(self,...,
// 1, 13.99f); the first success re-sets arg0[0x236]=1 and stops. Finally fold
// the pending bit into arg0[0x235] (set bit1 / else mask to bit0) and, when
// arg0[4]==1 && arg0[5]==0 and the pending flag changed, call
// func_00174A50(self, 12.0f). Returns 0.
//
// WALL: body/structure correct; the only residual is a saved-register coloring
// swap -- the target keeps 'old' in $s1 and the D_00248950 walk-pointer in $s2,
// mwcc assigns them the other way ($s2/$s1); the counter ($s0) and self ($s3)
// already match. Live-range-driven; declaration reordering does not move it.
// Register-allocation-permutation class.
extern float func_001B1470(float);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, char *);
extern void func_001026A0(void *, void *, int *);
extern int func_001760C0(char *, void *, int, float);
extern void func_00174A50(char *, float);
extern float D_00248950;
extern unsigned char D_00810700;
extern int D_700036A0;
extern int D_700038A0;

int func_001756E0(char *arg0) {
    int sp50;
    int sp54;
    int sp58;
    int sp5C;
    unsigned char old;
    int i;
    float *p;

    if (D_00810700 == 0x12 && !(*(float *)(arg0 + 0xB4) < 165.0f) &&
        (*(unsigned char *)(arg0 + 0xA) & 0x80) &&
        *(unsigned char *)(*(char **)(arg0 + 0x214) + 3) == 6) {
        *(unsigned char *)(arg0 + 0x236) = 1;
        *(unsigned char *)(arg0 + 0x235) = *(unsigned char *)(arg0 + 0x235) | 2;
        if (*(unsigned char *)(arg0 + 4) == 1) {
            if (*(unsigned char *)(arg0 + 5) == 0) {
                func_00174A50(arg0, 12.0f);
            }
        }
        return 1;
    }
    old = *(unsigned char *)(arg0 + 0x236);
    if (old != 0) {
        *(unsigned char *)(arg0 + 0x236) = 0;
        sp50 = 0;
        sp54 = 0x408051EC;
        sp58 = 0x40800000;
        sp5C = 0x3F800000;
        p = &D_00248950;
        i = 0;
        do {
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + *p));
            func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
            func_001026A0(&D_700038A0, &D_700036A0, &sp50);
            if (func_001760C0(arg0, &D_700038A0, 1, 13.99f) != 0) {
                *(unsigned char *)(arg0 + 0x236) = 1;
                break;
            }
            i += 1;
            p += 1;
        } while (i < 7);
    }
    if (*(unsigned char *)(arg0 + 0x236) != 0) {
        *(unsigned char *)(arg0 + 0x235) = *(unsigned char *)(arg0 + 0x235) | 2;
    } else {
        *(unsigned char *)(arg0 + 0x235) = *(unsigned char *)(arg0 + 0x235) & 1;
    }
    if (*(unsigned char *)(arg0 + 4) == 1) {
        if (*(unsigned char *)(arg0 + 5) == 0 && old != *(unsigned char *)(arg0 + 0x236)) {
            func_00174A50(arg0, 12.0f);
        }
    }
    return 0;
}
