// NEARMISS func_001F3620  (vram 0x001F3620, 0x80C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.92% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single register-allocation swap (a0<->v1) on one 7-instruction pair: in the D_00275C44 sound-reseed block, the target colors the modulo divisor/result into $a0 and the mute-byte (0x70003B8D) temp into $v1; mine colors them v1/a0. Everything else byte-identical (99.92% = 7 mismatched rows / ~514 i...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Debris/particle piece update state machine (called per-entity from matched
// func_001F40C0 with t = the entity's +0x82 type id). p+0x70 = state (0..3),
// p+0x74 = frame countdown, p+0x7C = kill request, p+0x80 = dead short,
// p+0x84 = scale/alpha, p+0x10 = velocity vec (+0x14 y), p+0x20 = quat,
// p+0x30/34/38 = spin rates, p+0x40 = color 0.6f, p+0x60 = home transform.
// State 0 (tumble-in): per type-group jitter the quat by a random-axis wobble of
// 8deg (types 0/1/5/6/4), 4deg (17/18/22, plus water-splash: below y=10 spawns
// effect 0x8000005F at (x,0,z) via func_001EFD20, kills the piece, every 2nd
// splash plays sound 0xDB at 800 range), or a fixed (0,-0.5,0) tilt (type 7);
// counts down p+0x74, entering state 1 at 0.
// State 1 (settle): if kill-requested -> dead. Types 2/10/23: re-home
// (func_00102948), color=0.6f, spin*=(2+rand), state 2, recurse. Types 3/7/8/9:
// velocity=(0.1,?,0)*random-yaw (func_00102BB0 rotate by rand*360deg),
// vy=min(1, -0.4*vy), 45-frame timer, state 3, and (rate-limited by gp-rel
// D_00275C44 = rand%20+30, muted by byte 0x70003B8D) plays a per-type clatter
// sound (0x16A/0x5E0/0x5DE at 200). Other types: dead.
// State 2 (spin-out): rebuild matrix from home quat (func_001CD390/func_001031E0),
// scale spin by fading p+0x84 (-0.01/frame; <0 -> dead).
// State 3 (bounce): 16deg wobble, fade after 20 frames, countdown -> dead.
// States 0/3 (ret=1) then integrate: quat->mat3, scale rows by spin*fade,
// translate by velocity (func_001028B8), and apply per-type gravity from table
// D_0025A398 (stride 0x60) to vy.
//
// NEARMISS 99.92% (mwcc233; 991202 = 84.66%) - logic fully recovered; sole
// residual is one a0<->v1 register-allocation swap in the D_00275C44 reseed
// block (modulo temps vs mute-byte temp).

extern int func_00122BB8(void);
extern void func_001CA3B0(char *q, float x, float y, float z);
extern void func_001CA4D0(char *dst, char *a, char *b);
extern void func_001EFD20(int id, char *v);
extern void func_001F02C0(char *v, int id, float f);
extern void func_00102948(char *p, char *v);
extern void func_001029C0(char *m);
extern void func_00102BB0(char *dst, char *m, float ang);
extern void func_001026A0(char *dst, char *m, char *v);
extern void func_001CD390(char *m, char *q);
extern void func_001031E0(char *m, char *p);
extern void func_00102900(char *dst, char *v, float s);
extern void quat_to_mat3(char *m, char *q, char *p);
extern void func_001028B8(char *dst, char *a, char *b);

extern char D_700038A0[];
extern char D_700038D0[];
extern char D_700036A0[];
extern char D_700036B0[];
extern char D_700036C0[];
extern char D_700036D0[];
extern char D_0025A398[];
extern int D_00275C44;
extern int D_00275C48;

void func_001F3620(char *p, int t) {
    int ret = 0;

    switch (*(int *)(p + 0x70)) {
    case 0:
        switch (t) {
        case 0:
        case 1:
        case 5:
        case 6:
        case 4:
        {
            float a = (3.14159274f * (8.0f * ((float)func_00122BB8() / 2147483648.0f))) / 180.0f;
            *(float *)0x70003A20 = a;
            func_001CA3B0(D_700038A0, a, a, a);
            func_001CA4D0(p + 0x20, p + 0x20, D_700038A0);
            if (*(int *)(p + 0x74) < 0x14) {
                *(float *)(p + 0x84) = *(float *)(p + 0x84) - 0.05f;
            }
            break;
        }
        case 17:
        case 18:
        case 22:
        {
            float a = (3.14159274f * (4.0f * ((float)func_00122BB8() / 2147483648.0f))) / 180.0f;
            *(float *)0x70003A20 = a;
            func_001CA3B0(D_700038A0, a, a, a);
            func_001CA4D0(p + 0x20, p + 0x20, D_700038A0);
            if (*(float *)(p + 4) < 10.0f) {
                *(float *)0x700038D0 = *(float *)p;
                *(int *)0x700038D4 = 0;
                *(float *)0x700038D8 = *(float *)(p + 8);
                *(int *)0x700038DC = 0x3F800000;
                func_001EFD20(0x8000005F, D_700038D0);
                *(short *)(p + 0x80) = 1;
                D_00275C48 += 1;
                if (D_00275C48 & 1) {
                    func_001F02C0(D_700038D0, 0xDB, 800.0f);
                }
            }
            break;
        }
        case 7:
            func_001CA3B0(D_700038A0, 0.0f, -0.5f, 0.0f);
            func_001CA4D0(p + 0x20, p + 0x20, D_700038A0);
            break;
        }
        *(int *)(p + 0x74) = *(int *)(p + 0x74) - 1;
        if (*(int *)(p + 0x74) <= 0) {
            *(int *)(p + 0x70) = 1;
        }
        ret = 1;
        break;
    case 1:
        if (*(int *)(p + 0x7C) != 0) {
            *(short *)(p + 0x80) = 1;
            ret = 1;
        } else {
            switch (t) {
            case 2:
            case 10:
            case 23:
                func_00102948(p, p + 0x60);
                *(int *)(p + 0x40) = 0x3F19999A;
                *(int *)(p + 0x44) = 0x3F19999A;
                *(int *)(p + 0x48) = 0x3F19999A;
                *(int *)(p + 0x4C) = 0x3F19999A;
                *(float *)(p + 0x30) = *(float *)(p + 0x30) *
                    (2.0f + ((float)func_00122BB8() / 2147483648.0f));
                *(float *)(p + 0x34) = 2.0f * *(float *)(p + 0x30);
                *(int *)(p + 0x38) = 0x3CA3D70A;
                *(int *)(p + 0x70) = 2;
                func_001F3620(p, t);
                break;
            case 3:
            case 7:
            case 8:
            case 9:
            {
                float f;
                *(int *)(p + 0x10) = 0x3DCCCCCD;
                *(int *)(p + 0x18) = 0;
                *(int *)(p + 0x1C) = 0x3F800000;
                func_00102948(p, p + 0x60);
                func_001029C0(D_700036A0);
                func_00102BB0(D_700036A0, D_700036A0,
                    (3.14159274f * (360.0f * ((float)func_00122BB8() / 2147483648.0f))) / 180.0f);
                func_001026A0(p + 0x10, D_700036A0, p + 0x10);
                f = -(0.4f * *(float *)(p + 0x14));
                *(float *)(p + 0x14) = f;
                if (!(f <= 1.0f)) {
                    f = 1.0f;
                }
                *(float *)(p + 0x14) = f;
                *(int *)(p + 0x74) = 0x2D;
                *(int *)(p + 0x70) = 3;
                ret = 1;
                if (D_00275C44 < 0) {
                    int rnd = func_00122BB8();
                    unsigned char m = *(unsigned char *)0x70003B8D;
                    D_00275C44 = (rnd % 20) + 0x1E;
                    if (m == 0) {
                        switch (t) {
                        case 3:
                            func_001F02C0(p, 0x16A, 200.0f);
                            break;
                        case 7:
                            func_001F02C0(p, 0x5E0, 200.0f);
                            break;
                        case 8:
                            func_001F02C0(p, 0x5E0, 200.0f);
                            break;
                        case 9:
                            func_001F02C0(p, 0x5DE, 200.0f);
                            break;
                        }
                    }
                }
                break;
            }
            default:
                *(short *)(p + 0x80) = 1;
                break;
            }
        }
        break;
    case 2:
        func_001CD390(D_700036A0, p + 0x50);
        func_001031E0(D_700036D0, p);
        func_00102900(D_700036A0, D_700036A0, *(float *)(p + 0x30) * *(float *)(p + 0x84));
        func_00102900(D_700036B0, D_700036B0, *(float *)(p + 0x34) * *(float *)(p + 0x84));
        func_00102900(D_700036C0, D_700036C0, *(float *)(p + 0x38));
        {
            float f = *(float *)(p + 0x84) - 0.01f;
            *(float *)(p + 0x84) = f;
            if (f < 0.0f) {
                *(short *)(p + 0x80) = 1;
            }
        }
        break;
    case 3:
    {
        float a = (3.14159274f * (16.0f * ((float)func_00122BB8() / 2147483648.0f))) / 180.0f;
        *(float *)0x70003A20 = a;
        func_001CA3B0(D_700038A0, a, a, a);
        func_001CA4D0(p + 0x20, p + 0x20, D_700038A0);
        if (*(int *)(p + 0x74) < 0x14) {
            *(float *)(p + 0x84) = *(float *)(p + 0x84) - 0.05f;
        }
        *(int *)(p + 0x74) = *(int *)(p + 0x74) - 1;
        if (*(int *)(p + 0x74) <= 0) {
            *(short *)(p + 0x80) = 1;
        }
        ret = 1;
        break;
    }
    }
    if (ret != 0) {
        quat_to_mat3(D_700036A0, p + 0x20, p);
        func_00102900(D_700036A0, D_700036A0, *(float *)(p + 0x30) * *(float *)(p + 0x84));
        func_00102900(D_700036B0, D_700036B0, *(float *)(p + 0x34) * *(float *)(p + 0x84));
        func_00102900(D_700036C0, D_700036C0, *(float *)(p + 0x38) * *(float *)(p + 0x84));
        func_001028B8(p, p, p + 0x10);
        *(float *)(p + 0x14) += *(float *)(D_0025A398 + t * 0x60);
    }
}
