// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Samples every animated bone at frame t: for each of the n track pointers,
// rotation (anim_sample_rotation), then func_001C90D0 and func_001C92C0, each
// given the bone index and t converted by float_to_int.
extern int float_to_int(float x);
extern void anim_sample_rotation(char *track, int bone, int frame);
extern void func_001C90D0(char *track, int bone, int frame);
extern void func_001C92C0(char *track, int bone, int frame);

void func_001C8710(char **tracks, int n, float t) {
    int i;

    for (i = 0; i < n; i++) {
        anim_sample_rotation(tracks[i], i, float_to_int(t));
        func_001C90D0(tracks[i], i, float_to_int(t));
        func_001C92C0(tracks[i], i, float_to_int(t));
    }
}
