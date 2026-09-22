// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Initialize a clip at an explicit source frame. The high clip bit marks
// the transition interval. Zero blend seeds a one-tick transition and
// resolves it immediately; nonzero blend retains the requested interval.
extern void anim_clip_resolve(void *, short);
extern short func_00128250(float);
extern void anim_sample_bones(void *, unsigned char, float, float);
extern void anim_advance_time(void *, float);
void anim_clip_init(unsigned char *actor, int clip, float blend, float frame) {
    *(short *)(actor + 0x2C) = clip | 0x8000;
    anim_clip_resolve(*(void **)(actor + 0x40), *(short *)(actor + 0x2C));
    if (blend == 0.0f) {
        *(float *)(actor + 0x3C) = 1.0f;
        *(short *)(*(unsigned char **)(actor + 0x110) + 0x8E) = func_00128250(frame);
        anim_sample_bones(actor + 0x110, actor[0xC], frame, 1.0f);
        anim_advance_time(actor, 1.0f);
        return;
    }
    *(float *)(actor + 0x3C) = blend;
    *(short *)(*(unsigned char **)(actor + 0x110) + 0x8E) = func_00128250(frame);
    anim_sample_bones(actor + 0x110, actor[0xC], frame, blend);
}
