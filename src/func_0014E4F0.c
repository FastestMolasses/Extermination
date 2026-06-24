// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state animation/clip driver, dispatched on the state byte at arg0+6
// (a 3-way switch 0/1/2). State 0: bumps the state, zeroes arg1+0x28, sets the
// 1.0f field at arg1+0x20 and the 0x2710 short at arg1+0x56, flags the global
// D_00810803 = 0x80, and kicks clip 2 via anim_clip_init(self, 2, 5.0f, 0.0f).
// State 1: re-arms the 0x2710 timer, and only once D_00810803 has reached 0x81
// advances the state, starts clip 0x18, marks the linked object at arg0+0x20
// (field +4 = 3) if present, records func_001EFE00(0x80000046, self) at
// arg0+0x24, then calls func_001FBD50(self, 0x868, 0, 300.0f). State 2: when the
// two globals D_0028A9A2/D_0028A9A0 say the sequence is done (!=0 and ==2), sets
// arg0+4 = 3 and propagates 3 into the linked object at arg0+0x24.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 90.2%. Two keys: (1) anim_clip_init's real signature is
// (int self, int clip, float a, float b) -- the trailing 0.0f is what emits the
// `mtc1 zero, $f13` pair-clear the target has and a single-float prototype omits;
// (2) writing the dispatch as a `switch` (not an if/else-if cascade) reproduces
// the target's 2->1->0 compare order with the case-0 branch-likely (beqzl).
// objdiff 100.0 vs expected (the only rendered deltas are objdiff naming the
// same $f13 odd-half register `fa0f` vs `fa1`; identical encoded bytes).
extern void anim_clip_init(int self, int clip, float a, float b);
extern char *func_001EFE00(int a, char *p);
extern void func_001FBD50(char *p, int a, int b, float f);
extern short D_0028A9A0;
extern char D_0028A9A2;
extern unsigned char D_00810803;

void func_0014E4F0(char *arg0, char *arg1) {
    unsigned char st;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x28) = 0;
        *(float *)(arg1 + 0x20) = 1.0f;
        *(short *)(arg1 + 0x56) = 0x2710;
        D_00810803 = 0x80;
        anim_clip_init((int)arg0, 2, 5.0f, 0.0f);
        break;
    case 1:
        *(short *)(arg1 + 0x56) = 0x2710;
        if (D_00810803 == 0x81) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init((int)arg0, 0x18, 5.0f, 0.0f);
            p = *(char **)(arg0 + 0x20);
            if (p != 0) {
                *(char *)(p + 4) = 3;
            }
            *(char **)(arg0 + 0x24) = func_001EFE00(0x80000046, arg0);
            func_001FBD50(arg0, 0x868, 0, 300.0f);
        }
        break;
    case 2:
        if (D_0028A9A2 != 0 && D_0028A9A0 == 2) {
            *(char *)(arg0 + 4) = 3;
            p = *(char **)(arg0 + 0x24);
            if (p != 0) {
                *(char *)(p + 4) = 3;
            }
        }
        break;
    }
}
