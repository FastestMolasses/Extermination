// NEARMISS func_00179D20  (vram 0x00179D20, 0x2CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (per-bone anim-clip pose seed loop over D_00275B40[]; scalar init block + loop with quat_nlerp/quat_to_mat3/func_00103230 sequence). Sole residual (3 of 173 instructions, all in the loop's first quat_nlerp call) is an arg-materialization-order artifact: target compu...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void func_001029C0(void *p);
extern void func_00102C58(void *dst, void *src, void *b);
extern void func_00103230(void *a, void *b, float angle);
extern void quat_nlerp(float *out, float *a, float *b, float t);
extern void quat_to_mat3(void *out, float *q, void *bone);
extern void func_001026D0(void *bone, void *a, void *b);

extern void **D_00275B40;
extern float D_70003400[4];
extern float D_70003410[4];
extern float D_70003420[4];
extern float D_70003440[4];
extern float D_70003450[4];
extern float D_70003460[4];
extern float D_70003600[4];

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
        func_001029C0(D_70003400);
        quat_nlerp(D_70003600, (float *)(b + 0x30), (float *)(b + 0x40), *(float *)(b + 0x50));
        quat_to_mat3(D_70003400, D_70003600, b);
        func_00103230(D_70003400, D_70003400, *(float *)(b + 0x18));
        func_00103230(D_70003410, D_70003410, *(float *)(b + 0x1C));
        func_00103230(D_70003420, D_70003420, *(float *)(b + 0x20));
        func_001029C0(D_70003440);
        func_00102C58(D_70003440, D_70003440, b + 0x70);
        *(float *)0x70003470 = *(float *)(b + 0x7C);
        *(float *)0x70003474 = *(float *)(b + 0x80);
        *(float *)0x70003478 = *(float *)(b + 0x84);
        func_00103230(D_70003440, D_70003440, (float)*(short *)(b + 0x88) / 4096.0f);
        func_00103230(D_70003450, D_70003450, (float)*(short *)(b + 0x8A) / 4096.0f);
        func_00103230(D_70003460, D_70003460, (float)*(short *)(b + 0x8C) / 4096.0f);
        func_001026D0(b + 0x90, D_70003400, D_70003440);
    }
}
