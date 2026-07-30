// NEARMISS func_00135D00  (vram 0x00135D00, 0x438 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.01% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.01% (mwcc233; mwcc 2.3.1 only reaches 83.7%). The jr-table dispatch is byte-identical INCLUDING the jtbl_0026D190 reloc, and all 9 case bodies, offsets, widths, constants and call targets match. 12 residual instructions of 290, in 3 clusters, all FP-argument register/order artifacts of CW's po...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: scripted actor/NPC behaviour state machine, one step per frame.
//   act = the actor block, ent = the entity/controller block driving it.
//   ent+0x52 (short) is reset to 100 on entry (the "alive/priority" counter).
//   act[6] = behaviour sub-state (0..8, 9-entry jump table). act[5] is the
//   outer state; both are cleared when the run is aborted.
//   ent+0x00 = input/event flag word; bit 0x1000 is the "animation finished"
//   advance signal that steps act[6] and starts the next clip; its low byte
//   is also compared against 5 in state 2.
//   act+0xB0..0xB8 = the actor's target position (340, 220, 270); act+0xC4 =
//   its yaw. ent+0x24 = the goal yaw, ent+0x2C / ent+0x30 = move speed and
//   acceleration, ent+0x04 = a countdown timer.
//   state 0: snap to the scripted position, face -2.2026055 rad, zero the
//            move speed/accel, play clip 0x14 (blend 5.0).
//   state 1: if act+0x2C has no 0x8000 flag and act+0x3C == 7.0, fire sound
//            cue 0x850 at 450.0; on the advance signal play clip 0x15.
//   state 2: on event byte 5, run func_001370C0(0x2A,3) + (0x26,3) and fire
//            cue 0x851; on the advance signal play clip 0x16.
//   state 3: on the advance signal set accel 0.05 and play clip 1.
//   state 4: steer -- goal yaw = bearing from act+0xB0 to (423.5, 235.0),
//            ease the current yaw toward it by pi/36 per frame, clamp speed
//            to 0.4 (zeroing accel when clamped); stage (423.0, 0, 235.0, 1)
//            and the actor position in the scratchpad and measure the
//            distance -- once it is <= 40.0 and the yaw has converged, zero
//            speed/accel and play clip 7.
//   state 5: on the advance signal play clip 9.
//   state 6: if act+0x3C == 70.0 fire cue 0x84E; on the advance signal play
//            clip 0xB.
//   state 7: on the advance signal arm the 0xB4-frame timer (ent+0x04) and
//            play clip 6 (blend 10.0).
//   state 8: tick the timer down; when it expires clear act[5]/act[6] and
//            ent+0x52.
//   Tail: if the global scene/phase byte D_00810805 is >= 4 the whole script
//   is torn down (act[5], act[6], ent+0x52, ent+0x30, ent+0x2C all cleared).
//
//   NOTE: the `volatile` read in state 0 is a MATCHING DEVICE only -- act[6]
//   is a plain byte; the qualifier reproduces CW's non-CSE'd re-load of
//   act[6] there (it removes 6 residual instructions and is semantically a
//   no-op for `act[6] = act[6] + 1`).

extern unsigned char D_00810805;
extern char D_700038A0[];
extern char D_700038B0[];

extern void anim_clip_init(unsigned char *act, int clip, float a, float b);
extern void func_001FBD50(unsigned char *act, int id, int flag, float a);
extern void func_001370C0(int a, int b);
extern float func_001B1240(unsigned char *pos, float px, float pz);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B15D0(char *a, char *b);
extern void func_00102948(char *dst, float *src);

void func_00135D00(unsigned char *act, unsigned char *ent)
{
    int st;
    int t;
    float d;

    *(short *)(ent + 0x52) = 100;
    st = act[6];
    switch (st) {
    case 0:
        act[6] = *(volatile unsigned char *)(act + 6) + 1;
        *(int *)(act + 0xB0) = 0x43AA0000;
        *(int *)(act + 0xB4) = 0x435C0000;
        *(int *)(act + 0xB8) = 0x43870000;
        *(int *)(act + 0xC4) = 0xC00CF77D;
        *(int *)(ent + 0x2C) = 0;
        *(int *)(ent + 0x30) = 0;
        anim_clip_init(act, 0x14, 5.0f, 0.0f);
        break;
    case 1:
        if (!(*(short *)(act + 0x2C) & 0x8000)) {
            if (*(float *)(act + 0x3C) == 7.0f) {
                func_001FBD50(act, 0x850, 0, 450.0f);
            }
        }
        if (*(int *)ent & 0x1000) {
            act[6] = act[6] + 1;
            anim_clip_init(act, 0x15, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (*(unsigned char *)ent == 5) {
            func_001370C0(0x2A, 3);
            func_001370C0(0x26, 3);
            func_001FBD50(act, 0x851, 0, 450.0f);
        }
        if (*(int *)ent & 0x1000) {
            act[6] = act[6] + 1;
            anim_clip_init(act, 0x16, 0.0f, 0.0f);
        }
        break;
    case 3:
        if (*(int *)ent & 0x1000) {
            act[6] = st + 1;
            *(float *)(ent + 0x30) = 0.05f;
            anim_clip_init(act, 1, 5.0f, 0.0f);
        }
        break;
    case 4:
        *(float *)(ent + 0x24) = func_001B1240(act + 0xB0, 423.5f, 235.0f);
        *(float *)(act + 0xC4) = func_001B12B0(*(float *)(ent + 0x24),
                                               *(float *)(act + 0xC4),
                                               0.0872664675f);
        if (!(*(float *)(ent + 0x2C) <= 0.4f)) {
            *(float *)(ent + 0x2C) = 0.4f;
            *(int *)(ent + 0x30) = 0;
        }
        *(volatile float *)0x700038A0 = 423.0f;
        *(volatile int *)0x700038A4 = 0;
        *(volatile float *)0x700038A8 = 235.0f;
        *(volatile float *)0x700038AC = 1.0f;
        func_00102948(D_700038B0, (float *)(act + 0xB0));
        *(volatile int *)0x700038B4 = 0;
        d = func_001B15D0(D_700038A0, D_700038B0);
        if (d <= 40.0f) {
            if (*(float *)(act + 0xC4) == *(float *)(ent + 0x24)) {
                act[6] = act[6] + 1;
                *(int *)(ent + 0x2C) = 0;
                *(int *)(ent + 0x30) = 0;
                anim_clip_init(act, 7, 5.0f, 0.0f);
            }
        }
        break;
    case 5:
        if (*(int *)ent & 0x1000) {
            act[6] = st + 1;
            anim_clip_init(act, 9, 0.0f, 0.0f);
        }
        break;
    case 6:
        if (*(float *)(act + 0x3C) == 70.0f) {
            func_001FBD50(act, 0x84E, 0, 450.0f);
        }
        if (*(int *)ent & 0x1000) {
            act[6] = act[6] + 1;
            anim_clip_init(act, 0xB, 0.0f, 0.0f);
        }
        break;
    case 7:
        if (*(int *)ent & 0x1000) {
            act[6] = st + 1;
            *(int *)(ent + 4) = 0xB4;
            anim_clip_init(act, 6, 10.0f, 0.0f);
        }
        break;
    case 8:
        t = *(int *)(ent + 4);
        if (t != 0) {
            *(int *)(ent + 4) = t - 1;
        } else {
            act[5] = 0;
            act[6] = 0;
            *(short *)(ent + 0x52) = 0;
        }
        break;
    }

    t = D_00810805;
    if (!(t < 4)) {
        act[5] = 0;
        act[6] = 0;
        *(short *)(ent + 0x52) = 0;
        *(int *)(ent + 0x30) = 0;
        *(int *)(ent + 0x2C) = 0;
    }
}
