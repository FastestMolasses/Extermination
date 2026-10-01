// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player sub-state +7 of a push / pull move (beat 12, the crevice jump).
// 0: when either speed +0x224 / +0x22C is non-zero, play sound 0x152 (with
//    func_0021C350) for +0x224 and 0x153 (with func_0021C270) for +0x22C, at
//    distance 300.0; advance +7, rumble (func_001B61C0(0, 0xC0, 5, 1)) and
//    start clip 0x6F (func_001749A0(e, 0x6F, 0, 1.0)); returns 1, or 0 when
//    both speeds are zero.
// 1: when bit 0x1000 of +0x200 is set, back to sub-state 0 with timer +0x20E
//    = 60 and blend into clip 0x6C (+0x25C < 2) or 0x6B over half its length
//    (func_001C61D0 frames, staged through the scratchpad float 0x70003A20)
//    with anim_clip_arbiter(e, clip, 4.0, frames / 2). Returns 1.
// Any other sub-state returns 1.
extern float D_70003A20;
extern void func_001FBD50(char *e, int sound, int b, float dist);
extern void func_0021C350(char *e);
extern void func_0021C270(char *e);
extern void func_001B61C0(int port, int strength, int mode, int arg3);
extern int func_001749A0(char *e, short clip, int flags, float blend);
extern int func_001C61D0(int model, int clip);
extern void anim_clip_arbiter(char *e, int clip, float speed, float frames);

int func_002243F0(char *e) {
    switch ((unsigned char)e[7]) {
    case 0:
        if (*(float *)(e + 0x224) || *(float *)(e + 0x22C)) {
            if (*(float *)(e + 0x224)) {
                func_001FBD50(e, 0x152, 0, 300.0f);
                func_0021C350(e);
            }
            if (*(float *)(e + 0x22C)) {
                func_001FBD50(e, 0x153, 0, 300.0f);
                func_0021C270(e);
            }
            ((unsigned char *)e)[7]++;
            func_001B61C0(0, 0xC0, 5, 1);
            func_001749A0(e, 0x6F, 0, 1.0f);
            break;
        }
        return 0;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[7] = 0;
            *(short *)(e + 0x20E) = 0x3C;
            if (((unsigned char *)e)[0x25C] < 2) {
                D_70003A20 = func_001C61D0(*(int *)(e + 0x40), 0x6C);
                anim_clip_arbiter(e, 0x6C, 4.0f, D_70003A20 / 2.0f);
            } else {
                D_70003A20 = func_001C61D0(*(int *)(e + 0x40), 0x6B);
                anim_clip_arbiter(e, 0x6B, 4.0f, D_70003A20 / 2.0f);
            }
        }
        break;
    }
    return 1;
}
