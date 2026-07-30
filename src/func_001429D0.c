// NEARMISS func_001429D0  (vram 0x001429D0, 0xAEC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.68% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.68% (22 of ~700 instrs). Dispatch itself is byte-identical except register colouring; jtbl reloc is NOT a residual (matched). Residuals, all mwcc-vs-CW allocator/scheduler: (1) rows 5/8/9/10/12 prologue: mwcc emits `paddub s0,a1` before `paddub s1,a0` (ascending s-reg) and therefore colours th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: behaviour handler #3 for an enemy actor, called as (e, d) from the
// per-actor dispatch in func_00142070 (e = actor record, d = its AI/behaviour
// record).  e[6] is the handler's own state (0..5) and selects the body through
// a 6-entry jump table; states >= 6 do nothing.
//   e[5]    = behaviour id -- writing it here hands the actor to another handler
//   e[6]    = this handler's state (0..5)
//   e[7]    = sub-state used only by state 5
//   e[0x2C] = animation flag word (short); (x & 0xFFFF7FFF) == 8 means "busy"
//   e[0x3C] = animation time remaining
//   e[0xB0] = actor position vec3
//   e[0xC4] = actor facing angle (radians)
//   d[0x20] = frame counter for state 3 (gives up after 300 frames)
//   d[0x24] = "keep current heading" countdown
//   d[0x28] = "don't re-pick a heading" countdown
//   d[0x30] = animation event flags; bit 0x1000 = clip finished
//   d[0x40] = animation playback rate
//   d[0x44] = desired heading
//   d[0x48] = state-5 timer
//   d[0x4C] = movement speed, d[0x50] = its per-frame delta (deceleration)
//   d[0x5C] = distance to the tracked world point (D_00810350)
//   d[0x62] = "chase" flag (short); cleared when the player is close and calm
//   d[0x74] = hit/contact flags; bit 2 = took a hit
//   d[0x75] = cleared on entry
//   d[0x79] = turn direction for state 3
//   d[0x7A] = "handler wants the heavy path" flag
//   d[0x7F] = alert flag; bit 0x80 = forced alert
// Angles are wrapped with func_001B1470 and interpolated with func_001B12B0;
// func_001B1240 is atan2-to-target, func_001B13F0 a distance test, func_001B1560
// a cone/visibility test and func_0019AD00 a movement/collision probe.  The
// 0x7000xxxx addresses are EE scratchpad scratch vectors.
// Fields written through `int *` hold IEEE-754 float bit patterns; that is how
// the original stored them (GPR store, no swc1).

extern int   func_001464B0(unsigned char *);
extern int   func_00122BB8(void);
extern float func_001B1240(float *, float, float);
extern float func_001B12B0(float, float, float);
extern void  func_00102948(float *, float *);
extern int   func_001B13F0(float *, float *, float);
extern void  anim_clip_init(unsigned char *, int, float, float);
extern int   func_001B1560(unsigned char *, float *, float);
extern float func_001B1470(float);
extern float func_0011DF78(float);
extern void  func_001B2B10(unsigned char *, float *, float *);
extern void  func_001028B8(float *, float *, float *);
extern int   func_0019AD00(unsigned char *, float *, int);
extern int   func_00146AF0(unsigned char *, unsigned char *, float);
extern int   func_001B39F0(unsigned char *, float *, float *);
extern void  func_001FBD50(unsigned char *, int, int, float);
extern void  func_001029C0(float *);
extern void  func_00102BB0(float *, float *, float);
extern void  func_001026A0(float *, float *, float *);

extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_00810350[];
extern float D_00810360[];
extern float D_00810368[];
extern float D_00810374[];
extern unsigned char D_0081050C;

void func_001429D0(unsigned char *e, unsigned char *d)
{
    int r;
    int t;
    float cur;
    float cur2;

    switch (e[6]) {
    case 0:
        e[6] = *(volatile unsigned char *)(e + 6) + 1;
        d[0x75] = 0;
        *(int *)(d + 0x4C) = 0;
        *(int *)(d + 0x50) = 0;
        *(float *)(d + 0x44) = *(float *)(e + 0xC4);
        anim_clip_init(e, 4, 0.0f, 0.0f);
        break;
    case 1:
        if (*(float *)(e + 0x3C) <= 14.0f) {
            *(int *)(d + 0x4C) = 0x3F800000;
        }
        if (*(int *)(d + 0x30) & 0x1000) {
            e[6] = e[6] + 1;
            *(int *)(d + 0x24) = 0;
            *(int *)(d + 0x28) = 0;
            *(int *)(d + 0x4C) = 0x3F933333;
            anim_clip_init(e, 5, 0.0f, 0.0f);
        }
        break;
    case 2:
        switch (func_001464B0(e)) {
        case 0:
            if (*(signed char *)(d + 0x7F) != 0) {
                t = *(int *)(d + 0x28);
                if (t != 0) {
                    *(int *)(d + 0x28) = t - 1;
                } else {
                    *(int *)(d + 0x28) = (func_00122BB8() >> 9) & 3;
                    *(float *)(d + 0x44) = func_001B1240((float *)(e + 0xB0),
                                                         *(float *)(d + 0x10),
                                                         *(float *)(d + 0x18));
                    *(volatile float *)0x70003A20 =
                        0.043633234f +
                        (0.06981317f * ((float)((func_00122BB8() >> 14) & 0xFF) / 255.0f));
                    cur = *(float *)(e + 0xC4);
                    *(float *)(e + 0xC4) = func_001B12B0(*(float *)(d + 0x44), cur,
                                                         *(volatile float *)0x70003A20);
                }
                func_00102948(D_700038A0, (float *)(e + 0xB0));
                func_00102948(D_700038B0, (float *)(d + 0x10));
                *(volatile int *)0x700038B4 = 0;
                *(volatile int *)0x700038A4 = 0;
                if (func_001B13F0(D_700038A0, D_700038B0, 45.0f) != 0 ||
                    (*(signed char *)(d + 0x7F) & 0x80)) {
                    e[6] = 4;
                    *(int *)(d + 0x40) = 0x40000000;
                    *(int *)(d + 0x50) = 0xBD4CCCCD;
                    anim_clip_init(e, 6, 0.0f, 0.0f);
                }
                break;
            }
            if (*(float *)(d + 0x5C) <= 40.0f &&
                func_001B1560(e, D_00810360, 0.7853982f) != 0) {
                if (*(float *)(d + 0x5C) <= 15.0f) {
                    if (D_0081050C == 3 &&
                        func_0011DF78(func_001B1470(D_00810374[0] - *(float *)(e + 0xC4)))
                            <= 0.7853982f) {
                        *(volatile int *)0x700038A0 = 0;
                        *(volatile int *)0x700038A4 = 0x41700000;
                        *(volatile int *)0x700038A8 = 0x420E0000;
                        *(volatile int *)0x700038AC = 0x3F800000;
                        func_001B2B10(e, D_700038A0, D_700038A0);
                        func_001028B8(D_700038A0, (float *)(e + 0xB0), D_700038A0);
                        if (func_0019AD00(e, D_700038A0, 7) == 0) {
                            e[5] = 7;
                            e[6] = 0;
                            break;
                        }
                    }
                } else if (!(*(float *)(d + 0x5C) < 35.0f)) {
                    if (!(func_0011DF78(func_001B1470(D_00810374[0] - *(float *)(e + 0xC4)))
                            <= 0.7853982f)) {
                        e[5] = 8;
                        e[6] = 0;
                        break;
                    }
                } else if (D_0081050C == 0 && *(float *)(d + 0x5C) <= 27.0f) {
                    *(short *)(d + 0x62) = 0;
                }
            }
            if (*(short *)(d + 0x62) == 0) {
                e[6] = 4;
                *(int *)(d + 0x40) = 0x40000000;
                *(int *)(d + 0x50) = 0xBD4CCCCD;
                anim_clip_init(e, 6, 0.0f, 0.0f);
                break;
            }
            if (*(signed char *)(d + 0x7F) == 0 &&
                !(*(float *)(d + 0x5C) <= 23.0f) &&
                *(float *)(e + 0xC4) == func_001B1240((float *)(e + 0xB0),
                                                      D_00810360[0], D_00810368[0]) &&
                func_00146AF0(e, d, 13.0f) != 0) {
                *(int *)(d + 0x24) = ((func_00122BB8() >> 17) & 0xF) + 10;
                *(int *)(d + 0x28) = 0;
                *(volatile int *)0x700038A0 = 0;
                *(volatile int *)0x700038A4 = 0;
                *(volatile int *)0x700038A8 = 0x41C80000;
                *(volatile int *)0x700038AC = 0x3F800000;
                func_001B2B10(e, D_700038A0, D_700038A0);
                if (func_001B39F0(e, D_700038A0, D_700038B0) != 0) {
                    *(float *)(d + 0x44) = func_001B1470(1.5707964f + *(float *)(d + 0x44));
                } else {
                    *(float *)(d + 0x44) = func_001B1470(*(float *)(d + 0x44) - 1.5707964f);
                }
            }
            t = *(int *)(d + 0x24);
            if (t != 0) {
                *(int *)(d + 0x24) = t - 1;
            } else {
                *(float *)(d + 0x44) = func_001B1240((float *)(e + 0xB0),
                                                     D_00810360[0], D_00810368[0]);
            }
            t = *(int *)(d + 0x28);
            if (t != 0) {
                *(int *)(d + 0x28) = t - 1;
            } else if (*(float *)(d + 0x5C) < 20.0f) {
                *(int *)(d + 0x28) = (func_00122BB8() >> 9) & 0x7F;
            } else {
                *(volatile float *)0x70003A20 =
                    0.017453292f +
                    (0.06981317f * ((float)((func_00122BB8() >> 14) & 0xFF) / 255.0f));
                cur2 = *(float *)(e + 0xC4);
                *(float *)(e + 0xC4) = func_001B12B0(*(float *)(d + 0x44), cur2,
                                                     *(volatile float *)0x70003A20);
            }
            break;
        case 1:
            e[6] = 3;
            *(int *)(d + 0x20) = 0;
            break;
        case 2:
            e[6] = 5;
            e[7] = 0;
            d[0x7A] = 1;
            *(int *)(d + 0x48) = 0x40066666;
            func_001FBD50(e, 0x831, 0, 300.0f);
            anim_clip_init(e, 7, 0.0f, 17.0f);
            break;
        case 3:
            e[6] = 4;
            *(int *)(d + 0x40) = 0x40000000;
            *(int *)(d + 0x50) = 0xBD4CCCCD;
            anim_clip_init(e, 6, 0.0f, 0.0f);
            break;
        }
        break;
    case 3:
        r = *(int *)(d + 0x20) + 1;
        *(int *)(d + 0x20) = r;
        if ((unsigned int)r >= 300U) {
            e[6] = 4;
            *(int *)(d + 0x50) = 0xBD23D70A;
            anim_clip_init(e, 6, 0.0f, 0.0f);
            break;
        }
        if (*(signed char *)(d + 0x79) != 0) {
            *(volatile int *)0x70003A20 = 0xBEC90FDB;
            *(float *)(e + 0xC4) = func_001B1470(0.06981317f + *(float *)(e + 0xC4));
        } else {
            *(volatile int *)0x70003A20 = 0x3EC90FDB;
            *(float *)(e + 0xC4) = func_001B1470(*(float *)(e + 0xC4) - 0.06981317f);
        }
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(e + 0xC4));
        *(volatile int *)0x700038A0 = 0;
        *(volatile int *)0x700038A4 = 0x40400000;
        *(volatile int *)0x700038A8 = 0x42060000;
        *(volatile int *)0x700038AC = 0x3F800000;
        func_001026A0(D_700038B0, D_700036A0, D_700038A0);
        func_001028B8(D_700038B0, D_700038B0, (float *)(e + 0xB0));
        if (func_0019AD00(e, D_700038B0, 6) == 0) {
            func_00102BB0(D_700036A0, D_700036A0, *(volatile float *)0x70003A20);
            func_001026A0(D_700038B0, D_700036A0, D_700038A0);
            func_001028B8(D_700038B0, D_700038B0, (float *)(e + 0xB0));
            if (func_0019AD00(e, D_700038B0, 6) == 0) {
                e[6] = 2;
                *(int *)(d + 0x20) = 0;
            }
        }
        break;
    case 4:
        if (*(float *)(d + 0x4C) < 0.0f) {
            *(int *)(d + 0x4C) = 0;
            *(int *)(d + 0x50) = 0;
        }
        if (*(int *)(d + 0x30) & 0x1000) {
            *(int *)(d + 0x40) = 0x3F800000;
            if (func_001B1560(e, D_00810350, 0.7853982f) != 0 &&
                func_001B13F0(D_00810360, (float *)(e + 0xB0), 20.0f) != 0) {
                e[5] = 6;
                e[6] = 0;
                break;
            }
            e[5] = 0;
            e[6] = 0;
            *(int *)(d + 0x4C) = 0;
            *(int *)(d + 0x50) = 0;
        }
        break;
    case 5:
        d[0x7A] = 1;
        switch (e[7]) {
        case 0:
            if (*(float *)(d + 0x48) <= 0.0f) {
                e[7] = e[7] + 1;
                *(int *)(d + 0x40) = 0x3F800000;
                anim_clip_init(e, 9, 0.0f, 0.0f);
                break;
            }
            if ((*(short *)(e + 0x2C) & 0xFFFF7FFF) != 8 &&
                (*(int *)(d + 0x30) & 0x1000)) {
                anim_clip_init(e, 8, 0.0f, 0.0f);
            }
            break;
        case 1:
            if (*(float *)(e + 0x3C) <= 32.0f) {
                *(int *)(d + 0x40) = 0;
            }
            if (*(signed char *)(d + 0x74) & 2) {
                e[6] = 2;
                e[7] = 0;
                *(int *)(d + 0x40) = 0x3F800000;
                func_001FBD50(e, 0x832, 0, 300.0f);
                anim_clip_init(e, 5, 5.0f, 20.0f);
            }
            break;
        }
        break;
    }
}
