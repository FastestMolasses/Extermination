// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: per-frame damage/stagger driver for actor arg0 with its controller
// block arg1, dispatched on the state byte arg0[5] through jtbl_0026CFE0
// (8 states; index >= 8 falls straight to the shared tail).  arg0+0x34 is the
// remaining stagger/HP short, arg0+0x36 the pending damage word (low byte = the
// amount, bit 0x4000 = play the hurt voice, bit 0x2000 = force the knockdown
// branch), arg0+0x28 a 60-frame voice cooldown and arg0+0x38 the saved move speed.
// mode = func_001C2770(arg0, arg1, 0) is polled once up front; mode 0 runs
// func_001C3D60 in the tail.
//   0 - enter the reaction: stash arg1+0xD8 (move speed) into arg0+0x38 and zero it,
//       fire voice 0x80000027 (arming the 60-frame cooldown) when bit 0x4000 is set,
//       subtract the damage byte from arg0+0x34, clear arg1+0xF4 and
//       func_001FBD50(arg0, 0x1B1, 0, 300.0f).  Then, if bit 0x2000 is set, or
//       arg1[0xFB] bit 7 is set, or arg1+0xE4 is 0 / 0x400 / 0x500, play the
//       knockdown anim 0x1B, halve arg0+0x70 via func_00103230(q, q, 0.5f) with q = arg0+0x70, clear
//       arg0+0xC0, set arg1+0xF0 = 0.8f and arg1+0xE4 = 0x600 and jump to state 5;
//       otherwise play the flinch anim 0x1D and advance one state.  Either way, if
//       arg0+0x34 has run out, clamp it to 0, clear arg1[0xFA] and fire the death
//       event 0x8000000C(arg0+0xB0, arg0+0xC0), else set arg0[0] = 1.  Finally clear
//       the damage word.
//   1 - tick the cooldown down; re-apply a newly posted damage word the same way
//       (dying -> arg0[0] = 3).  When arg1+0xF4 reports 0x1000 (anim finished):
//       clear arg1[0xFA]; if the actor survived, restore it (arg0[4] = 1,
//       state = arg1[0xFB] & 0x7F, clear arg0[6]/arg0[7], hand the saved speed back
//       to arg1+0xD8, arg0[0] = 1); otherwise arg0[0] = 2, clamp arg0+0x34, stage the
//       fall direction (0, -1.4, 0, 1) at 0x700038A0 and pick state 2 or 3 from
//       func_001C25E0(arg0, D_700038A0) and (arg1+0xE4 >> 8) == 1.
//   2 - delegate to func_0012D580(arg0, arg1, mode).
//   3 - play anim 0x20 at 4.0, func_001FBD50(arg0, 0x1B7, 0, 300.0f), arg0[0] = 2,
//       advance and set arg1+0xF0 = 0.06f.
//   4 - while arg0+0x3C < 10.0f, play sound 0x1B5 and either go to state 7 (when
//       event 0x8000000F fires) or set arg0[4] = 3; then func_00129F00.
//   5 - func_001B5360, the same cooldown/damage block as state 1, integrate
//       arg0+0xB0/0xB8 by arg0+0x70/0x78, and advance unless arg1+0xE4 == 0x600.
//   6 - play anim 0x1D at 4.0 and go back to state 1.
//   7 - func_00129F00.
// Tail: func_001C3D60 when mode == 0, arg1+0xF4 = anim_advance_time(arg0, 1.0f),
// copy_qw4(D_70003400, D_70003000), func_001C69A0(arg0), and when func_001B17A0
// reports the actor is visible, optionally func_001288D0 then call the per-actor
// hook at arg0+0x4C.
extern short anim_advance_time(char *p, float dt);
extern void copy_qw4(void *d, void *s);
extern void func_00103230(char *d, char *s, float f);
extern void func_001287F0(char *p, char *q, int a, float f);
extern void func_001288D0(char *p, char *q);
extern void func_00129F00(char *p, char *q);
extern void func_0012D580(char *p, char *q, int mode);
extern int func_001B17A0(char *p);
extern void func_001B5360(char *p);
extern int func_001C25E0(char *p, char *v);
extern int func_001C2770(char *p, char *q, int a);
extern void func_001C3D60(char *p, char *q);
extern void func_001C69A0(char *p);
extern void func_001EFD90(unsigned int id, char *a, char *b);
extern int func_001EFE00(unsigned int id, char *p);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_001FC580(char *p, int a);
extern char D_70003000[];
extern char D_70003400[];
extern char D_700038A0[];

void func_00129FC0(char *arg0, char *arg1) {
    int mode;
    int n;
    int t;
    int dec;

    mode = func_001C2770(arg0, arg1, 0);
    switch (*(unsigned char *)(arg0 + 5)) {
    case 0:
        *(float *)(arg0 + 0x38) = *(float *)(arg1 + 0xD8);
        *(float *)(arg1 + 0xD8) = 0.0f;
        if (*(short *)(arg0 + 0x36) & 0x4000) {
            func_001EFE00(0x80000027, arg0);
            *(short *)(arg0 + 0x28) = 0x3C;
        } else {
            *(short *)(arg0 + 0x28) = 0;
        }
        dec = *(unsigned char *)(arg0 + 0x36);
        *(short *)(arg0 + 0x34) = *(short *)(arg0 + 0x34) - dec;
        *(short *)(arg1 + 0xF4) = 0;
        func_001FBD50(arg0, 0x1B1, 0, 300.0f);
        if ((*(short *)(arg0 + 0x36) & 0x2000) ||
            (*(unsigned char *)(arg1 + 0xFB) & 0x80) ||
            ((n = *(int *)(arg1 + 0xE4)) == 0) || (n == 0x400) || (n == 0x500)) {
            func_001287F0(arg0, arg1, 0x1B, 0.0f);
            func_00103230(arg0 + 0x70, arg0 + 0x70, 0.5f);
            *(float *)(arg0 + 0xC0) = 0.0f;
            *(float *)(arg1 + 0xF0) = 0.8f;
            *(int *)(arg1 + 0xE4) = 0x600;
            *(unsigned char *)(arg0 + 5) = 5;
            if (*(short *)(arg0 + 0x34) <= 0) {
                *(short *)(arg0 + 0x34) = 0;
                *(char *)(arg1 + 0xFA) = 0;
                func_001EFD90(0x8000000C, arg0 + 0xB0, arg0 + 0xC0);
            } else {
                *(char *)arg0 = 1;
            }
        } else {
            func_001287F0(arg0, arg1, 0x1D, 0.0f);
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            if (*(short *)(arg0 + 0x34) <= 0) {
                *(short *)(arg0 + 0x34) = 0;
                *(char *)(arg1 + 0xFA) = 0;
                func_001EFD90(0x8000000C, arg0 + 0xB0, arg0 + 0xC0);
            } else {
                *(char *)arg0 = 1;
            }
        }
        *(short *)(arg0 + 0x36) = 0;
        break;
    case 1:
        if (*(short *)(arg0 + 0x28) != 0) {
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
        }
        t = *(short *)(arg0 + 0x36);
        if (t != 0 && *(short *)(arg0 + 0x34) != 0) {
            if (((short)t & 0x4000) && *(short *)(arg0 + 0x28) == 0) {
                func_001EFE00(0x80000027, arg0);
                *(short *)(arg0 + 0x28) = 0x3C;
            }
            dec = *(unsigned char *)(arg0 + 0x36);
            *(short *)(arg0 + 0x34) = *(short *)(arg0 + 0x34) - dec;
            if (*(short *)(arg0 + 0x34) <= 0) {
                *(short *)(arg0 + 0x34) = 0;
                *(char *)(arg1 + 0xFA) = 0;
                func_001EFD90(0x8000000C, arg0 + 0xB0, arg0 + 0xC0);
                *(char *)arg0 = 3;
            } else {
                *(char *)arg0 = 1;
                *(short *)(arg0 + 0x36) = 0;
            }
        }
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            *(char *)(arg1 + 0xFA) = 0;
            if (*(short *)(arg0 + 0x34) <= 0) {
                *(char *)arg0 = 2;
                *(short *)(arg0 + 0x34) = 0;
                *(volatile int *)0x700038A0 = 0;
                *(volatile int *)0x700038A4 = 0xBFB33333;
                *(volatile int *)0x700038A8 = 0;
                *(volatile int *)0x700038AC = 0x3F800000;
                if (func_001C25E0(arg0, D_700038A0) == 0) {
                    *(unsigned char *)(arg0 + 5) = 2;
                } else if ((*(int *)(arg1 + 0xE4) >> 8) == 1) {
                    *(unsigned char *)(arg0 + 5) = 3;
                } else {
                    *(unsigned char *)(arg0 + 5) = 2;
                }
            } else {
                *(char *)(arg0 + 4) = 1;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg1 + 0xFB) & 0x7F;
                *(char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 7) = 0;
                *(float *)(arg1 + 0xD8) = *(float *)(arg0 + 0x38);
                *(char *)arg0 = 1;
            }
        }
        break;
    case 2:
        func_0012D580(arg0, arg1, mode);
        break;
    case 3:
        func_001287F0(arg0, arg1, 0x20, 4.0f);
        func_001FBD50(arg0, 0x1B7, 0, 300.0f);
        *(char *)arg0 = 2;
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        *(float *)(arg1 + 0xF0) = 0.06f;
        break;
    case 4:
        if (*(float *)(arg0 + 0x3C) < 10.0f) {
            func_001FC580(arg0, 0x1B5);
            if (func_001EFE00(0x8000000F, arg0) != 0) {
                *(unsigned char *)(arg0 + 5) = 7;
            } else {
                *(char *)(arg0 + 4) = 3;
            }
        }
        func_00129F00(arg0, arg1);
        break;
    case 5:
        func_001B5360(arg0);
        if (*(short *)(arg0 + 0x28) != 0) {
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
        }
        t = *(short *)(arg0 + 0x36);
        if (t != 0 && *(short *)(arg0 + 0x34) != 0) {
            if (((short)t & 0x4000) && *(short *)(arg0 + 0x28) == 0) {
                func_001EFE00(0x80000027, arg0);
                *(short *)(arg0 + 0x28) = 0x3C;
            }
            dec = *(unsigned char *)(arg0 + 0x36);
            *(short *)(arg0 + 0x34) = *(short *)(arg0 + 0x34) - dec;
            if (*(short *)(arg0 + 0x34) <= 0) {
                *(short *)(arg0 + 0x34) = 0;
                *(char *)(arg1 + 0xFA) = 0;
                func_001EFD90(0x8000000C, arg0 + 0xB0, arg0 + 0xC0);
                *(char *)arg0 = 3;
            } else {
                *(char *)arg0 = 1;
                *(short *)(arg0 + 0x36) = 0;
            }
        }
        *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x70);
        *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x78);
        if (*(int *)(arg1 + 0xE4) != 0x600) {
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        }
        break;
    case 6:
        func_001287F0(arg0, arg1, 0x1D, 4.0f);
        *(unsigned char *)(arg0 + 5) = 1;
        break;
    case 7:
        func_00129F00(arg0, arg1);
        break;
    }
    if (mode == 0) {
        func_001C3D60(arg0, arg1);
    }
    *(short *)(arg1 + 0xF4) = anim_advance_time(arg0, 1.0f);
    copy_qw4(D_70003400, D_70003000);
    func_001C69A0(arg0);
    if (func_001B17A0(arg0) != 0) {
        if (*(char *)(arg1 + 0xFA) != 0) {
            func_001288D0(arg0, arg1);
        }
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
    }
}
