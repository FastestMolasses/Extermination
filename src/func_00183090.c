// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Scripted player clip request commit (00183090). A changed ordinary
// request initializes the transition and returns0, so the player frame
// does not also advance it. Special bone modes1/3 become2/4 and return1.
// An unchanged request returns1 to permit the ordinary animation advance.
extern void func_001D0C70(void);
extern void bone_init_default_2(unsigned char *, short);
extern void anim_clip_init(unsigned char *, int, float, float);

int func_00183090(unsigned char *player) {
    int mode;
    short requested;
    if (*(unsigned char *)0x70003B8F == 2) {
        func_001D0C70();
    }
    mode = player[0x2F3];
    if (mode != 0) {
        /* Retain the explicit byte conversion after the zero test. */
        mode &= 255;
        if (mode == 1) {
            bone_init_default_2(player, *(short *)(player + 0x1F2));
            *(int *)(player + 0x200) = 0;
            player[0x2F3] = 2;
            return 1;
        }
        if (mode == 3) {
            bone_init_default_2(player, *(short *)(player + 0x1F2));
            *(int *)(player + 0x200) = 0;
            player[0x2F3] = 4;
            return 1;
        }
        return 1;
    }
    requested = *(short *)(player + 0x1F2);
    if (requested == *(short *)(player + 0x20C)) return 1;
    *(short *)(player + 0x20C) = requested;
    anim_clip_init(player, *(short *)(player + 0x20C),
                   *(float *)(player + 0x1F8), 0.0f);
    *(int *)(player + 0x200) = 0;
    return 0;
}
