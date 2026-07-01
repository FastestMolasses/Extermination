// NEARMISS func_00134500  (vram 0x00134500, 0x3D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 96.82% (233) / 90.79% (991202) -- essentially body-complete. Two residual mwcc scheduling artifacts, neither fixable from source: (1) idiom-13-adjacent -- 2.3.3 speculatively turns the arg1+4 decrement/store into a `bnezl` (branch-likely) with the subtract filled into its own delay slot,...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_init(unsigned char *self, int clip, float a, float b);
extern int func_00122BB8(void);
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B1470(float a);
extern float func_001B12B0(float a, float b, float c);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *a0, void *a1, float b);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028B8(void *a0, void *a1, void *a2);
extern float func_0011DF78(float a);
extern float D_00810360;
extern float D_00810368;
extern unsigned char D_00245190;
extern float D_700036A0;
extern float D_700038A0;
extern float D_700038B0;

void func_00134500(unsigned char *arg0, unsigned char *arg1) {
    unsigned char state;
    float angle_scale;

    state = *(unsigned char *)(arg0 + 6);
    angle_scale = 2.0f;

    switch (state) {
    case 0:
        *(unsigned char *)(arg0 + 6) = state + 1;
        *(int *)(arg1 + 0x2C) = 0x3ECCCCCD;
        *(int *)(arg1 + 4) = ((func_00122BB8() >> 0x11) & 0x7F) + 0x3C;
        *(int *)(arg1 + 8) = 0;
        anim_clip_init(arg0, 1, 5.0f, 0.0f);
        if ((func_00122BB8() >> 0xE) & 3) {
            *(float *)(arg1 + 0x24) = func_001B1470(6.2831855f * ((float)((func_00122BB8() >> 0x11) & 0xFF) / 255.0f));
        } else {
            *(float *)(arg1 + 0x24) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
        }
        break;
    case 1:
        if (*(int *)(arg1 + 4) == 0) {
            *(unsigned char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            break;
        }
        {
            *(int *)(arg1 + 4) = *(int *)(arg1 + 4) - 1;
            if (*(signed char *)(arg1 + 0x5C) != 0) {
                float delta;

                *(int *)0x700038A0 = 0x43AA0000;
                *(int *)0x700038A4 = 0;
                *(int *)0x700038A8 = 0x43870000;
                *(int *)0x700038AC = 0x3F800000;
                *(float *)0x70003A20 = func_001B1470(3.1415927f + func_001B1240(&D_700038A0, D_00810360, D_00810368));
                delta = func_001B1470(func_001B1240(&D_700038A0, *(float *)(arg0 + 0xB0), *(float *)(arg0 + 0xB8)) - *(float *)0x70003A20);
                *(float *)0x70003A24 = delta;
                if (!(func_0011DF78(delta) <= 0.7853982f)) {
                    angle_scale = 4.0f;
                    func_001029C0(&D_700036A0);
                    func_00102BB0(&D_700036A0, &D_700036A0, *(float *)0x70003A20);
                    *(int *)0x700038B0 = 0;
                    *(int *)0x700038B4 = 0;
                    *(int *)0x700038B8 = 0x42780000;
                    *(int *)0x700038BC = 0x3F800000;
                    func_001026A0(&D_700038B0, &D_700036A0, &D_700038B0);
                    func_001028B8(&D_700038A0, &D_700038A0, &D_700038B0);
                    *(float *)(arg1 + 0x24) = func_001B1240(arg0 + 0xB0, *(float *)0x700038A0, *(float *)0x700038A8);
                } else if (*(short *)(arg1 + 0x50) == 0) {
                    *(unsigned char *)(arg0 + 5) = *(&D_00245190 + ((func_00122BB8() >> 0xE) & 0xF));
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(int *)(arg1 + 0x2C) = 0;
                }
            } else {
                if (*(int *)(arg1 + 8) != 0) {
                    *(int *)(arg1 + 8) = *(int *)(arg1 + 8) - 1;
                } else {
                    *(int *)(arg1 + 8) = (func_00122BB8() >> 0x11) & 0xF;
                    *(float *)(arg1 + 0x24) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
                }
                if (*(short *)(arg1 + 0x50) == 0) {
                    if (*(float *)(arg1 + 0x34) < 50.0f) {
                        if ((func_00122BB8() >> 0x13) & 1) {
                            *(unsigned char *)(arg0 + 5) = 6;
                        } else {
                            *(unsigned char *)(arg0 + 5) = 5;
                        }
                    } else {
                        *(unsigned char *)(arg0 + 5) = 6;
                    }
                    *(unsigned char *)(arg0 + 6) = 0;
                }
            }
        }
        break;
    }

    *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x24), *(float *)(arg0 + 0xC4), (3.1415927f * angle_scale) / 180.0f);
}
