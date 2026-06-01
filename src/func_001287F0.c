// CFLAGS: -O4,p -sdatathreshold 0
extern void anim_clip_init(int, int);

void func_001287F0(int a0, short *a1, short a2) {
    if (a1[0xF8 / 2] == a2) return;
    a1[0xF8 / 2] = a2;
    anim_clip_init(a0, a2);
}
