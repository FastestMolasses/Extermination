// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: scripted actor/NPC behaviour state machine, one step per frame,
//   dispatched through a 6-entry jump table on the sub-state byte act[6].
//   act (arg0) = the actor block, ent (arg1) = the entity/controller block.
//   act+0x05 = outer state, act+0x06 = behaviour sub-state (0..5),
//   act+0x2C (short) = a flag word (bit 0x8000 = "suppressed"),
//   act+0x3C = the clip's current frame counter, act+0xB0/0xB8 = position x/z,
//   act+0xC4 = yaw. ent+0x00 = event/flag word (bit 0x1000 = "clip finished",
//   the advance signal), ent+0x04 = a latch, ent+0x24 = goal yaw,
//   ent+0x2C / ent+0x30 = move speed and acceleration, ent+0x50 (short) =
//   a respawn/idle countdown, ent+0x5D = the "active" byte.
//   state 0: latch active, zero speed/accel, play clip 7 (blend 5.0).
//   state 1: steer at 5 deg/frame; if act+0x2C lacks 0x8000 and the frame
//            counter is exactly 30.0, fire sound cue 0x84D at 300.0; on the
//            advance signal clear ent+0x04 and play clip 9.
//   state 2: at frame 70.0 fire cue 0x84E; between frames 20.0 and 62.0 ramp
//            the speed (clamp to 0.5, accel 0.05) else zero it; once the frame
//            counter is <= 49.0 and the two proximity checks pass, latch
//            ent+0x04, set flag bit 2 in D_008102B0, pick the camera distance
//            (30.0 or 25.0 depending on D_0081070A), stage the delta from the
//            listener position into D_00810320..328 and kick the two helpers.
//            On the advance signal play clip 0xB (if latched) or clip 8.
//   state 3: steer at 2.5 deg/frame with the same speed clamp; on the advance
//            signal reset the latch and speed, play clip 0xA and fire cue 0x84F.
//   state 4: same proximity/camera sequence as state 2 but at frame 37.0 and
//            with distances 35.0 / 30.0; on the advance signal play clip 0xC.
//   state 5: on the advance signal tear the script down (clear act+5, act+6,
//            ent+0x5D) and arm ent+0x50 with 0x3C + a 5-bit random value.
//   Tail (states 1 and 3 only): goal yaw = bearing from act+0xB0 to the
//   listener position, then ease act+0xC4 toward it by (pi * step) / 180.
//
//   NOTE: two MATCHING DEVICES, both semantic no-ops: the `volatile` read in
//   state 0 reproduces CW's non-CSE'd re-load of act[6]; the `(z, (z = 0.0f))`
//   argument pair on the clip-8 call reproduces CW's f13-then-f12 float-argument
//   register order in that one arm (see docs/fanout/MATCHING_GUIDE.md).

extern void anim_clip_init(int self, int clip, float a, float b);
extern float func_001B1240(int p, float a, float b);
extern float func_001B12B0(float a, float b, float c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_00122BB8(void);
extern int func_0021BE40(unsigned char *g, char *p);
extern int func_001A7B80(char *p);
extern void func_00102760(float *a, float *b);
extern void func_001B55E0(char *p, int a);

extern unsigned char D_008102B0;
extern unsigned char D_0081070A;
extern float D_008104D4;
extern volatile float D_00810320;
extern volatile int D_00810324;
extern volatile float D_00810328;
extern volatile float D_00810360;
extern volatile float D_00810368;

void func_001352E0(char *arg0, char *arg1) {
    float ang;
    int steer;
    float d;
    float z;

    ang = 0.0f;
    steer = 0;
    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(unsigned char *)(arg0 + 6) = *(volatile unsigned char *)(arg0 + 6) + 1;
        *(char *)(arg1 + 0x5D) = 1;
        *(int *)(arg1 + 0x2C) = 0;
        *(int *)(arg1 + 0x30) = 0;
        anim_clip_init((int)arg0, 7, 5.0f, 0.0f);
        break;
    case 1:
        steer = 1;
        ang = 5.0f;
        if (!(*(short *)(arg0 + 0x2C) & 0x8000) && *(float *)(arg0 + 0x3C) == 30.0f) {
            func_001FBD50(arg0, 0x84D, 0, 300.0f);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(int *)(arg1 + 4) = 0;
            anim_clip_init((int)arg0, 9, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (*(float *)(arg0 + 0x3C) == 70.0f) {
            func_001FBD50(arg0, 0x84E, 0, 300.0f);
        }
        d = *(float *)(arg0 + 0x3C);
        if (d <= 62.0f && !(d < 20.0f)) {
            if (!(*(float *)(arg1 + 0x2C) <= 0.5f)) {
                *(float *)(arg1 + 0x2C) = 0.5f;
                *(int *)(arg1 + 0x30) = 0;
            } else {
                *(int *)(arg1 + 0x30) = 0x3D4CCCCD;
            }
        } else {
            *(int *)(arg1 + 0x2C) = 0;
            *(int *)(arg1 + 0x30) = 0;
        }
        if (*(int *)(arg1 + 4) == 0 && *(float *)(arg0 + 0x3C) <= 49.0f &&
            func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            *(int *)(arg1 + 4) = 1;
            D_008102B0 |= 2;
            if (D_0081070A != 0) {
                D_008104D4 = 30.0f;
            } else {
                D_008104D4 = 25.0f;
            }
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760((float *)&D_00810320, (float *)&D_00810320);
            func_001B55E0(arg0, 1);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            if (*(int *)(arg1 + 4) != 0) {
                *(unsigned char *)(arg0 + 6) = 5;
                anim_clip_init((int)arg0, 0xB, 0.0f, 0.0f);
            } else {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                anim_clip_init((int)arg0, 8, z, (z = 0.0f));
            }
        }
        break;
    case 3:
        ang = 2.5f;
        steer = 1;
        if (!(*(float *)(arg1 + 0x2C) <= 0.5f)) {
            *(float *)(arg1 + 0x2C) = 0.5f;
            *(int *)(arg1 + 0x30) = 0;
        } else {
            *(int *)(arg1 + 0x30) = 0x3D4CCCCD;
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(int *)(arg1 + 4) = 0;
            *(int *)(arg1 + 0x2C) = 0;
            *(int *)(arg1 + 0x30) = 0;
            anim_clip_init((int)arg0, 0xA, 0.0f, 0.0f);
            func_001FBD50(arg0, 0x84F, 0, 300.0f);
        }
        break;
    case 4:
        if (*(int *)(arg1 + 4) == 0 && *(float *)(arg0 + 0x3C) <= 37.0f &&
            func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            *(int *)(arg1 + 4) = 1;
            D_008102B0 |= 2;
            if (D_0081070A != 0) {
                D_008104D4 = 35.0f;
            } else {
                D_008104D4 = 30.0f;
            }
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760((float *)&D_00810320, (float *)&D_00810320);
            func_001B55E0(arg0, 1);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init((int)arg0, 0xC, 0.0f, 0.0f);
        }
        break;
    case 5:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x5D) = 0;
            *(short *)(arg1 + 0x50) = ((func_00122BB8() >> 11) & 0x1F) + 0x3C;
        }
        break;
    }
    if (steer) {
        *(float *)(arg1 + 0x24) = func_001B1240((int)arg0 + 0xB0, D_00810360, D_00810368);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x24), *(float *)(arg0 + 0xC4), 3.1415927f * ang / 180.0f);
    }
}
