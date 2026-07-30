// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-state animation/AI driver for actor `self`, dispatched on the state byte at
// self+6 and driving the owner/anim context `ctx`.  State 0: advances the state,
// plays the "ready" cue (func_001B1190 + event 0x7D8) and then either — when the
// owner flag at self+0xD bit1 is set — jumps straight to state 3, sets the global
// hint D_008107EA and kicks clip 4 or 5 depending on ctx+0x61, or otherwise kicks
// clip 0x29 / 0x2A / 0x2B (the 0x2A-vs-0x2B pick gated by bit 9 of func_00122BB8).
// State 1: plays the transition cue 0x7D5 when the current clip (self+0x2C masked)
// and pose angle (self+0x3C) form the (0x2A,34.0) or (0x2B,21.0) pair, then on the
// owner's 0x1000 input bit advances the state and — unless the owner flag is set —
// fires the unbind event 0x7D7 and releases the handler via func_001EFE00, marking
// self+4 = 3 if the release fails.  State 2: on the owner flag plus the two gate
// globals, marks self+4 = 3 and propagates it to the linked actor at self+0x24
// (clearing the link).  State 3: waits for D_008107EA == 2, reverts to state 1,
// resets ctx+0x34, rebinds via func_001EFE00 and kicks clip 0x2A / 0x2B.
//
// Two source shapes are load-bearing for the match:
//  * every early exit is written as `goto done` to a single trailing label.  That
//    shared-exit CFG is what makes mwcc 2.3.3 reproduce CodeWarrior's conservative
//    branch-delay-slot filling — the unfilled NOPs after the dispatch/guard branches
//    and the dead const re-materialisations sitting just before the branch-target
//    labels.  With plain `return;` statements mwcc speculates those ops into the
//    delay slots and drops the dead copies (95.4%).
//  * idiom-24 (`zi = 0; z = (float)zi;`) at the three anim_clip_init sites whose
//    trailing 0.0f argument must be emitted as `mtc1 zero,$f13` BEFORE `mtc1 $v0,$f12`;
//    each site needs its OWN int/float pair (sharing one pair loses the ordering).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern int func_00122BB8(void);
extern void func_001B1190(int a0);
extern char *func_001EFE00(int a0, char *a1);
extern void func_001FBD50(char *self, int a, int b, float f);
extern short D_0028A9A0;
extern char D_0028A9A2;
extern unsigned char D_008107EA;

void func_00131B10(char *self, char *ctx) {
    unsigned char st;
    int clip;
    char *link;
    int zi0;
    float z0;
    int zi2;
    float z2;
    int zi3;
    float z3;

    st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(self + 6) = st + 1;
        func_001B1190(*(unsigned char *)(self + 0x9A));
        func_001FBD50(self, 0x7D8, 0, 300.0f);
        if (*(unsigned char *)(self + 0xD) & 2) {
            *(unsigned char *)(self + 6) = 3;
            *(int *)(ctx + 0x34) = 0x3F800000;
            D_008107EA = 1;
            if (*(unsigned char *)(ctx + 0x61) != 0) {
                anim_clip_init(self, 4, 5.0f, 0.0f);
                goto done;
            }
            zi0 = 0;
            z0 = (float)zi0;
            anim_clip_init(self, 5, 5.0f, z0);
            goto done;
        }
        *(int *)(ctx + 0x34) = 0x3F800000;
        if (*(unsigned char *)(ctx + 0x61) != 0) {
            anim_clip_init(self, 0x29, 5.0f, 0.0f);
            goto done;
        }
        if ((func_00122BB8() >> 9) & 1) {
            anim_clip_init(self, 0x2A, 5.0f, 0.0f);
            goto done;
        }
        anim_clip_init(self, 0x2B, 5.0f, 0.0f);
        goto done;
    case 1:
        clip = *(short *)(self + 0x2C) & 0xFFFF7FFF;
        if (clip == 0x2A && *(float *)(self + 0x3C) == 34.0f) {
            func_001FBD50(self, 0x7D5, 0, 300.0f);
        } else if (clip == 0x2B && *(float *)(self + 0x3C) == 21.0f) {
            func_001FBD50(self, 0x7D5, 0, 300.0f);
        }
        if (*(unsigned short *)(ctx + 0x58) & 0x1000) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            if (!(*(unsigned char *)(self + 0xD) & 2)) {
                *(char *)(ctx + 0x6D) = 0;
                func_001FBD50(self, 0x7D7, 0, 300.0f);
                if (func_001EFE00(0x8000001E, self) == 0) {
                    *(char *)(self + 4) = 3;
                    goto done;
                }
            }
        }
        goto done;
    case 2:
        if ((*(unsigned char *)(self + 0xD) & 2) && D_0028A9A2 != 0 && D_0028A9A0 == 2) {
            *(char *)(self + 4) = 3;
            link = *(char **)(self + 0x24);
            if (link != 0) {
                *(char *)(link + 4) = 3;
                *(char **)(self + 0x24) = 0;
                goto done;
            }
        }
        break;
    case 3:
        if (D_008107EA == 2) {
            *(unsigned char *)(self + 6) = 1;
            *(int *)(ctx + 0x34) = 0x3EB33333;
            *(char **)(self + 0x24) = func_001EFE00(0x80000046, self);
            if ((func_00122BB8() >> 9) & 1) {
                zi2 = 0;
                z2 = (float)zi2;
                anim_clip_init(self, 0x2A, 5.0f, z2);
                goto done;
            }
            zi3 = 0;
            z3 = (float)zi3;
            anim_clip_init(self, 0x2B, 5.0f, z3);
        }
        break;
    }
done:
    ;
}
