// NEARMISS func_001486E0  (vram 0x001486E0, 0x458 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Full 4-state logic/structure recovered. Matching wave 2026-10-27 fixes: func_001B3C30 declared with its real pointer parameters (no int casts; restores the argument setup order), and the spad copy block uses the twin's separately declared t0..t3 with volatile reads (f3..f0 load-all-then-store-all). Remaining differences: (1) the 0.8f/1.0f scale constants: the target materialises 0.8f into v0 after the first copy load and 1.0f only just before its store, ours hoists both (v1/v0); (2) in the D_0081070A == 0 arm the target sets up 15.0f (f13) before 200.0f (f12), speculating its constant into the branch delay slot (same residual as func_0013A3B0).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Per-state animation/clip driver for entity arg0 (state byte at arg0+6), companion object
// arg1. State 0: advance the state, set arg1+0x88=1, zero arg1+0x50/0x4C, kick clip 0xD via
// anim_clip_init(arg0, 0xD, 5.0f, 0.0f). State 1: once arg1+0x20 bit 0x1000 is set, advance
// the state, derive three RNG-seeded fields at arg1+0x28/0x2C/0x30 from func_00122BB8(), and
// start clip 0xE. State 2: while arg1+0x20 bit 0x1000 is set, count down arg1+0x30 driving
// clip 0xE, then on expiry advance the state and start clip 0xF; otherwise (bit clear), two
// independent 10-tick timers at arg1+0x28 and arg1+0x2C each drive a quaternion-slerp/scale
// effect: on expiry, reset the timer to 0xA, build a rotation from the bone matrix at
// arg0+0x17C(+0x90) or arg0+0x180(+0x90) via copy_qw4/func_001026A0 into the shared scratch
// quat D_700036A0/D_700038A0, copy the result into D_700036D0, set a uniform 0.8f/0.8f/0.8f/
// 1.0f scale quat D_700038B0, and submit it via func_001B3C30(arg0, D_700036A0, D_700038B0,
// 200.0f, D_0081070A ? 20.0f : 15.0f) followed by func_001FBD50(arg0, 0x7E5, 0, 300.0f).
// State 3: once arg1+0x20 bit 0x1000 is set, reset arg0+5/+6 and arg1+0x38/0x3C/0x88 to 0
// (done).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void copy_qw4(void *d, void *s);
extern void func_001026A0(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern void func_001B3C30(char *arg0, void *arg1, void *arg2, float fparg0, float fparg1);
extern void func_001FBD50(char *p, int a, int b, float f);
extern unsigned char D_0081070A;
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_001486E0(char *arg0, char *arg1) {
    unsigned char st;
    int v;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg1 + 0x88) = 1;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init(arg0, 0xD, 5.0f, 0.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)(arg1 + 0x28) = (func_00122BB8() >> 0x10) & 7;
            *(int *)(arg1 + 0x2C) = (func_00122BB8() >> 0xB) & 7;
            *(int *)(arg1 + 0x30) = (((func_00122BB8() >> 0xD) & 1) + 1) & 0xFFFF;
            anim_clip_init(arg0, 0xE, 1.0f, 0.0f);
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            v = *(int *)(arg1 + 0x30);
            if (v != 0) {
                *(int *)(arg1 + 0x30) = v - 1;
                anim_clip_init(arg0, 0xE, 1.0f, 0.0f);
                break;
            }
            *(unsigned char *)(arg0 + 6) = st + 1;
            anim_clip_init(arg0, 0xF, 1.0f, 0.0f);
            break;
        }
        v = *(int *)(arg1 + 0x28);
        if (v != 0) {
            *(int *)(arg1 + 0x28) = v - 1;
        } else {
            *(int *)(arg1 + 0x28) = 0xA;
            copy_qw4(D_700036A0, *(char **)(arg0 + 0x17C) + 0x90);
            *(float *)0x700038A0 = 11.0f;
            *(int *)0x700038A4 = 0;
            *(int *)0x700038A8 = 0;
            *(float *)0x700038AC = 1.0f;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            {
                float t0, t1, t2, t3;
                t3 = *(volatile float *)0x700038A0;
                t2 = *(volatile float *)0x700038A4;
                t1 = *(volatile float *)0x700038A8;
                t0 = *(volatile float *)0x700038AC;
                *(float *)0x700036D0 = t3;
                *(float *)0x700036D4 = t2;
                *(float *)0x700036D8 = t1;
                *(float *)0x700036DC = t0;
            }
            *(float *)0x700038B0 = 0.8f;
            *(float *)0x700038B4 = 0.8f;
            *(float *)0x700038B8 = 0.8f;
            *(float *)0x700038BC = 1.0f;
            if (D_0081070A != 0) {
                func_001B3C30(arg0, D_700036A0, D_700038B0, 200.0f, 20.0f);
            } else {
                func_001B3C30(arg0, D_700036A0, D_700038B0, 200.0f, 15.0f);
            }
            func_001FBD50(arg0, 0x7E5, 0, 300.0f);
        }
        v = *(int *)(arg1 + 0x2C);
        if (v != 0) {
            *(int *)(arg1 + 0x2C) = v - 1;
            break;
        }
        *(int *)(arg1 + 0x2C) = 0xA;
        copy_qw4(D_700036A0, *(char **)(arg0 + 0x180) + 0x90);
        *(float *)0x700038A0 = 8.0f;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
        {
            float t0, t1, t2, t3;
            t3 = *(volatile float *)0x700038A0;
            t2 = *(volatile float *)0x700038A4;
            t1 = *(volatile float *)0x700038A8;
            t0 = *(volatile float *)0x700038AC;
            *(float *)0x700036D0 = t3;
            *(float *)0x700036D4 = t2;
            *(float *)0x700036D8 = t1;
            *(float *)0x700036DC = t0;
        }
        *(float *)0x700038B0 = 0.8f;
        *(float *)0x700038B4 = 0.8f;
        *(float *)0x700038B8 = 0.8f;
        *(float *)0x700038BC = 1.0f;
        if (D_0081070A != 0) {
            func_001B3C30(arg0, D_700036A0, D_700038B0, 200.0f, 20.0f);
        } else {
            func_001B3C30(arg0, D_700036A0, D_700038B0, 200.0f, 15.0f);
        }
        func_001FBD50(arg0, 0x7E5, 0, 300.0f);
        break;
    case 3:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(int *)(arg1 + 0x3C) = 0;
            *(int *)(arg1 + 0x38) = 0;
            *(char *)(arg1 + 0x88) = 0;
        }
        break;
    }
}
