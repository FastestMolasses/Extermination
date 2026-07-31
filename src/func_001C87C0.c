// NEARMISS func_001C87C0  (vram 0x001C87C0, 0x590 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.40% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring/scheduling permutation across the three near-identical (rotation/translation/scale) directory-lookup blocks: which local wins s4 vs s5 and which temp wins a1 vs a2 for the section-base/bone-deref pair differs from target in a handful of spots, plus one duplicate-branch delay-slo...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Per-frame TRS channel advancer for a bone array. For each bone i in
// [0,n): decrement the rotation/translation/scale channel durations
// (+0x60/+0x58/+0x5C) by dt; if a duration goes <= 0, the channel's
// keyframe stream (D_00275BF4/BF0/BEC directories) is walked forward by
// one record via func_001C84D0 (rotation) / anim_decode_translation
// (translation, scale) and the channel's drive state (blend/inv_dur/dur,
// step counter) is refreshed exactly like anim_sample_rotation/func_001C90D0;
// otherwise the channel is advanced in place by lerping its stored delta
// into the live vector. The scale channel additionally captures the
// record's bit-0x8000 "clip end" flag (end_flag) into var_s3.  If that
// flag was seen (or the global door/lock latch D_008106F3 is set), the
// bottom loop resets each bone's translation/scale accumulators to 0.
//
// NEARMISS: 98.40% (mwcc 2.3.3), 92.40% (pinned 991202). Logic and
// structure fully recovered from the raw disassembly; the residual is a
// register-coloring/scheduling permutation (which local gets s4 vs s5/a1
// vs a2 in the three near-identical directory-lookup blocks, plus a
// duplicate-branch delay-slot placement at entry and D_008106F3's
// absolute lui/lbu addressing) -- not a logic difference. Parked per the
// regalloc-permutation wall class after exhausting the guard-extract /
// inverse-CSE idioms (see docs).
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
extern char D_008106F3;

extern void func_001C84D0(unsigned short *, float *);
extern void anim_decode_translation(unsigned short *, float *);
extern void func_001C86A0(float *, float *, float *, float);

void func_001C87C0(BoneChan **bones, int n, float dt) {
    int i;
    int end_flag;
    char *rec;
    BoneChan **b;
    BoneChan *bone;
    unsigned short t_prev;
    unsigned short t_next;
    float fdur;
    float old_dur;

    end_flag = 0;
    if (0 < n) {
        i = 0;
        b = bones;
        do {
            b[0]->dur -= dt;
            bone = b[0];
            if (bone->dur <= 0.0f) {
                rec = D_00275BF4 + *(int *)(D_00275BF4 + i * 4) + bone->step_r * 0xC;
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
                rec = D_00275BF0 + *(int *)(D_00275BF0 + i * 4) + bone->step_t * 0xC;
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
            if (b[0]->dur_s <= 0.0f) {
                rec = D_00275BEC + *(int *)(D_00275BEC + i * 4) + b[0]->step_s * 0xC;
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
            i++;
            b++;
        } while (i < n);
    }

    {
        int has_more = 0 < n;
        if (end_flag == 0) {
            if (D_008106F3 != 0) {
                has_more = 0 < n;
                goto reset;
            }
        } else {
reset:
            i = 0;
            if (has_more != 0) {
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
}
