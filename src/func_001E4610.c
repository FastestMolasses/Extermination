// NEARMISS func_001E4610  (vram 0x001E4610, 0x3D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 88.31% (233) / 82.19% (991202). Body/logic fully recovered: 4-state dispatch (0=init emitter fields+ident transform+link to other-actor scratch; 1=tick; 2/3=free via func_001AFC10). Residual is a genuine scheduling artifact: a speculative dead prefetch pair (`ld t0,0x20(s0)`/`lw t1,0x1c(...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern void func_00102918(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102C58(void *a0, void *a1, void *a2);
extern int func_00128250(float a);
extern void func_001AFC10(unsigned char *handle);
extern void func_001B17A0(unsigned char *self);
extern void func_001CE660(int a0, int a1, void *a2, void *a3);
extern float D_700038B0;
extern void func_001E4600(void);

void func_001E4610(unsigned char *arg0) {
    unsigned char *other;
    unsigned char *scratch;
    unsigned char *other_scratch;
    unsigned char state;
    float f;

    other = *(unsigned char **)(arg0 + 0x14);
    other_scratch = other + 0x1F0;
    scratch = arg0 + 0x1F0;
    state = *(unsigned char *)(arg0 + 4);

    switch (state) {
    case 0:
        break;
    case 1:
        goto tick;
    case 2:
    case 3:
        func_001AFC10(arg0);
        return;
    default:
        return;
    }
    {
        if (*(unsigned char *)(scratch + 0xD) == 1) {
            *(float *)(scratch + 0x14) = 30.0f;
            *(float *)(scratch + 0x28) = 5.0f;
            *(int *)(scratch + 0x44) = 0;
        } else {
            *(float *)(scratch + 0x14) = 50.0f;
            *(float *)(scratch + 0x28) = 5.0f;
            *(int *)(scratch + 0x44) = 0;
        }
        *(int *)(scratch + 0x50) = 0;

        *(float *)(scratch + 0x14) = *(float *)(scratch + 0x14) / 2.0f;
        *(float *)(scratch + 0x18) = 0.0f;
        *(long long *)(scratch + 0x20) = ((long long)0x20041A85LL << 32) | 0x553220D0LL;
        *(int *)(scratch + 0x30) = 0x14;
        *(int *)(scratch + 0x34) = 0;
        *(int *)(scratch + 0x38) = 0x43B40000;

        func_001029C0(arg0 + 0xD0);
        func_00102C58(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xC0);
        func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);

        *(unsigned char **)(arg0 + 0x30) = other_scratch;
        *(void (**)(void))(arg0 + 0x34) = func_001E4600;
        *(unsigned char *)(arg0 + 0xC) = 0;
        *(unsigned char *)(arg0 + 9) = 0;
        *(unsigned char *)(arg0 + 0) = 1;
        *(unsigned char *)(arg0 + 4) = 1;
    }

tick:
    f = *(float *)(scratch + 0x18);
    *(float *)0x70003A24 = f;
    if (!(f <= 1.0f)) {
        float t;
        *(unsigned char *)(arg0 + 0) = 2;
        t = *(float *)0x70003A24 - 1.0f;
        *(float *)0x70003A20 = t;
        t = t / 0.7f;
        t = 1.0f - t;
        *(float *)0x70003A20 = t;
        if (t < 0.0f) {
            t = 0.0f;
        }
        *(float *)0x70003A20 = t;
        *(float *)0x70003A20 = *(float *)0x70003A20 * 24.0f;
    } else {
        *(unsigned char *)(arg0 + 0) = 1;
        *(float *)0x70003A20 = 24.0f;
    }

    *(float *)(scratch + 0x3C) = *(float *)0x70003A24 * *(float *)(scratch + 0x14);
    *(float *)(scratch + 0x40) = *(float *)0x70003A24 * *(float *)(scratch + 0x14);
    *(float *)(scratch + 0x48) = *(float *)(scratch + 0x28) + *(float *)0x70003A24 * *(float *)(scratch + 0x14);
    *(float *)(scratch + 0x4C) = *(float *)(scratch + 0x28) + *(float *)0x70003A24 * *(float *)(scratch + 0x14);

    *(float *)0x700038B0 = *(float *)(scratch + 0x48);
    *(int *)0x700038B4 = 0x3F800000;
    *(float *)0x700038B8 = *(float *)(scratch + 0x4C);
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(scratch, arg0 + 0xD0, &D_700038B0);
    func_001028D0(scratch, scratch, arg0 + 0x100);

    {
        float v20 = *(float *)0x70003A20;
        *(float *)0x700038A0 = v20;
        *(float *)0x700038A4 = v20;
        *(float *)0x700038A8 = v20;
        *(int *)0x700038AC = 0;
    }

    *(int *)(scratch + 0x1C) = func_00128250(*(float *)0x700038A0);
    *(int *)(scratch + 0x1C) = *(int *)(scratch + 0x1C) | (func_00128250(*(float *)0x700038A4) << 8);
    *(int *)(scratch + 0x1C) = *(int *)(scratch + 0x1C) | (func_00128250(*(float *)0x700038A8) << 0x10);
    *(int *)(scratch + 0x1C) = *(int *)(scratch + 0x1C) | (func_00128250(*(float *)0x700038AC) << 0x18);

    func_001CE660(0, 2, arg0 + 0xD0, scratch + 0x30);

    {
        float v = *(float *)(scratch + 0x18);
        v = v + (2.0f - v) / 25.0f;
        *(float *)(scratch + 0x18) = v;
        v = v + 0.001f;
        *(float *)(scratch + 0x18) = v;
        if (v > 2.0f) {
            *(unsigned char *)(arg0 + 4) = 3;
            *(unsigned char *)(arg0 + 0) = 2;
        }
    }

    func_001B17A0(arg0);
}
