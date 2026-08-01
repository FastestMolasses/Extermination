// NEARMISS func_00128C10  (vram 0x00128C10, 0xB6C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.48% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// SEMANTICS: per-frame update for one NPC, called as (e) where e is the actor
// record and b = e + 0x1F0 is its animation/behaviour block.  base = D_008102B0
// is the global player/camera record; base + 0xA0 is its position vec3.
//   e[0]    = actor update/priority class
//   e[1]    = "actor is visible/active" flag
//   e[4]    = top-level state (0..4), tested as a descending compare chain
//   e[5]    = sub-state; in state 1 it selects a body through a 9-entry table
//   e[6]    = sub-sub-state used by e[5] == 3
//   e[0xA]  = per-frame event flags (bit 0 = "startled")
//   e[0xB]  = cleared at the end of a state-1 frame
//   e[0xD]  = NPC kind (0..9); selects a body through a 10-entry table.
//             Kinds 4 and 9 are the "panic" kinds and are remapped to 3 / 8.
//   e[0x11] = difficulty / scenario selector
//   e[0x28] = generic short countdown
//   e[0x4C] = per-actor virtual update hook, called last
//   e[0x54] = cleared whenever the NPC re-plans
//   e[0x9A] = sound/voice id
//   e[0xB0] = actor position vec3
//   e[0xC4] = actor facing angle
//   b[0xD0] = short countdown driving the walk/turn cycle
//   b[0xD8] = movement speed, b[0xE4] = flags (>>8 == 1 selects the near radii)
//   b[0xE8] = desired heading, b[0xEC] = animation playback rate
//   b[0xF4] = animation frame result, b[0xF8] = bone-init request
//   b[0xFA] = "run the extra pose pass", b[0xFB] = pose/clip selector
//   b[0xE0] = "animation finished" flag
// D_00242F20 is a 4-byte-stride table of per-kind clip ids (only the leading
// short is read); D_00242EB0 / D_00242EB6 / D_00242EBC are random-duration
// tables indexed by func_00128600(0).  D_00275380 is a small gp-relative table
// of stagger durations.  The 0x70003Bxx addresses are EE scratchpad frame
// counters used to stagger work across frames.
// Fields written through `int *` hold IEEE-754 float bit patterns; that is how
// the original stored them (GPR store, no swc1).

extern int   func_00128AB0(unsigned char *, unsigned char *);
extern void  bone_init_default_2(unsigned char *, int);
extern int   func_00129780(unsigned char *, unsigned char *, unsigned char);
extern int   func_001B2140(unsigned char *);
extern void  func_001029C0(float *);
extern int   func_001B0D80(unsigned char *);
extern void  func_001B17A0(unsigned char *);
extern int   func_00128B80(unsigned char *, unsigned char *);
extern void  func_001287F0(unsigned char *, unsigned char *, short, float);
extern int   func_00122BB8(void);
extern float func_001B1470(float);
extern int   func_001B13F0(float *, unsigned char *, float);
extern int   func_001C25E0(unsigned char *, float *);
extern float func_001B12B0(float, float, float);
extern int   func_00128600(int);
extern void  func_00102948(float *, float *);
extern int   func_001C2770(unsigned char *, unsigned char *, int);
extern void  func_0012D580(unsigned char *, unsigned char *, int);
extern void  func_001C3D60(unsigned char *, unsigned char *);
extern short anim_advance_time(unsigned char *, float);
extern void  copy_qw4(float *, float *);
extern void  func_001C69A0(unsigned char *);
extern void  func_001288D0(unsigned char *, unsigned char *);
extern void  func_00129FC0(unsigned char *, unsigned char *);
extern void  func_001B1190(unsigned char);
extern void  func_001AFC10(unsigned char *);
extern void  func_001289C0(unsigned char *, unsigned char *);
extern int   func_001B1630(float, float, float);

extern float D_70003000[];
extern float D_70003400[];
extern float D_700038A0[];
extern float D_008102B0[0x400];
extern short D_00242EB0[0x40];
extern short D_00242EB6[0x40];
extern short D_00242EBC[0x40];
extern short D_00242F20[0x40];
extern short D_00275380[4];

void func_00128C10(unsigned char *e)
{
    float *base;
    unsigned char *b;
    int kind;
    int t;
    int r;
    short c;
    float x;

    base = D_008102B0;
    b = e + 0x1F0;
    switch (e[4]) {
    case 0:
        switch (e[5]) {
        case 0:
            if (func_00128AB0(e, b) != 0) {
                e[5] = e[5] + 1;
                *(short *)(b + 0xF8) = 1;
                bone_init_default_2(e, 1);
                *(short *)(e + 0x28) = 0;
            }
            break;
        case 1:
            if (func_00129780(e, b, e[0xD]) != 0) {
                *(short *)(e + 0x54) = 0;
                t = e[0xD];
                if (t != 4 && t != 9) {
                    e[0] = 1;
                }
            }
            break;
        }
        break;
    case 1:
        if (func_001B2140(e) == 0) {
            break;
        }
        t = *(volatile unsigned char *)0x70003B8D;
        if (t != 0 && (t & 0xFF) >= 2) {
            break;
        }
        *(short *)(e + 0x54) = 0;
        func_001029C0(D_70003000);
        c = *(volatile short *)0x70003B8A;
        if (((*(volatile int *)0x70003B68 + c) & 0x3F) == 0 && func_001B0D80(e) != 0) {
            break;
        }
        func_001B17A0(e);
        if (func_00128B80(e, b) == 0) {
            b[0xFA] = 0;
            kind = e[0xD];
            switch (kind) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 5:
            case 6:
            case 7:
            case 8:
                switch (e[5]) {
                case 0:
                    b[0xFB] = 2;
                    func_001287F0(e, b, 1, 4.0f);
                    *(int *)(b + 0xEC) = 0x3F800000;
                    if (e[0xA] & 1) {
                        *(float *)(b + 0xE8) = func_001B1470(
                            (6.2831855f * (float)(func_00122BB8() & 0xF0)) / 256.0f);
                        e[5] = 2;
                        break;
                    }
                    t = e[0xD];
                    if (t < 4) {
                        if (func_001B13F0(base + 0x28, e + 0xB0, 100.0f) != 0) {
                            e[5] = e[5] + 1;
                            *(short *)(b + 0xD0) = 0;
                        }
                    } else {
                        if ((*(int *)(b + 0xE4) >> 8) == 1) {
                            x = 20.0f;
                        } else {
                            x = 40.0f;
                        }
                        if (func_001B13F0(base + 0x28, e + 0xB0, x) != 0) {
                            e[5] = e[5] + 1;
                            *(short *)(b + 0xD0) = 0;
                        }
                    }
                    if (e[1] != 0) {
                        *(volatile int *)0x700038A0 = 0;
                        *(volatile int *)0x700038A4 = 0xBFB33333;
                        *(volatile int *)0x700038A8 = 0;
                        *(volatile int *)0x700038AC = 0x3F800000;
                        if (func_001C25E0(e, D_700038A0) == 0) {
                            e[5] = 8;
                            e[6] = 0;
                            e[7] = 0;
                        }
                    }
                    break;
                case 1:
                    b[0xFB] = 2;
                    func_001287F0(e, b, D_00242F20[e[0xD] * 2], 4.0f);
                    *(int *)(b + 0xEC) = 0x3F800000;
                    if (e[0xA] & 1) {
                        *(float *)(b + 0xE8) = func_001B1470(
                            (6.2831855f * (float)(func_00122BB8() & 0xF0)) / 256.0f);
                        e[5] = 2;
                        break;
                    }
                    if (func_001B13F0(base + 0x28, e + 0xB0, 150.0f) == 0) {
                        *(short *)(b + 0xD0) = *(short *)(b + 0xD0) + 1;
                        if (*(short *)(b + 0xD0) >= 0x5A) {
                            e[5] = 0;
                        }
                    } else {
                        *(short *)(b + 0xD0) = 0;
                        if ((*(int *)(b + 0xE4) >> 8) == 1) {
                            x = 10.0f;
                        } else {
                            x = 24.0f;
                        }
                        if (func_001B13F0(base + 0x28, e + 0xB0, x) != 0) {
                            e[5] = e[5] + 1;
                            *(float *)(b + 0xE8) = func_001B1470(
                                (6.2831855f * (float)(func_00122BB8() & 0xF0)) / 256.0f);
                        }
                    }
                    t = e[5];
                    if (t < 2 && e[1] != 0) {
                        *(volatile int *)0x700038A0 = 0;
                        *(volatile int *)0x700038A4 = 0xC0800000;
                        *(volatile int *)0x700038A8 = 0;
                        *(volatile int *)0x700038AC = 0x3F800000;
                        if (func_001C25E0(e, D_700038A0) == 0) {
                            e[5] = 8;
                            e[6] = 0;
                            e[7] = 0;
                        }
                    }
                    break;
                case 2:
                    func_001287F0(e, b, D_00242F20[(unsigned char)kind * 2], 4.0f);
                    b[0xFB] = 2;
                    b[0xFA] = 1;
                    x = func_001B12B0(*(float *)(b + 0xE8), *(float *)(e + 0xC4), 0.34906587f);
                    *(float *)(e + 0xC4) = x;
                    if (x == *(float *)(b + 0xE8)) {
                        e[5] = e[5] + 1;
                        e[6] = 0;
                        *(int *)(b + 0xD8) = 0x3F4CCCCD;
                        *(int *)(b + 0xEC) = 0x40266666;
                        *(short *)(b + 0xD0) = D_00242EB6[func_00128600(0)];
                    }
                    break;
                case 3:
                    b[0xFB] = 3;
                    switch (e[6]) {
                    case 0:
                        *(float *)(e + 0xC4) = func_001B12B0(*(float *)(b + 0xE8),
                                                             *(float *)(e + 0xC4),
                                                             0.06981317f);
                        func_001287F0(e, b, 6, 4.0f);
                        b[0xFA] = 1;
                        *(int *)(b + 0xEC) = 0x40266666;
                        *(int *)(b + 0xD8) = 0x3F4CCCCD;
                        c = *(short *)(b + 0xD0) - 1;
                        *(short *)(b + 0xD0) = c;
                        if (c == 0) {
                            func_001287F0(e, b, D_00242F20[e[0xD] * 2], 4.0f);
                            *(int *)(b + 0xEC) = 0x3F800000;
                            *(int *)(b + 0xD8) = 0;
                            if (func_00122BB8() & 0x10) {
                                *(float *)(b + 0xE8) =
                                    func_001B1470(0.69813174f + *(float *)(b + 0xE8));
                            } else {
                                *(float *)(b + 0xE8) =
                                    func_001B1470(-0.69813174f + *(float *)(b + 0xE8));
                            }
                            *(short *)(b + 0xD0) = D_00242EB0[func_00128600(0)];
                            e[6] = e[6] + 1;
                        }
                        break;
                    case 1:
                        c = *(short *)(b + 0xD0) - 1;
                        *(short *)(b + 0xD0) = c;
                        if (c == 0) {
                            *(int *)(b + 0xEC) = 0x40266666;
                            *(int *)(b + 0xD8) = 0x3F4CCCCD;
                            func_001287F0(e, b, 6, 4.0f);
                            e[6] = 0;
                            *(short *)(b + 0xD0) = D_00242EBC[func_00128600(0)];
                        }
                        break;
                    }
                    if (e[1] == 0) {
                        e[0] = 2;
                        e[4] = 3;
                        e[5] = 0;
                        e[6] = 0;
                    }
                    break;
                case 4:
                    b[0xFB] = 0;
                    func_001287F0(e, b, 6, 0.0f);
                    c = *(short *)(b + 0xD0) - 1;
                    *(short *)(b + 0xD0) = c;
                    if (c == 0) {
                        func_00102948((float *)(b + 0x50), (float *)(e + 0xB0));
                        *(int *)(b + 0xEC) = 0x3F800000;
                        *(int *)(b + 0xD8) = 0;
                        e[5] = 0;
                    }
                    break;
                case 8:
                    b[0xFB] = 0x80;
                    r = func_001C2770(e, b, 2);
                    func_0012D580(e, b, r);
                    if (r == 0) {
                        func_001C3D60(e, b);
                    }
                    break;
                }
                break;
            case 4:
            case 9:
                if (kind == 4) {
                    e[0xD] = 3;
                } else {
                    e[0xD] = 8;
                }
                *(int *)(b + 0xEC) = 0x3FCCCCCD;
                *(short *)(b + 0xD0) = (func_00122BB8() & 0x30) + 0x3C;
                *(int *)(b + 0xD8) = 0x3F19999A;
                e[5] = 4;
                e[0] = 1;
                func_00102948((float *)(b + 0x50), (float *)(e + 0xB0));
                break;
            }
        }
        if (e[5] != 8 && func_001C2770(e, b, 0) == 0) {
            func_001C3D60(e, b);
        }
        *(short *)(b + 0xF4) = anim_advance_time(e, *(float *)(b + 0xEC));
        copy_qw4(D_70003400, D_70003000);
        func_001C69A0(e);
        if (e[1] != 0) {
            if (*(signed char *)(b + 0xFA) != 0) {
                func_001288D0(e, b);
            }
            (*(void (**)(unsigned char *))(e + 0x4C))(e);
        }
        e[0xB] = 0;
        e[0xA] = 0;
        break;
    case 2:
        c = *(volatile short *)0x70003B8A;
        if (((*(volatile int *)0x70003B68 + c) & 0x3F) == 0 && func_001B0D80(e) != 0) {
            break;
        }
        func_00129FC0(e, b);
        e[0xB] = 0;
        e[0xA] = 0;
        break;
    case 3:
        if (b[0xE0] != 0) {
            e[0] = 2;
            e[4] = 4;
            e[5] = 0;
            func_00102948((float *)(e + 0xB0), (float *)(b + 0x50));
            break;
        }
        func_001B1190(e[0x9A]);
        func_001AFC10(e);
        break;
    case 4:
        switch (e[5]) {
        case 0:
            *(int *)(e + 0x60) = 0x3F800000;
            *(int *)(e + 0x64) = 0x3F800000;
            *(int *)(e + 0x68) = 0x3F800000;
            *(int *)(e + 0x6C) = 0x3F800000;
            func_001289C0(e, b);
            *(short *)(b + 0xF8) = 1;
            bone_init_default_2(e, 1);
            *(short *)(e + 0x28) = 0;
            e[5] = e[5] + 1;
            break;
        case 1:
            if (func_00129780(e, b, e[0xD]) != 0) {
                e[4] = 4;
                e[5] = 2;
                *(short *)(e + 0x28) = D_00275380[((func_00122BB8() >> 16) * 4) >> 15];
            }
            break;
        case 2:
            c = *(short *)(e + 0x28) - 1;
            *(short *)(e + 0x28) = c;
            if (c == 0) {
                e[5] = 3;
            }
            break;
        case 3:
            if (func_001B13F0(base + 0x28, e + 0xB0, 100.0f) == 0 &&
                (func_001B1630(*(float *)(e + 0xB0), *(float *)(e + 0xB4),
                               *(float *)(e + 0xB8)) & 0xFF) == 0) {
                e[0] = 1;
                e[4] = 1;
                e[5] = 0;
                e[6] = 0;
                e[7] = 0;
            }
            break;
        }
        break;
    }
}
