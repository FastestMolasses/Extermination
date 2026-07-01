// CFLAGS: -O4,p -sdatathreshold 0
//
// Menu player behavior (docs/FINDINGS.md "The menu player (behavior
// func_0020E6F0)"), state byte at arg0+4. State 0: bind model from the
// PLAYER VARIANT table (D_008104E4/D_00810C60), bone-count/matrix setup,
// bind clip by displayed health D_00810858 (>35 -> 0x1C2, else 0xA), seed
// infection tint +0x80/0x84/0x88, rotation +0xC0/0xC4=pi/0xC8, position
// +0xB0/B4/B8 from the view-matrix columns D_00810610.. State 1: breathe
// ramp +0x38 (1.0<->1.3), re-derive tint (clamped >= -127 on +0x84), wrap
// yaw +0xC4, clip swap-back only (health>35 && +0xB==1 -> clip 0x1C2),
// anim_advance_time + func_0020EC80 publish. States 2/3/default:
// func_001AFF90(self) (free the slot).
// COMPILER: mwcc233
extern void anim_advance_time(char *self, float t);
extern void anim_bone_array_setup(int n);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void bone_init_default_2(char *self, int n);
extern int func_001AF7C0(void);
extern void func_001AFF90(int a0);
extern unsigned char func_001C6150(int v);
extern void func_001CA5F0(char *self, int mode);
extern void func_001CA6E0(char *self, int model);
extern void func_0020EC80(char *self);
extern unsigned char D_008104E4;
extern unsigned char D_00810C60;
extern int D_0028A57C;
extern int D_0028A580;
extern int D_0028A584;
extern int D_0028A588;
extern int D_0028A58C;
extern int D_0028A590;
extern float D_00810610;
extern float D_00810614;
extern float D_00810618;
extern float D_0081061C;
extern float D_00810620;
extern float D_00810624;
extern float D_00810628;
extern float D_0081062C;
extern float D_00810630;
extern float D_00810634;
extern float D_00810638;
extern float D_0081063C;
extern float D_00810858;
extern float D_0081085C;

void func_0020E6F0(char *arg0) {
    unsigned char st;
    int bones;
    int i;
    char *p;
    unsigned char sub;

    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        if (D_008104E4 == 0) {
            sub = D_00810C60;
            switch (sub) {
            case 1:
                func_001CA6E0(arg0, D_0028A588);
                break;
            case 2:
                func_001CA6E0(arg0, D_0028A58C);
                break;
            case 0:
            default:
                func_001CA6E0(arg0, D_0028A57C);
                break;
            }
        } else if (D_008104E4 == 1) {
            sub = D_00810C60;
            switch (sub) {
            case 1:
                func_001CA6E0(arg0, D_0028A588);
                break;
            case 2:
                func_001CA6E0(arg0, D_0028A58C);
                break;
            case 0:
            default:
                func_001CA6E0(arg0, D_0028A590);
                break;
            }
        } else {
            func_001CA6E0(arg0, D_0028A584);
        }

        bones = *(int *)(arg0 + 0x44);
        *(unsigned char *)(arg0 + 0xC) = func_001C6150(bones);

        i = 0;
        p = arg0;
        while (i < (int) *(unsigned char *)(arg0 + 0xC)) {
            *(int *)(p + 0x110) = func_001AF7C0();
            p += 4;
            i += 1;
        }
        bones = *(unsigned char *)(arg0 + 0xC);
        *(unsigned char *)(arg0 + 9) = (unsigned char) bones;
        anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
        *(int *)(arg0 + 0x40) = D_0028A580;

        if (!(D_00810858 <= 35.0f)) {
            bone_init_default_2(arg0, 0x1C2);
            *(unsigned char *)(arg0 + 0xB) = 0;
        } else {
            bone_init_default_2(arg0, 0xA);
            *(unsigned char *)(arg0 + 0xB) = 1;
        }
        func_001CA5F0(arg0, 0xB);

        *(float *)(arg0 + 0x80) = 0.01f * (-80.0f * D_0081085C);
        *(float *)(arg0 + 0x84) = 0.01f * (-100.0f * D_0081085C);
        *(float *)(arg0 + 0x88) = 0.01f * (-30.0f * D_0081085C);
        *(int *)(arg0 + 0x8C) = 0;
        *(float *)(arg0 + 0x38) = 1.0f;
        *(int *)(arg0 + 0xC0) = 0;
        *(float *)(arg0 + 0xC4) = 3.1415927f;
        *(int *)(arg0 + 0xC8) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        *(float *)(arg0 + 0xB0) = D_0081061C;
        *(float *)(arg0 + 0xB4) = D_0081062C;
        *(float *)(arg0 + 0xB8) = D_0081063C;
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + 40.0f * D_00810618;
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 40.0f * D_00810628;
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + 40.0f * D_00810638;
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + 7.4f * D_00810610;
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 7.4f * D_00810620;
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + 7.4f * D_00810630;
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + 2.4f * D_00810614;
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 2.4f * D_00810624;
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + 2.4f * D_00810634;
        return;
    case 1:
        if (*(unsigned char *)(arg0 + 5) == 0) {
            float k = 0.01f;
            *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) + k;
            if (!(*(float *)(arg0 + 0x38) < 1.3f)) {
                *(unsigned char *)(arg0 + 5) = 1;
            }
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - 0.01f;
            if (*(float *)(arg0 + 0x38) <= 1.0f) {
                *(unsigned char *)(arg0 + 5) = 0;
            }
        }
        *(float *)(arg0 + 0x80) = 0.01f * (-80.0f * D_0081085C) * *(float *)(arg0 + 0x38);
        *(float *)(arg0 + 0x84) = 0.01f * (-100.0f * D_0081085C) * *(float *)(arg0 + 0x38);
        if (*(float *)(arg0 + 0x84) < -127.0f) {
            *(float *)(arg0 + 0x84) = -127.0f;
        }
        *(float *)(arg0 + 0x88) = 0.01f * (-30.0f * D_0081085C) * *(float *)(arg0 + 0x38);
        {
            float k2 = 0.01f;
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0xC4) + k2;
        }
        if (!(*(float *)(arg0 + 0xC4) <= 3.1415927f)) {
            *(volatile float *)(arg0 + 0xC4) = *(volatile float *)(arg0 + 0xC4) - 6.2831855f;
        }
        if (!(D_00810858 <= 35.0f)) {
            if (*(unsigned char *)(arg0 + 0xB) == 1) {
                anim_clip_init(arg0, 0x1C2, 16.0f, 0.0f);
                *(unsigned char *)(arg0 + 0xB) = 0;
            }
        }
        anim_advance_time(arg0, 1.0f);
        func_0020EC80(arg0);
        return;
    case 2:
    case 3:
    default:
        func_001AFF90((int) arg0);
        return;
    }
}
