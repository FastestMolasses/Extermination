// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: one step of a scripted animation/aim command, dispatched on the int
// command id at cmd+8 via jtbl_0026DF20 (6 entries, ids 0..5).  Ids 0, 1, 3, 4 and
// any out-of-range id do nothing but the common tail.  `state[4]` is the command's
// own sub-state (0 = start, 1 = running, 2 = wait-for-flag) and is advanced by the
// handlers themselves.
//   id 2 works on the per-command block `cmd`:
//     sub-state 0: latch the entity's current angle (ent+0xC4) into cmd+0x10, then
//       func_001B1380(cmd+0x30, ent+0xB0, ent+0xC4 angle) picks which of the two
//       clip ids (cmd+0x18 when it returns non-zero, cmd+0x14 otherwise) to start
//       with anim_clip_init(ent, clip, 20.0f, 0.0f).  Sets state[4] = 1 and returns
//       0 WITHOUT advancing animation time.
//     sub-state 1: if cmd+0x24 (the step rate) is 0 the target angle is simply the
//       current angle; otherwise func_001B1240(ent+0xB0, cmd+0x30, cmd+0x38) gives
//       the target and func_001B12B0(target, current, rate) steps ent+0xC4 toward
//       it.  Once ent+0xC4 has reached the target, start clip cmd+0x1C at 30.0f.
//   id 5 is the same pair of behaviours driven from globals instead of `cmd`:
//     the angle lives in D_00810374, the reference quats in D_00810350 /
//     D_00810360, and the result is published as D_008104A2 (clip id),
//     D_008104A8 (clip speed 20.0f / 30.0f) and D_008104A4 (blend 1.0f).
//     sub-state 2 waits on bit 0x1000 of D_008104B0 and returns 1 when it is set.
// Every other path ends by advancing the entity's animation clock one frame
// (anim_advance_time(ent, 1.0f)) and returning 0.  The return value is the
// "command finished" flag.
//

extern int   func_001B1380(char *dst, char *src, float t);
extern float func_001B1240(char *q, float x, float y);
extern float func_001B12B0(float cur, float target, float rate);
extern void  anim_clip_init(char *ent, int clip, float speed, float blend);
extern void  anim_advance_time(char *ent, float dt);

extern char  D_00810350[];
extern char  D_00810360[];
extern float D_00810374;
extern short D_008104A2;
extern float D_008104A4;
extern float D_008104A8;
extern int   D_008104B0;

int func_001B73A0(char *ent, char *state, char *cmd)
{
    float t;

    switch (*(int *)(cmd + 8)) {
    case 0:
    case 1:
        break;
    case 2:
        switch (*(unsigned char *)(state + 4)) {
        case 0:
            *(float *)(cmd + 0x10) = *(float *)(ent + 0xC4);
            if (func_001B1380(cmd + 0x30, ent + 0xB0, *(float *)(ent + 0xC4)))
                anim_clip_init(ent, *(short *)(cmd + 0x18), 20.0f, 0.0f);
            else
                anim_clip_init(ent, *(short *)(cmd + 0x14), 20.0f, 0.0f);
            *(char *)(state + 4) = 1;
            return 0;
        case 1:
            if (*(float *)(cmd + 0x24) == 0.0f) {
                t = *(float *)(ent + 0xC4);
            } else {
                t = func_001B1240(ent + 0xB0, *(float *)(cmd + 0x30), *(float *)(cmd + 0x38));
                *(float *)(ent + 0xC4) = func_001B12B0(t, *(float *)(ent + 0xC4), *(float *)(cmd + 0x24));
            }
            if (*(float *)(ent + 0xC4) == t) {
                anim_clip_init(ent, *(short *)(cmd + 0x1C), 30.0f, 0.0f);
            }
            break;
        }
        break;
    case 3:
    case 4:
        break;
    case 5:
        switch (*(unsigned char *)(state + 4)) {
        case 0:
            if (func_001B1380(cmd + 0x30, D_00810350, D_00810374)) {
                D_008104A2 = *(short *)(cmd + 0x18);
            } else {
                D_008104A2 = *(short *)(cmd + 0x14);
            }
            D_008104A8 = 20.0f;
            D_008104A4 = 1.0f;
            *(char *)(state + 4) = 1;
            break;
        case 1:
            if (*(float *)(cmd + 0x24) == 0.0f) {
                t = D_00810374;
            } else {
                t = func_001B1240(D_00810360, *(float *)(cmd + 0x30), *(float *)(cmd + 0x38));
                D_00810374 = func_001B12B0(t, D_00810374, *(float *)(cmd + 0x24));
            }
            if (D_00810374 == t) {
                D_008104A2 = *(short *)(cmd + 0x1C);
                D_008104A8 = 30.0f;
                *(char *)(state + 4) = 2;
            }
            break;
        case 2:
            if (D_008104B0 & 0x1000) {
                return 1;
            }
            break;
        }
        break;
    }

    anim_advance_time(ent, 1.0f);
    return 0;
}
