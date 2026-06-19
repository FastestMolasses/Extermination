// CFLAGS: -O4,p -sdatathreshold 0
extern void anim_clip_init(void *obj, int clip, float a, float b);

void func_001BFF90(void *obj, short *p, int clip) {
    if (p[1] != (short)clip) {
        float b = 0.0f;
        p[1] = (short)clip;
        anim_clip_init(obj, clip, 4.0f, b);
    }
}
