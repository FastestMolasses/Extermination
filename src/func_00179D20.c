// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-bone anim-clip pose seed. Bone 0 (*D_00275B40) gets its 3x3 rotation built
// in place at +0x90: identity (func_001029C0), compose with the +0x70 basis
// (func_00102C58), copy the +0x7C/80/84 translation to +0xC0/C4/C8, then apply the
// three fixed-point Euler angles at +0x88/8A/8C (1/4096 scale) via func_00103230
// to rows +0x90/A0/B0. Bones 1..arg0[0xC]-1 (walking D_00275B40[] by 4) build the
// same pose in the VU0 scratchpad: rotation at 0x70003400 seeded from the NLERP of
// the bone's quat pair (+0x30/+0x40, blend +0x50) through 0x70003600, angles from
// +0x18/1C/20; a second basis at 0x70003440 from +0x70 with translation +0x7C/80/84
// -> 0x70003470/74/78 and the +0x88/8A/8C Euler angles; func_001026D0 combines the
// two into the bone's own matrix at +0x90.
//
// idiom-22: the 0x7000xxxx scratchpad globals must be declared `volatile` -- the
// original treats them as volatile, which pins the call-argument evaluation order.
// Without it mwcc sinks the quat_nlerp out-pointer addiu into the jal delay slot
// instead of the third argument (the 99.82% near-miss).
extern void func_001029C0(void *p);
extern void func_00102C58(void *dst, void *src, void *b);
extern void func_00103230(void *a, void *b, float angle);
extern void quat_nlerp(float *out, float *a, float *b, float t);
extern void quat_to_mat3(void *out, float *q, void *bone);
extern void func_001026D0(void *bone, void *a, void *b);

extern void **D_00275B40;
extern volatile float D_70003400[4];
extern volatile float D_70003410[4];
extern volatile float D_70003420[4];
extern volatile float D_70003440[4];
extern volatile float D_70003450[4];
extern volatile float D_70003460[4];
extern volatile float D_70003600[4];

void func_00179D20(unsigned char *arg0) {
    unsigned char *p;
    int i;
    int off;
    unsigned char *b;

    p = (unsigned char *)*D_00275B40;
    func_001029C0(p + 0x90);
    func_00102C58(p + 0x90, p + 0x90, p + 0x70);
    *(float *)(p + 0xC0) = *(float *)(p + 0x7C);
    *(float *)(p + 0xC4) = *(float *)(p + 0x80);
    *(float *)(p + 0xC8) = *(float *)(p + 0x84);
    func_00103230(p + 0x90, p + 0x90, (float)*(short *)(p + 0x88) / 4096.0f);
    func_00103230(p + 0xA0, p + 0xA0, (float)*(short *)(p + 0x8A) / 4096.0f);
    func_00103230(p + 0xB0, p + 0xB0, (float)*(short *)(p + 0x8C) / 4096.0f);

    for (i = 1, off = 4; i < (int)arg0[0xC]; off += 4, i++) {
        b = *(unsigned char **)((char *)D_00275B40 + off);
        func_001029C0((float *)D_70003400);
        quat_nlerp((float *)D_70003600, (float *)(b + 0x30), (float *)(b + 0x40), *(float *)(b + 0x50));
        quat_to_mat3((float *)D_70003400, (float *)D_70003600, b);
        func_00103230((float *)D_70003400, (float *)D_70003400, *(float *)(b + 0x18));
        func_00103230((float *)D_70003410, (float *)D_70003410, *(float *)(b + 0x1C));
        func_00103230((float *)D_70003420, (float *)D_70003420, *(float *)(b + 0x20));
        func_001029C0((float *)D_70003440);
        func_00102C58((float *)D_70003440, (float *)D_70003440, b + 0x70);
        *(volatile float *)0x70003470 = *(float *)(b + 0x7C);
        *(volatile float *)0x70003474 = *(float *)(b + 0x80);
        *(volatile float *)0x70003478 = *(float *)(b + 0x84);
        func_00103230((float *)D_70003440, (float *)D_70003440, (float)*(short *)(b + 0x88) / 4096.0f);
        func_00103230((float *)D_70003450, (float *)D_70003450, (float)*(short *)(b + 0x8A) / 4096.0f);
        func_00103230((float *)D_70003460, (float *)D_70003460, (float)*(short *)(b + 0x8C) / 4096.0f);
        func_001026D0(b + 0x90, (float *)D_70003400, (float *)D_70003440);
    }
}
