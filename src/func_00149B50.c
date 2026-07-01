// NEARMISS func_00149B50  (vram 0x00149B50, 0x574 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Explosion/effect-trigger state machine (5 states on arg0+6) driving anim_clip_init transitions, a func_00213-family blast-radius probe loop over 4 vec4 slots, and two literal-address (0x700038A0/0x700036A0) scratch-vector setups feeding func_001B2B10/func_001028B8 transform composition. Applied F...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_init(char *self, int clip, float speed, float d);
extern float func_001B1240(float *obj, float px, float py);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float v);
extern float func_0011DF78(float a0);
extern int func_0014C130(char *arg0);
extern int func_0021BED0(unsigned char *a0);
extern void func_0021BF90(unsigned char *a0, void *a1);
extern void func_0021C040(unsigned char *a0, void *a1);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001B2B10(char *arg0, void *arg1, void *arg2);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f12);
extern void func_00182F90(unsigned char *p, void *a1);
extern void *func_001EFEB0(int a0, void *a1);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);

extern int D_00245B90[16];
extern unsigned char D_008102B0;
extern float D_00810354;
extern float D_00810360;
extern float D_00810368;
extern float D_00810374;
extern int D_008104D4;
extern int D_008104DC;
extern unsigned char D_008106BD;
extern unsigned char D_0081070A;
extern int D_700036A0[4];
extern int D_700038A0[4];

typedef unsigned __int128 uint128;

void func_00149B50(char *arg0, char *arg1) {
    int sp50[16];
    int v0;
    int state;
    float z;
    uint128 q0, q1, q2, q3;
    volatile uint128 *src = (volatile uint128 *)D_00245B90;
    uint128 *dst = (uint128 *)sp50;

    q0 = src[0];
    q1 = src[1];
    q2 = src[2];
    q3 = src[3];
    dst[0] = q0;
    dst[1] = q1;
    dst[2] = q2;
    dst[3] = q3;

    state = *(unsigned char *)(arg0 + 6);
    switch (state) {
    case 0:
        *(unsigned char *)(arg0 + 6) = state + 1;
        *(int *)(arg1 + 0x40) = 0x40000000;
        *(char *)(arg1 + 0x83) = 0;
        *(char *)(arg1 + 0x88) = 1;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init(arg0, 0x10, 5.0f, (z = 0.0f));
        return;
    case 1:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = state + 1;
            *(int *)(arg1 + 0x40) = 0x3F800000;
            z = 0.0f;
            anim_clip_init(arg0, 0x11, z, z);
            return;
        }
        return;
    case 2:
        if (*(float *)(arg0 + 0x3C) == 7.0f) {
            func_001FBD50(arg0, 0x7E2, 0, 300.0f);
        }
        *(float *)(arg1 + 0x44) = func_001B1240((float *)(arg0 + 0xB0), D_00810360, D_00810368);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(arg0 + 0xC4), 0.0698131695f);
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            if (func_0021BED0(&D_008102B0) == 0
                && func_0014C130(arg0) != 0
                && func_0011DF78(D_00810354 - *(float *)(arg0 + 0xB4)) <= 6.0f) {
                int s2 = 0;
                int *s3 = sp50;
                do {
                    func_001B2B10(arg0, s3, s3);
                    func_001028B8(s3, s3, arg0 + 0xB0);
                    s2 += 1;
                    s3 += 4;
                } while (s2 < 4);
                if (func_001B1EA0(0, &D_00810360, sp50, 4) != 0) {
                    *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                    func_0021BF90(&D_008102B0, arg0);
                    *(char *)(arg1 + 0x86) = 1;
                    D_00810374 = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
                    *(int *)0x700038A0 = 0xBF000000;
                    *(int *)0x700038A4 = 0;
                    *(int *)0x700038A8 = 0x41893333;
                    *(int *)0x700038AC = 0x3F800000;
                    func_001B2B10(arg0, D_700038A0, D_700038A0);
                    func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
                    func_00182F90(&D_008102B0, D_700038A0);
                    anim_clip_init(arg0, 0x12, 1.0f, (z = 0.0f));
                    return;
                }
            }
            *(unsigned char *)(arg0 + 6) = 4;
            anim_clip_init(arg0, 0x14, 1.0f, (z = 0.0f));
            return;
        }
        break;
    case 3:
        if (*(float *)(arg0 + 0x3C) == 14.0f) {
            *(int *)0x700038A0 = 0x3FC00000;
            *(int *)0x700038A4 = 0x41700000;
            *(int *)0x700038A8 = 0x41700000;
            *(int *)0x700038AC = 0x3F800000;
            func_001B2B10(arg0, D_700038A0, D_700038A0);
            func_001028B8(D_700038A0, arg0 + 0xB0, D_700038A0);
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
            {
                float f3 = *(float *)0x700038A0;
                float f2 = *(float *)0x700038A4;
                float f1 = *(float *)0x700038A8;
                float f0 = *(float *)0x700038AC;
                *(float *)0x700036D0 = f3;
                *(float *)0x700036D4 = f2;
                *(float *)0x700036D8 = f1;
                *(float *)0x700036DC = f0;
            }
            func_001EFEB0(0x8000002A, D_700036A0);
            if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
                if (D_0081070A != 0) {
                    D_008104D4 = 0x41C80000;
                    v0 = 0x42340000;
                } else {
                    D_008104D4 = 0x41A00000;
                    v0 = 0x42200000;
                }
            } else if (D_0081070A != 0) {
                D_008104D4 = 0x41A00000;
                v0 = 0x420C0000;
            } else {
                D_008104D4 = 0x41900000;
                v0 = 0x420C0000;
            }
            D_008104DC = v0;
            D_008102B0 |= 2;
            *(char *)(arg1 + 0x86) = 0;
            func_0021C040(&D_008102B0, arg0);
            if (D_008106BD == 1) {
                D_008106BD = 0;
            }
        }
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init(arg0, 0x13, 1.0f, 0.0f);
            return;
        }
        break;
    case 4:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x88) = 0;
            *(int *)(arg1 + 0x40) = 0x3F800000;
        }
        break;
    }
}
