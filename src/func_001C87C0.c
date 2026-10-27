// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.40% NEARMISS).
// Fixes read from the original instructions:
//  - D_008106F3 is addressed absolutely (not gp-relative) and read unsigned:
//    over-declared as `unsigned char [16]` (idiom #20, > sdata threshold).
//  - The bottom reset runs when `end_flag || D_008106F3` (plain || condition,
//    not the goto shape), with its own `i = 0; if (0 < n)` loop guard.
//  - `i = 0` is set before the `0 < n` guard of the main loop, and the loop
//    advances `b` before `i` (fixes the s5/s6 increment order and the
//    entry delay slot).
//  - Each directory lookup reads the stream offset into a local, then forms
//    `rec = dir + off; rec += step * 12;` (fixes the s4/a0/a2 colouring), and
//    the scale block tests and indexes through `bone` like the other two.
//
// Per-frame TRS channel advancer for a bone array. For each bone i in
// [0,n): decrement the rotation/translation/scale channel durations
// (+0x60/+0x58/+0x5C) by dt; if a duration goes <= 0, the channel's
// keyframe stream (D_00275BF4/BF0/BEC directories) is walked forward by
// one record via func_001C84D0 (rotation) / anim_decode_translation
// (translation, scale) and the channel's drive state (blend/inv_dur/dur,
// step counter) is refreshed exactly like anim_sample_rotation/func_001C90D0;
// otherwise the channel is advanced in place by lerping its stored delta
// into the live vector. The scale channel additionally captures the
// record's bit-0x8000 "clip end" flag (end_flag).  If that
// flag was seen (or the global door/lock latch D_008106F3 is set), the
// bottom loop resets each bone's translation/scale accumulators to 0.
typedef struct BoneChan {
    float pos[3];           /* 0x00 */
    float trans_a[3];       /* 0x0C */
    float scratch18[3];     /* 0x18 */
    float scale_a[3];       /* 0x24 */
    char pad30[0x20];       /* 0x30 quat A/B */
    float blend;            /* 0x50 */
    float inv_dur;          /* 0x54 */
    float dur_t;            /* 0x58 */
    float dur_s;            /* 0x5C */
    float dur;              /* 0x60 */
    char pad62[0x2];
    unsigned short step_r;  /* 0x66 */
    unsigned short step_t;  /* 0x68 */
    unsigned short step_s;  /* 0x6A */
} BoneChan;

extern char *D_00275BF4;
extern char *D_00275BF0;
extern char *D_00275BEC;
extern unsigned char D_008106F3[16];

extern void func_001C84D0(unsigned short *, float *);
extern void anim_decode_translation(unsigned short *, float *);
extern void func_001C86A0(float *, float *, float *, float);

void func_001C87C0(BoneChan **bones, int n, float dt) {
    int i;
    int end_flag;
    char *rec;
    int off;
    BoneChan **b;
    BoneChan *bone;
    unsigned short t_prev;
    unsigned short t_next;
    float fdur;
    float old_dur;

    end_flag = 0;
    i = 0;
    if (0 < n) {
        b = bones;
        do {
            b[0]->dur -= dt;
            bone = b[0];
            if (bone->dur <= 0.0f) {
                off = *(int *)(D_00275BF4 + i * 4);
                rec = D_00275BF4 + off;
                rec += bone->step_r * 0xC;
                t_prev = *(unsigned short *)(rec + 0xA);
                fdur = (float)(unsigned int)t_prev;
                func_001C84D0((unsigned short *)rec, (float *)((char *)b[0] + 0x30));
                t_next = *(unsigned short *)(rec + 0x16);
                fdur = (float)(unsigned int)t_next - fdur;
                func_001C84D0((unsigned short *)(rec + 0xC), (float *)((char *)b[0] + 0x40));
                old_dur = b[0]->dur;
                b[0]->dur = old_dur + fdur;
                b[0]->inv_dur = 1.0f / fdur;
                b[0]->blend = -old_dur * b[0]->inv_dur;
                b[0]->step_r += 1;
            } else {
                b[0]->blend += dt * b[0]->inv_dur;
            }

            b[0]->dur_t -= dt;
            bone = b[0];
            if (bone->dur_t <= 0.0f) {
                off = *(int *)(D_00275BF0 + i * 4);
                rec = D_00275BF0 + off;
                rec += bone->step_t * 0xC;
                t_prev = *(unsigned short *)(rec + 0xA);
                fdur = (float)(unsigned int)t_prev;
                anim_decode_translation((unsigned short *)rec, (float *)b[0]);
                t_next = *(unsigned short *)(rec + 0x16);
                fdur = (float)(unsigned int)t_next - fdur;
                anim_decode_translation((unsigned short *)(rec + 0xC), (float *)((char *)b[0] + 0xC));
                func_001C86A0((float *)((char *)b[0] + 0xC), (float *)((char *)b[0] + 0xC), (float *)b[0], fdur);
                old_dur = b[0]->dur_t;
                b[0]->dur_t = old_dur + fdur;
                b[0]->pos[0] += -old_dur * b[0]->trans_a[0];
                b[0]->pos[1] += -old_dur * b[0]->trans_a[1];
                b[0]->pos[2] += -old_dur * b[0]->trans_a[2];
                b[0]->step_t += 1;
            } else {
                b[0]->pos[0] += dt * b[0]->trans_a[0];
                b[0]->pos[1] += dt * b[0]->trans_a[1];
                b[0]->pos[2] += dt * b[0]->trans_a[2];
            }

            b[0]->dur_s -= dt;
            bone = b[0];
            if (bone->dur_s <= 0.0f) {
                off = *(int *)(D_00275BEC + i * 4);
                rec = D_00275BEC + off;
                rec += bone->step_s * 0xC;
                t_prev = *(unsigned short *)(rec + 0xA);
                fdur = (float)(unsigned int)t_prev;
                anim_decode_translation((unsigned short *)rec, (float *)((char *)b[0] + 0x18));
                end_flag = (*(unsigned short *)(rec + 8) & 0x8000) ? 1 : 0;
                t_next = *(unsigned short *)(rec + 0x16);
                fdur = (float)(unsigned int)t_next - fdur;
                anim_decode_translation((unsigned short *)(rec + 0xC), (float *)((char *)b[0] + 0x24));
                func_001C86A0((float *)((char *)b[0] + 0x24), (float *)((char *)b[0] + 0x24), (float *)((char *)b[0] + 0x18), fdur);
                old_dur = b[0]->dur_s;
                b[0]->dur_s = old_dur + fdur;
                b[0]->scratch18[0] += -old_dur * b[0]->scale_a[0];
                b[0]->scratch18[1] += -old_dur * b[0]->scale_a[1];
                b[0]->scratch18[2] += -old_dur * b[0]->scale_a[2];
                b[0]->step_s += 1;
            } else {
                b[0]->scratch18[0] += dt * b[0]->scale_a[0];
                b[0]->scratch18[1] += dt * b[0]->scale_a[1];
                b[0]->scratch18[2] += dt * b[0]->scale_a[2];
            }
            b++;
            i++;
        } while (i < n);
    }

    if (end_flag != 0 || D_008106F3[0] != 0) {
        i = 0;
        if (0 < n) {
            b = bones;
            do {
                i++;
                b[0]->inv_dur = 0.0f;
                b[0]->trans_a[0] = 0.0f;
                b[0]->trans_a[1] = 0.0f;
                b[0]->trans_a[2] = 0.0f;
                b[0]->scale_a[0] = 0.0f;
                b[0]->scale_a[1] = 0.0f;
                *(int *)((char *)b[0] + 0x2C) = 0;
                b++;
            } while (i < n);
        }
    }
}
