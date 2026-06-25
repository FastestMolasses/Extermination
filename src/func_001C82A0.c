// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-bone rigid-prop matrix composer. Builds the actor's TRS world matrix
// at +0xD0 from pos(+0xB0)/rot(+0xC0)/scale(+0x60), then for each bone
// (count byte at +0xC) re-derives a fresh transform in the VU0 scratchpad
// matrix at 0x70003440: load identity (func_001029C0), apply the bone's
// local translation (+0x70 of the bone's data block at +0x110), splat its
// rotation (+0x7C/0x80/0x84) and per-axis Euler angles (s16 at +0x88/0x8A/0x8C
// scaled by 1/4096) via func_00103230 on the three angle scratch blocks,
// concat into the destination at `out`, then propagate parent (+0x64; -1 =
// root uses the world matrix at +0xD0) into the bone's own matrix at +0x90.
extern void build_trs_matrix(void *mtx, void *pos, void *rot, void *scale);
extern void func_001029C0(void *mtx);
extern void func_00102C58(void *dst, void *src, void *v);
extern void func_00103230(void *dst, void *src, float angle);
extern void func_001026D0(void *dst, void *a, void *b);
extern int D_70003440;
extern int D_70003450;
extern int D_70003460;

void func_001C82A0(unsigned char *arg0, unsigned char *arg1) {
    int i;
    unsigned char *bone;
    unsigned char *out;
    short parent;
    unsigned char *p;

    build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, arg0 + 0xC0, arg0 + 0x60);
    out = arg1;
    i = 0;
    bone = arg0;
    for (; i < arg0[0xC]; i++) {
        func_001029C0(&D_70003440);
        func_00102C58(&D_70003440, &D_70003440, *(unsigned char **)(bone + 0x110) + 0x70);
        *(volatile float *)0x70003470 = *(float *)(*(unsigned char **)(bone + 0x110) + 0x7C);
        *(volatile float *)0x70003474 = *(float *)(*(unsigned char **)(bone + 0x110) + 0x80);
        *(volatile float *)0x70003478 = *(float *)(*(unsigned char **)(bone + 0x110) + 0x84);
        func_00103230(&D_70003440, &D_70003440, 0.00024414062f * *(short *)(*(unsigned char **)(bone + 0x110) + 0x88));
        func_00103230(&D_70003450, &D_70003450, 0.00024414062f * *(short *)(*(unsigned char **)(bone + 0x110) + 0x8A));
        func_00103230(&D_70003460, &D_70003460, 0.00024414062f * *(short *)(*(unsigned char **)(bone + 0x110) + 0x8C));
        func_001026D0(&D_70003440, out, &D_70003440);
        p = *(unsigned char **)(bone + 0x110);
        parent = *(short *)(p + 0x64);
        if (parent != -1) {
            func_001026D0(p + 0x90, *(unsigned char **)(arg0 + parent * 4 + 0x110) + 0x90, &D_70003440);
        } else {
            func_001026D0(p + 0x90, arg0 + 0xD0, &D_70003440);
        }
        bone += 4;
        out += 0x40;
    }
}
