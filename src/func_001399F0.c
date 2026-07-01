// NEARMISS func_001399F0  (vram 0x001399F0, 0x410 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// 2-state actor tick keyed on p[6] (a1 companion struct = e). State 0 seeds
// heading/anim, RNG-picks a facing angle via func_00122BB8/func_001B1470,
// optionally nudges heading toward a target at 0x700038A0 via
// func_00102948/func_0019A570, then rebuilds a bone-transform scratch matrix
// at D_700038A0/D_700038B0/D_700038C0/D_700038D0 and calls func_001EFF10
// to attach it at p+0x20. State 1 advances a countdown timer at e+0x44 by
// e+0x48; on expiry resets the actor and starts a new clip; otherwise builds
// a vec4 transform (translate/scale via func_001029C0/func_00102C58/
// func_001026A0), applies it to p+0xB0 via func_001028B8/func_001028D0, and
// if func_0019A440 signals a hit, negates/renormalizes and writes the result
// back to p+0xB0..0xBC.
extern int func_00122BB8(void);
extern float func_001B1470(float);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_00102948(void *a0, void *a1);
extern int func_0019A570(void *a0, void *a1, int a2, int a3);
extern void *func_001EFF10(int mode, int a1, void *a2, void *a3, void *a4, void *a5);
extern void func_001029C0(void *a0);
extern void func_00102C58(void *a0, int unused, void *a1);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028B8(void *a0, void *a1, void *a2);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern int func_0019A440(void *a0, void *a1, int a2);
extern void func_00102760(void *a0, void *a1);
extern void func_001028E8(void *a0, void *a1, void *a2);

extern char D_700031B0[];
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

void func_001399F0(char *p, char *e) {
    int t;
    float f, z;

    t = p[6];
    switch (t) {
    case 0:
        p[6] = (char)(t + 1);
        *(int *)(p + 0xC8) = 0;
        *(float *)(e + 0x44) = 9.0f;
        *(float *)(e + 0x48) = -1.7f;
        anim_clip_init(p, 5, (z = 0.0f), z);
        f = func_001B1470(6.2831855f * (float)((func_00122BB8() >> 18) & 0xFF) / 255.0f);
        *(float *)(e + 0x58) = f;
        if (f < 0.0f) {
            func_00102948(D_700038A0, p + 0xB0);
            *(float *)0x700038A4 = *(float *)0x700038A4 - 10.0f;
            if (func_0019A570(p + 0xB0, D_700038A0, 6, 0) != 0) {
                *(float *)(e + 0x58) = func_001B1470(3.1415927f + *(float *)(e + 0x58));
            }
        }
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 0.0f;
        *(int *)0x700038AC = 0x3F800000;
        *(float *)0x700038B0 = 10.0f;
        *(int *)0x700038B4 = 0;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0x3F800000;
        *(int *)0x700038C0 = 0x43000000;
        *(int *)0x700038D0 = 0x43000000;
        *(int *)0x700038D4 = 0x43000000;
        *(int *)0x700038D8 = 0x43000000;
        *(int *)0x700038C4 = 0x42200000;
        *(int *)0x700038C8 = 0x42A00000;
        *(int *)0x700038CC = 0x42A00000;
        *(int *)0x700038DC = 0x42A00000;
        *(void **)(p + 0x20) = func_001EFF10(0x8000000D, *(int *)(p + 0x120) + 0x90, D_700038A0, D_700038B0, D_700038C0, D_700038D0);
        /* fallthrough */
    case 1:
        break;
    default:
        return;
    }

    f = *(float *)(e + 0x44);
    if (f <= 0.0f) {
        p[5] = 2;
        p[6] = 0;
        *(float *)(e + 0x58) = *(float *)(p + 0xC4);
        *(float *)(e + 0x48) = 0.0f;
        *(float *)(e + 0x44) = 0.0f;
        *(short *)(p + 0x32) = 0x78;
        anim_clip_init(p, 4, 10.0f, 0.0f);
        (*(char **)(p + 0x20))[4] = 3;
        *(void **)(p + 0x20) = 0;
        return;
    }
    *(float *)(e + 0x44) = f + *(float *)(e + 0x48);
    func_001029C0(D_700036A0);
    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = *(float *)(p + 0xC4);
    *(float *)0x700038A8 = *(float *)(e + 0x58);
    *(int *)0x700038AC = 0x3F800000;
    func_00102C58(D_700036A0, 0, D_700038A0);
    *(float *)0x700038A0 = *(float *)(e + 0x44);
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038A8 = 3.5f;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    func_001028B8(p + 0xB0, p + 0xB0, D_700038A0);
    func_001028D0(D_700038B0, p + 0xB0, D_700038A0);
    if (func_0019A440(D_700038B0, p + 0xB0, 6) != 0) {
        func_00102760(D_700038A0, D_700038A0);
        *(int *)0x700038B0 = 0xBF000000;
        *(int *)0x700038B4 = 0xBF000000;
        *(int *)0x700038B8 = 0xBF000000;
        *(int *)0x700038BC = 0x3F800000;
        func_001028E8(D_700038A0, D_700038A0, D_700038B0);
        func_001028B8(D_700038A0, D_700031B0, D_700038A0);
        *(float *)(p + 0xB0) = *(float *)0x700038A0;
        *(float *)(p + 0xB4) = *(float *)0x700038A4;
        *(float *)(p + 0xB8) = *(float *)0x700038A8;
        *(float *)(p + 0xBC) = *(float *)0x700038AC;
    }
}
