// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void anim_clip_init(int, int, float, float);

void func_001287F0(int obj, char *rec, int clip, float speed) {
    if (*(short *)(rec + 0xF8) == (short)clip) return;
    *(short *)(rec + 0xF8) = clip;
    anim_clip_init(obj, clip, speed, 0.0f);
}
