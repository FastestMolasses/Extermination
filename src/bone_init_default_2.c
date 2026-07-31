// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Init a *(arg0+0x2C)=arg1; convert *D_00275BF8+2 (u16) to float via the
// unsigned->float idiom into +0x3C; then for each of *(arg0+0xC) entries seed
// the 0x110-pointed sub-struct (copy *src into +0x64, set +0x88/8A/8C=0x1000,
// zero +0x70/74/78/7C/80/84) and finish with func_001C8710(...,0.0f).
extern void anim_clip_resolve(int, short);
extern void func_001C8710(char *, unsigned char, int, short *, float);
extern char *D_00275BF8;

void bone_init_default_2(char *arg0, short arg1) {
    unsigned short v;
    short *src;
    char *p;
    int i;

    *(short *)(arg0 + 0x2C) = arg1;
    anim_clip_resolve(*(int *)(arg0 + 0x40), *(short *)(arg0 + 0x2C));
    v = *(unsigned short *)(D_00275BF8 + 2);
    *(float *)(arg0 + 0x3C) = (float) (unsigned int) v;
    i = 0;
    p = arg0;
    src = (short *)(D_00275BF8 + 0x20);
    for (; i < *(unsigned char *)(arg0 + 0xC); i++) {
        *(short *)(*(char **)(p + 0x110) + 0x64) = *src;
        *(short *)(*(char **)(p + 0x110) + 0x88) = 0x1000;
        *(short *)(*(char **)(p + 0x110) + 0x8A) = 0x1000;
        *(short *)(*(char **)(p + 0x110) + 0x8C) = 0x1000;
        *(int *)(*(char **)(p + 0x110) + 0x7C) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x80) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x84) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x70) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x74) = 0;
        *(int *)(*(char **)(p + 0x110) + 0x78) = 0;
        src += 2;
        p += 4;
    }
    func_001C8710(arg0 + 0x110, *(unsigned char *)(arg0 + 0xC), i, src, 0.0f);
}
