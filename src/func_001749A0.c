// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Keep f12 as the caller's blend and pass explicit source frame0 in f13.
// The old no-float prototype omitted this real initialization argument.
// The clip id arrives as a full int: the body sign-extends it itself before
// comparing it with the entity's current clip (+0x20C).
extern void anim_clip_init(unsigned char *actor, int clip, float blend, float frame);

int func_001749A0(unsigned char *e, int clip, int flags, float blend) {
    if (flags == 0) {
        short cur = *(short *)(e + 0x20C);
        if ((short)clip == cur) {
            return 1;
        }
    }
    *(short *)(e + 0x20C) = clip;
    anim_clip_init(e, *(short *)(e + 0x20C), blend, 0.0f);
    return 0;
}
