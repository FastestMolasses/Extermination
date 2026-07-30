// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Per-bone ROTATION keyframe directory walk + interval sampler. Resolves the
// bone's keyframe stream via the section-1 directory (D_00275BF4 +
// u32-offset[bone_idx]), walks 12-byte records counting steps into
// actor->step (+0x66, seeded 1) until t_prev(+0xA) <= time < t_next(+0x16),
// then decodes both straddling rotation keyframes via func_001C84D0 into
// quat A (+0x30) and quat B (+0x40) and writes NLERP drive state:
// dur(+0x60)=t_next-time, inv_dur(+0x54)=1/(t_next-t_prev),
// blend(+0x50)=inv_dur*(time-t_prev). u16 times convert through the
// (float)(unsigned int) idiom; time converts signed.
//
// mwcc 2.3.3 matches byte-identical; the pinned 991202 build caps at 82.45%.
// Keys: -sdatathreshold 8 puts D_00275BF4 through gp-rel (single lw 0(gp),
// not lui/lw) matching CW; BoneChan struct fields defeat mwcc address-CSE on
// the +0x54 store-then-reload; and writing the FIRST guard as `t_prev > time`
// (not `time < t_prev`) makes CW's `slt $at` register choice match (mwcc
// otherwise colored that compare into $v0).
typedef struct BoneChan {
    char pad00[0x30];
    float quat_a[4];        /* 0x30 */
    float quat_b[4];        /* 0x40 */
    float blend;            /* 0x50 */
    float inv_dur;          /* 0x54 */
    char pad58[0x8];
    float dur;              /* 0x60 */
    char pad64[0x2];
    unsigned short step;    /* 0x66 */
} BoneChan;

extern char *D_00275BF4;
extern void func_001C84D0(unsigned short *, float *);

void anim_sample_rotation(BoneChan *actor, int bone_idx, int time) {
    char *rec;
    int t_prev;
    int t_next;
    float fdur;

    rec = D_00275BF4 + *(int *)(D_00275BF4 + bone_idx * 4);
    actor->step = 1;
top:
    t_prev = *(unsigned short *)(rec + 0xA);
    if (t_prev > time) goto advance;
    t_next = *(unsigned short *)(rec + 0x16);
    if (time >= t_next) goto advance;

    fdur = (float)(unsigned int)t_prev;
    func_001C84D0((unsigned short *)rec, actor->quat_a);
    fdur = (float)(unsigned int)*(unsigned short *)(rec + 0x16) - fdur;
    func_001C84D0((unsigned short *)(rec + 0xC), actor->quat_b);
    actor->dur = (float)(unsigned int)t_next - (float)time;
    actor->inv_dur = 1.0f / fdur;
    actor->blend = actor->inv_dur * ((float)time - (float)(unsigned int)t_prev);
    goto done;

advance:
    actor->step += 1;
    rec += 0xC;
    goto top;

done:
    return;
}
