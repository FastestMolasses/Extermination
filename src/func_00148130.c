// NEARMISS func_00148130  (vram 0x00148130, 0x2D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered: 4-state animation blend-transition state machine (state field p[6]) driving anim_clip_init transitions gated on e+0x20 flag 0x1000 / e+0x81 bit 0 / probe results from func_001B3250, func_001B1560, func_001B13F0. All branch polarities, the FP-ARG-ORDER idiom (z=0.0f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_init(char *self, int clip, float a, float b);
extern float func_001B1240(char *p, int a, int b, float c, float d);
extern float func_001B12B0(float a, float b, float c);
extern void func_001B2B10(char *p, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B3250(char *p, void *v, float r);
extern int func_001B1560(char *p, void *v, float r);
extern int func_001B13F0(void *v, char *p, float r);

extern float D_00810360;
extern float D_00810368;
extern char D_700038A0[];

void func_00148130(unsigned char *p, unsigned char *e) {
    unsigned char st;
    float z;

    st = p[6];
    switch (st) {
    case 0:
        p[6] = p[6] + 1;
        *(int *)(e + 0x50) = 0;
        *(float *)(e + 0x4C) = 0.0f;
        anim_clip_init((char *)p, 3, 1.0f, (z = 0.0f));
        return;
    case 1:
        *(float *)(e + 0x44) = func_001B1240((char *)(p + 0xB0), 1, st, D_00810360, D_00810368);
        *(float *)(p + 0xC4) = func_001B12B0(*(float *)(e + 0x44), *(float *)(p + 0xC4), 0.034906585f);
        if (*(int *)(e + 0x20) & 0x1000) {
            p[6] = p[6] + 1;
            *(int *)(e + 0x50) = 0x3CA3D70A;
            anim_clip_init((char *)p, 4, 1.0f, 0.0f);
            return;
        }
        return;
    case 2:
        if (!(*(float *)(e + 0x4C) <= 0.6f)) {
            *(float *)(e + 0x4C) = 0.6f;
            *(int *)(e + 0x50) = 0;
        }
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 3.0f;
        *(float *)0x700038A8 = 30.0f;
        *(float *)0x700038AC = 1.0f;
        func_001B2B10((char *)p, D_700038A0, D_700038A0);
        func_001028B8(D_700038A0, D_700038A0, p + 0xB0);
        if (func_001B3250((char *)p, D_700038A0, 15.0f) != 0 || (((char *)e)[0x81] & 1)) {
            p[6] = p[6] + 1;
            *(int *)(e + 0x50) = 0xBC23D70A;
            anim_clip_init((char *)p, 5, 1.0f, 0.0f);
            return;
        }
        if (func_001B1560((char *)p, &D_00810360, 1.3962635f) == 0 ||
            func_001B13F0(&D_00810360, (char *)(p + 0xB0), 30.0f) != 0) {
            p[6] = p[6] + 1;
            *(int *)(e + 0x50) = 0xBC23D70A;
            anim_clip_init((char *)p, 5, 1.0f, 0.0f);
            return;
        }
        break;
    case 3:
        if (*(float *)(e + 0x4C) < 0.0f) {
            *(int *)(e + 0x50) = 0;
            *(float *)(e + 0x4C) = 0.0f;
        }
        if (*(int *)(e + 0x20) & 0x1000) {
            p[5] = 0;
            p[6] = 0;
            *(int *)(e + 0x50) = 0;
            *(float *)(e + 0x4C) = 0.0f;
            anim_clip_init((char *)p, 0, 5.0f, 0.0f);
        }
        break;
    }
}
