// NEARMISS func_00143AF0  (vram 0x00143AF0, 0x544 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.94% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order artifact: mwcc assigns arg1->s0 / arg0->s1 (opposite of the natural arg0-first order), pinning a differently-numbered callee-saved register pair through the whole function; cascades into ARG_MISMATCH on most branches/calls using those regs plus a v0/v1 + FP-load scheduli...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Enemy melee-boarding/grapple state machine dispatched on the state byte at
// arg0[6] (arg0 = actor, arg1 = per-actor extra state block). State 0 clears the
// hit-flag at arg0[0x5E], resets arg1's timers (0x20/0x24/0x40/0x4C/0x75) and kicks
// clip 0xA. State 1 arms a "close" flag (0x24) once within 35 units and, once the
// 0x30 flag bit 0x1000 (anim-finished) is set, advances to state 2 and kicks clip
// 0xB. State 2 arms the same flag at 8 units, sets a 0.1f/0 damage-multiplier at
// arg1+0x4C by a distance-band check on arg1+0x5C, and once within 8 units (and not
// already grappled) checks func_0021BE40 (LOS?) + func_001A7B80 (grapple-eligible?)
// to lock the grab (sets D_008102B0 bit 1, arg1+0x20), picks a struggle-speed const
// by arg0[0xD]&0x80 and the difficulty flag D_0081070A, and seeds the world-space
// grab offset (D_00810320/0x328) from the player's position (D_00810360/0x368)
// relative to arg0; once the 0x1000 flag fires again, advances to state 3 / clip
// 0xC. State 3 repeats the same grapple-lock logic but at a flat 8-unit gate, then
// on the finish flag advances to state 4 / clip 0xE (no grab-state reset here --
// intentional, matches target). State 4, on finish flag, resets the actor to its
// idle state (arg0[5]=arg0[6]=0) and rerolls arg1's random look-offset (0x71) from
// func_00122BB8. Tail (every state): re-samples the head-tracking pitch via
// func_001B1240(arg0->pos, player_x, player_z) into arg1+0x44, and eases arg0's
// aim-pitch (arg0+0xC4) toward it via func_001B12B0.
//
// WALL: register-allocation-order artifact. mwcc assigns arg1->s0 / arg0->s1
// (opposite of the natural arg0-first order this source implies), which this
// source's statement order does not reproduce; that pins one differently-numbered
// callee-saved register pair through the whole function and cascades into
// ARG_MISMATCH on nearly every branch/call using those regs, even though every
// idiom fix landed (branch-sense inversion on the 0x4C multiplier store, and fixing
// func_001FBD50's real 4-arg (self,a,b,f) signature, get this to 91.9% on mwcc233;
// tried touching arg1 first / re-ordering the two entry stores / alternate
// -sdatathreshold, none flip the s0/s1 assignment). The sole remaining residual
// besides the s0/s1 swap is a v0/v1-register + FP-load scheduling variant in the
// D_008104D4 speed-const select and the D_00810320/D_00810368 delta computation
// (target loads D_00810368 before storing D_00810320's result; ours stores first).
// Not the clean-store nop, not fixable by a source-level idiom found so far --
// parked as a genuine register-coloring near-miss. Best measured: mwcc233 91.9%,
// 991202 78.5%.

extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_00102760(void *a0, void *a1);
extern int func_00122BB8(void);
extern int func_001A7B80(char *a0);
extern float func_001B1240(char *a0, float a1, float a2);
extern float func_001B12B0(float a0, float a1, float a2);
extern void func_001B55E0(char *a0, int a1);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(void *a0, char *a1);
extern unsigned char D_008102B0;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern int D_008104D4;
extern float D_00810360;
extern float D_00810368;
extern unsigned char D_0081070A;

void func_00143AF0(char *arg0, char *arg1) {
    int v0;
    unsigned char st;

    *(char *)(arg0 + 0x5E) = 0;
    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x20) = 0;
        *(int *)(arg1 + 0x24) = 0;
        *(char *)(arg1 + 0x75) = 0;
        *(int *)(arg1 + 0x40) = 0x40133333;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init(arg0, 0xA, 5.0f, 0.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0x24) == 0 && *(float *)(arg0 + 0x3C) <= 35.0f) {
            *(int *)(arg1 + 0x24) = 1;
            func_001FBD50(arg0, 0x835, 0, 300.0f);
        }
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(int *)(arg1 + 0x24) = 0;
            *(float *)(arg1 + 0x40) = 1.0f;
            anim_clip_init(arg0, 0xB, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x24) == 0 && *(float *)(arg0 + 0x3C) <= 8.0f) {
            *(int *)(arg1 + 0x24) = 1;
            func_001FBD50(arg0, 0x836, 0, 300.0f);
        }
        if (!(*(float *)(arg1 + 0x5C) <= 18.0f)) {
            *(float *)(arg1 + 0x4C) = 0.1f;
        } else {
            *(int *)(arg1 + 0x4C) = 0;
        }
        if (*(float *)(arg0 + 0x3C) <= 8.0f && *(int *)(arg1 + 0x20) == 0 &&
            func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            *(int *)(arg1 + 0x20) = 1;
            D_008102B0 |= 2;
            if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
                v0 = 0x41A00000;
                if (D_0081070A != 0) {
                    v0 = 0x41C80000;
                }
            } else {
                v0 = 0x41700000;
                if (D_0081070A != 0) {
                    v0 = 0x41A00000;
                }
            }
            D_008104D4 = v0;
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760(&D_00810320, &D_00810320);
            func_001B55E0(arg0, 0);
        }
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(int *)(arg1 + 0x20) = 0;
            *(int *)(arg1 + 0x24) = 0;
            *(int *)(arg1 + 0x4C) = 0;
            anim_clip_init(arg0, 0xC, 0.0f, 0.0f);
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0x24) == 0 && *(float *)(arg0 + 0x3C) <= 8.0f) {
            *(int *)(arg1 + 0x24) = 1;
            func_001FBD50(arg0, 0x836, 0, 300.0f);
        }
        if (*(float *)(arg0 + 0x3C) <= 8.0f && *(int *)(arg1 + 0x20) == 0 &&
            func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            *(int *)(arg1 + 0x20) = 1;
            D_008102B0 |= 2;
            if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
                v0 = 0x41A00000;
                if (D_0081070A != 0) {
                    v0 = 0x41C80000;
                }
            } else {
                v0 = 0x41700000;
                if (D_0081070A != 0) {
                    v0 = 0x41A00000;
                }
            }
            D_008104D4 = v0;
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760(&D_00810320, &D_00810320);
            func_001B55E0(arg0, 0);
        }
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init(arg0, 0xE, 0.0f, 0.0f);
        }
        break;
    case 4:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x70) = 0;
            *(char *)(arg1 + 0x71) = (func_00122BB8() >> 0x13) & 0x1F;
        }
        break;
    }
    {
        float f = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
        *(float *)(arg1 + 0x44) = f;
        *(float *)(arg0 + 0xC4) = func_001B12B0(f, *(float *)(arg0 + 0xC4), 0.06981317f);
    }
}
