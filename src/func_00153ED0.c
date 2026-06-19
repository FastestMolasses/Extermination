// CFLAGS: -O4,p -sdatathreshold 0
extern void anim_clip_init(int, int, float, float);

void func_00153ED0(int a0, short *a1, int a2, float f12) {
    if (a1[0] != (short)a2) {
        a1[0] = (short)a2;
        anim_clip_init(a0, a2, f12, 0.0f);
    }
}
