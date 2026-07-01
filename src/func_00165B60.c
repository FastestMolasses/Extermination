// NEARMISS func_00165B60  (vram 0x00165B60, 0x770 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.54% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/control-flow/data 100% byte-faithful except D_00275B40 (a pointer global near $gp): the real linked binary accesses it via %gp_rel(D_00275B40)($gp), but a per-translation-unit standalone recompile against this stub source always lowers it to lui/lw absolute addressing regardless of -sdatathr...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Entity think-state dispatcher on state byte e[6]. Case 0 checks the current
// world/camera mode (D_00810700==0xD) and whether the camera pos (e+0xB0..0xB8)
// is within a fixed box; if so snaps state to 0xA and sets e[0x1F0]=0x15,
// otherwise advances the state and plays a fallback anim (0xE3/0xE4) depending
// on e[0x1F0]. Case 1 waits for input bit 0x8000. Case 2 is the big body: builds
// a rotation/translation matrix from e+0xD0/e+0xB0/e+0xC0, drives sub-state
// machines on e[7] / e+0x28 to step a camera interpolation, and starts an anim
// clip (D_002754D0) + sound (0x1FBD50). Case 10 bumps state and kicks off clip
// 0x70. Case 11/12 seed a fixed camera pose (e+0xB0..0xB8 = 1021.9/187.2/836.3)
// or restart clip D_002754D0. At the end, if D_00810700==2, calls func_00176DC0.
extern void *build_trs_matrix(void *mtx, void *pos, void *rot, void *scale);
extern void func_001026A0(void *dst, void *src, void *xf);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_00102918(void *dst, void *a, void *b);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern int func_001031E0(void *dst, void *src, unsigned char n);
extern float func_0011DF78(float);
extern int func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern void func_00176DC0(unsigned char *e);
extern void func_0017FC80(unsigned char *e, float f);
extern int func_00180300(unsigned char *e, void *m, unsigned char b);
extern void func_00182A70(void *e);
extern int func_00199FA0(char *arg0, char *arg1);
extern float func_001B1470(float a);
extern void func_001FBD50(unsigned char *e, int id, int b, float f);

extern short D_002754D0;
extern int *D_00275B40;
extern unsigned char D_00810700;
extern int D_700038A0;
extern int D_700038B0;

void func_00165B60(unsigned char *e) {
    unsigned char st;
    unsigned char v1;
    float sp20[4];
    float sp30[4];

    st = e[6];
    switch (st) {
    case 0:
        if (D_00810700 == 0xD && *(float *)(e + 0xB0) >= 1010.0f && *(float *)(e + 0xB0) <= 1030.0f
            && *(float *)(e + 0xB4) >= 170.0f && *(float *)(e + 0xB4) <= 180.0f
            && *(float *)(e + 0xB8) >= 830.0f && *(float *)(e + 0xB8) <= 850.0f) {
            e[6] = 0xA;
            e[7] = 0;
            v1 = 0x15;
            e[0x1F0] = v1;
        } else {
            e[6] = *(volatile unsigned char *)(e + 6) + 1;
            e[7] = 0;
            if (e[0x1F0] == 0x15) {
                func_001749A0(e, 0xE3, 0, 8.0f);
            } else {
                func_001749A0(e, 0xE4, 0, 8.0f);
                *(short *)(e + 0x28) = 0;
            }
        }
        break;
    case 1:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            e[6] = st + 1;
        }
        break;
    case 2:
        if (*(int *)(e + 0x200) & 0x1000) {
            if (e[0x1F0] == 0x16) {
                *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
            }
            func_001029C0(e + 0xD0);
            func_00102C58(e + 0xD0, e + 0xD0, e + 0xC0);
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0xC1266666;
            *(int *)0x700038A8 = 0x40C9999A;
            *(int *)0x700038AC = 0;
            func_001026A0(&D_700038B0, e + 0xD0, &D_700038A0);
            func_001028B8(e + 0xB0, (void *)(*(int *)((char *)D_00275B40 + 4) + 0xC0), &D_700038B0);
            *(int *)(e + 0xBC) = 0x3F800000;
            func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
            if (e[0x1F0] == 0x16) {
                *(float *)(e + 0xB4) = *(float *)(e + 0x294);
            }
            e[0x2F1] = 0;
            func_001749A0(e, D_002754D0, 0, 0.0f);
            func_0017FC80(e, 16.0f);
            e[5] = 0xC;
            e[6] = 0;
            e[0x1F0] = 0x17;
        } else if (e[0x1F0] == 0x16) {
            switch (e[7]) {
            case 0:
                if (*(float *)(e + 0x3C) <= 25.0f) {
                    func_001031E0(&D_700038A0, (void *)(*(int *)((char *)D_00275B40 + 4) + 0xC0), st);
                    *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
                    build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                    if (func_00180300(e, &D_700038A0, 0) == 0 && func_00199FA0((char *)sp20, (char *)sp30) != 0) {
                        *(float *)0x70003A20 = func_0011DF78(sp30[1] - sp20[1]);
                        *(float *)0x70003A20 = *(float *)0x70003A20 - 16.0f;
                        do {
                            *(float *)0x70003A20 = *(float *)0x70003A20 - 3.0f;
                        } while (!(*(float *)0x70003A20 < 3.0f));
                        e[7] = e[7] + 1;
                        *(float *)(e + 0x294) = *(float *)(e + 0xB4) - (16.0f + *(float *)0x70003A20);
                        *(float *)(e + 0x2E4) = *(float *)0x70003A20 / (*(float *)(e + 0x3C) - 1.0f);
                    }
                    *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
                }
                break;
            case 1:
                *(float *)(e + 0xB4) -= *(float *)(e + 0x2E4);
                break;
            }
        }
        if (e[0x1F0] == 0x16) {
            switch (*(short *)(e + 0x28)) {
            case 0:
                if (*(float *)(e + 0x3C) <= 63.0f) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    func_00182A70(e);
                }
                break;
            case 1:
                if (*(float *)(e + 0x3C) <= 52.0f) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    func_00182A70(e);
                }
                break;
            case 2:
                if (*(float *)(e + 0x3C) <= 20.0f) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    func_00182A70(e);
                }
                break;
            case 3:
                if (*(float *)(e + 0x3C) <= 2.0f) {
                    *(short *)(e + 0x28) = *(short *)(e + 0x28) + 1;
                    func_00182A70(e);
                    func_001FBD50(e, 0x107, 0, 300.0f);
                }
                break;
            }
        } else {
            switch (e[7]) {
            case 0:
                if (*(float *)(e + 0x3C) <= 24.0f) {
                    e[7] = e[7] + 1;
                    func_00182A70(e);
                }
                break;
            case 1:
                if (*(float *)(e + 0x3C) <= 2.0f) {
                    e[7] = e[7] + 1;
                    func_00182A70(e);
                    func_001FBD50(e, 0x107, 0, 300.0f);
                }
                break;
            case 2:
                break;
            }
        }
        break;
    case 10:
        e[6] = st + 1;
        func_001749A0(e, 0x70, 0, 1.0f);
        break;
    case 11:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            *(float *)(e + 0xB0) = 1021.9f;
            *(float *)(e + 0xB4) = 187.2f;
            *(float *)(e + 0xB8) = 836.3f;
            func_001749A0(e, 0xFE, 0, 0.0f);
        }
        break;
    case 12:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[0x2F1] = 0;
            func_001749A0(e, D_002754D0, 0, 0.0f);
            func_0017FC80(e, 16.0f);
            e[5] = 0xC;
            e[6] = 0;
            v1 = 0x17;
            e[0x1F0] = v1;
        }
        break;
    }
    if (D_00810700 == 2) {
        func_00176DC0(e);
    }
}
