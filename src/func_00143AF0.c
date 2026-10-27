// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.94% NEARMISS).
// Fixes read from the original instructions:
// - the struggle-speed select is four separate stores of a float constant
//   (25/20 when arg0[0xD] bit 0x80 is set, 20/15 otherwise, by D_0081070A),
//   not one int temp stored once; D_008104D4 is a float.
// - the grab-offset staging block D_00810320/324/328 and the player position
//   D_00810360/368 are volatile externs (as in the byte-matched twin
//   func_001352E0), so D_00810368 is loaded after D_00810320 is stored.
// - the old "arg1->s0 / arg0->s1 register-order wall" was not a wall: the
//   original assigns them that way from this source once the body is right.
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
extern volatile float D_00810320;
extern volatile int D_00810324;
extern volatile float D_00810328;
extern float D_008104D4;
extern volatile float D_00810360;
extern volatile float D_00810368;
extern unsigned char D_0081070A;

void func_00143AF0(char *arg0, char *arg1) {
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
                if (D_0081070A != 0) {
                    D_008104D4 = 25.0f;
                } else {
                    D_008104D4 = 20.0f;
                }
            } else {
                if (D_0081070A != 0) {
                    D_008104D4 = 20.0f;
                } else {
                    D_008104D4 = 15.0f;
                }
            }
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760((float *)&D_00810320, (float *)&D_00810320);
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
                if (D_0081070A != 0) {
                    D_008104D4 = 25.0f;
                } else {
                    D_008104D4 = 20.0f;
                }
            } else {
                if (D_0081070A != 0) {
                    D_008104D4 = 20.0f;
                } else {
                    D_008104D4 = 15.0f;
                }
            }
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760((float *)&D_00810320, (float *)&D_00810320);
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
