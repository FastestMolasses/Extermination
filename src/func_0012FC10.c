// NEARMISS func_0012FC10  (vram 0x0012FC10, 0x9A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Address-mode/list-scheduling + register-coloring permutation (permuter class, NOT the clean-store nop). Body fully recovered, 623==623 instructions on mwcc233, 35 diff rows in exactly 2 artifact clusters: (1) x3 identical anim-table index sites (D_00244F70/72/74[kind*3]) - target evaluates the lb...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// entity_logic: 4-state landing / ground-attack sequence on the byte at a+6
// (arg0 = actor, arg1 = control record).
//   state 0: clear the record timers/flags (+0x56/+0x52/+0x63/+0x5A/+0x3C, set
//     +0x6B and actor+0x5C), speed +0x34 = 1.5f (2.0f when actor flag bit 1),
//     start the descend clip from table D_00244F70[3*kind] (kind = arg1+0x64),
//     advance.
//   state 1: home toward the landing spot (heading via func_001B1240 on
//     D_00810360/68, slewed into actor+0xC4 at 0.0698/frame). When the clip
//     signals (arg1+0x58 & 0x1000): speed 1.0f, start clip D_00244F72[3*kind],
//     advance; non-kind-2 also arms the 30-tick timer +0x54 and spawns the
//     dust/impact emitter func_001EFF10(0x8000000D, *(actor+0x170)+0x90, ...)
//     with the (0,0,0,1)/(7,0,0,1)/(128,40,80,128)/(0,0,0,0) scratch records,
//     storing the emitter handle at actor+0x20; kind 2 arms 20 ticks instead.
//   state 2: when the +0x54 countdown expires fire SFX 0x7D1 (kind 2) / 0x7D0.
//     Once per landing (guard word +0x5A, kind != 2, zone check func_0021BE40
//     clear, func_001A7B80 accepts): latch +0x5A, publish the player-relative
//     offset D_00810320 = D_00810360 - actor.x / D_00810328 = D_00810368 -
//     actor.z (normalized via func_00102760), func_001B55E0(actor,1), and set
//     the pursuit speed D_008104D4 from the actor flag byte (bits 0x83) and the
//     difficulty toggle D_0081070A -- two tables, keyed on whether +0x64 is 0.
//     Kind-2 actors at anim frame 24.0 (actor+0x3C) instead build the muzzle
//     transform (offset -1.18,11.1,7.54 through func_001B2B10/func_001028B8),
//     aim it (func_001B15D0 / func_0011E620 pitch, func_001029C0 / func_00102B08
//     / func_00102BB0 yaw into D_700036A0) and fire the beam/breath effect
//     func_001EFFD0(0x80000008, ...) variant-picked from flag bits 3/0x80
//     (0.7f strength; the 3-case uses 0.55+0.15*rand/255). On the clip signal
//     start clip D_00244F74[3*kind], advance, and if the emitter handle at
//     actor+0x20 is live, set its byte +4 = 3 (fade out) and clear the handle.
//   state 3: on the clip signal reset behavior/substate, clear +0x6B, and set
//     the re-engage delay +0x5A = rand>>6 & 0x1F (0 when actor flag bit 1).
// Tail: always runs the shared post-update func_00132490(actor, record).

extern void anim_clip_init(char *self, int clip, float a, float b);
extern float func_001B1240(char *origin, float x, float z);
extern float func_001B12B0(float a, float b, float c);
extern char *func_001EFF10(int mode, int a1, float *a2, float *a3, float *a4, float *a5, float f);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(unsigned char *a, char *b);
extern int func_001A7B80(char *p);
extern void func_00102760(float *a, float *b);
extern void func_001B55E0(char *p, int a);
extern void func_001B2B10(char *p, float *a, float *b);
extern void func_001028B8(float *a, float *b, char *c);
extern void func_00102948(float *a, float *b);
extern float func_001B15D0(float *a, float *b);
extern float func_0011DF78(float a);
extern float func_0011E620(float a, float b);
extern void func_001029C0(float *a);
extern void func_00102B08(float *a, float *b, float c);
extern void func_00102BB0(float *a, float *b, float c);
extern void func_001026A0(float *a, float *b, float *c);
extern int func_00122BB8(void);
extern void func_00132490(char *a, char *b);
extern void func_001EFFD0(int a, float *b, float *c, int d, float e);

extern short D_00244F70[];
extern short D_00244F72[];
extern short D_00244F74[];
extern float D_00810320[];
extern int D_00810324;
extern float D_00810328[];
extern float D_00810350;
extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern int D_008104D4;
extern unsigned char D_008102B0;
extern unsigned char D_0081070A;
extern float D_700036A0;
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;
extern float D_700038D0;

void func_0012FC10(char *a, char *b)
{
    char *arg1 = b;
    char *arg0 = a;
    unsigned char st;
    unsigned short tm;
    int flags;
    float t;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(short *)(arg1 + 0x56) = 0;
        *(short *)(arg1 + 0x52) = 0;
        *(char *)(arg1 + 0x63) = 0;
        *(short *)(arg1 + 0x5A) = 0;
        *(int *)(arg1 + 0x3C) = 0;
        *(char *)(arg1 + 0x6B) = 1;
        *(char *)(arg0 + 0x5C) = 1;
        if (!(*(unsigned char *)(arg0 + 0xD) & 2)) {
            *(int *)(arg1 + 0x34) = 0x3FC00000;
        } else {
            *(int *)(arg1 + 0x34) = 0x40000000;
        }
        anim_clip_init(arg0, D_00244F70[*(unsigned char *)(arg1 + 0x64) * 3], 5.0f, 0.0f);
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        break;
    case 1:
        *(float *)(arg1 + 0x30) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x30), *(float *)(arg0 + 0xC4), 0.06981317f);
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(int *)(arg1 + 0x34) = 0x3F800000;
            anim_clip_init(arg0, D_00244F72[*(unsigned char *)(arg1 + 0x64) * 3], 5.0f, 0.0f);
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            if (*(unsigned char *)(arg1 + 0x64) != 2) {
                *(short *)(arg1 + 0x54) = 0x1E;
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0;
                *(int *)0x700038A8 = 0;
                *(int *)0x700038D0 = 0;
                *(int *)0x700038D4 = 0;
                *(int *)0x700038D8 = 0;
                *(int *)0x700038DC = 0;
                *(int *)0x700038AC = 0x3F800000;
                *(int *)0x700038B0 = 0x40E00000;
                *(int *)0x700038B4 = 0;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x3F800000;
                *(int *)0x700038C0 = 0x43000000;
                *(int *)0x700038C4 = 0x42200000;
                *(int *)0x700038C8 = 0x42A00000;
                *(int *)0x700038CC = 0x43000000;
                *(char **)(arg0 + 0x20) = func_001EFF10(0x8000000D, *(int *)(arg0 + 0x170) + 0x90, &D_700038A0, &D_700038B0, &D_700038C0, &D_700038D0, 10.0f);
            } else {
                *(short *)(arg1 + 0x54) = 0x14;
            }
        }
        break;
    case 2:
        tm = *(unsigned short *)(arg1 + 0x54) - 1;
        *(unsigned short *)(arg1 + 0x54) = tm;
        if (!tm) {
            if (*(unsigned char *)(arg1 + 0x64) == 2) {
                func_001FBD50(arg0, 0x7D1, 0, 300.0f);
            } else {
                func_001FBD50(arg0, 0x7D0, 0, 300.0f);
            }
        }
        if ((func_0021BE40(&D_008102B0, arg0) == 0) && (*(unsigned short *)(arg1 + 0x5A) == 0) &&
            (*(unsigned char *)(arg1 + 0x64) != 2) && (func_001A7B80(arg0) != 0)) {
            *(unsigned short *)(arg1 + 0x5A) = 0xFFFF;
            D_00810320[0] = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328[0] = D_00810368 - *(float *)(arg0 + 0xB8);
            func_00102760(D_00810320, D_00810320);
            func_001B55E0(arg0, 1);
            if (*(unsigned char *)(arg1 + 0x64) != 0) {
                flags = *(unsigned char *)(arg0 + 0xD) & 0x83;
                switch (flags) {
                case 1:
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41F00000;
                    } else {
                        D_008104D4 = 0x41C80000;
                    }
                    break;
                case 3:
                    D_008104D4 = 0x41C80000;
                    break;
                case 0x80:
                    D_008104D4 = 0x41C80000;
                    break;
                case 0x81:
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x42200000;
                    } else {
                        D_008104D4 = 0x421C0000;
                    }
                    break;
                default:
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41A00000;
                    } else {
                        D_008104D4 = 0x41700000;
                    }
                    break;
                }
                D_008102B0 |= 2;
            } else {
                flags = *(unsigned char *)(arg0 + 0xD) & 0x83;
                switch (flags) {
                case 1:
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41C80000;
                    } else {
                        D_008104D4 = 0x41A00000;
                    }
                    break;
                case 3:
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41C80000;
                    } else {
                        D_008104D4 = 0x41A00000;
                    }
                    break;
                case 0x80:
                    D_008104D4 = 0x41C80000;
                    break;
                case 0x81:
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41F00000;
                    } else {
                        D_008104D4 = 0x41E00000;
                    }
                    break;
                default:
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41A00000;
                    } else {
                        D_008104D4 = 0x41700000;
                    }
                    break;
                }
                D_008102B0 |= 2;
            }
        } else if ((*(unsigned char *)(arg1 + 0x64) == 2) && (*(float *)(arg0 + 0x3C) == 24.0f)) {
            *(int *)0x700038A0 = 0xBF9703B0;
            *(int *)0x700038A4 = 0x41318ADB;
            *(int *)0x700038A8 = 0x40F14539;
            *(int *)0x700038AC = 0x3F800000;
            func_001B2B10(arg0, &D_700038A0, &D_700038A0);
            func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
            func_00102948(&D_700038B0, &D_700038A0);
            func_00102948(&D_700038C0, &D_00810350);
            *(int *)0x700038C4 = 0;
            *(int *)0x700038B4 = 0;
            *(float *)0x70003A20 = func_001B15D0(&D_700038B0, &D_700038C0);
            t = func_0011DF78(D_00810364 - *(float *)0x700038A4);
            *(float *)0x70003A24 = t;
            *(float *)0x70003A20 = -1.0f * func_0011E620(t, *(float *)0x70003A20);
            func_001029C0(&D_700036A0);
            func_00102B08(&D_700036A0, &D_700036A0, *(float *)0x70003A20);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(arg0 + 0xC4));
            *(int *)0x700038B0 = 0;
            *(int *)0x700038B4 = 0;
            *(int *)0x700038B8 = 0x3F800000;
            *(int *)0x700038BC = 0x3F800000;
            func_001026A0(&D_700038B0, &D_700036A0, &D_700038B0);
            flags = *(unsigned char *)(arg0 + 0xD);
            switch (flags & 3) {
            case 0:
                if (flags & 0x80) {
                    func_001EFFD0(0x80000008, &D_700038A0, &D_700038B0, 0xF, 0.7f);
                } else {
                    func_001EFFD0(0x80000008, &D_700038A0, &D_700038B0, 0xE, 0.7f);
                }
                break;
            case 1:
                if (flags & 0x80) {
                    func_001EFFD0(0x80000008, &D_700038A0, &D_700038B0, 0xD, 0.7f);
                } else {
                    func_001EFFD0(0x80000008, &D_700038A0, &D_700038B0, 0xC, 0.7f);
                }
                break;
            case 3:
                *(float *)0x70003A20 = 0.55f + (0.15f * ((float)((func_00122BB8() >> 0xE) & 0xFF) / 255.0f));
                func_001EFFD0(0x80000008, &D_700038A0, &D_700038B0, 0x12, *(float *)0x70003A20);
                break;
            }
        }
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            anim_clip_init(arg0, D_00244F74[*(unsigned char *)(arg1 + 0x64) * 3], 5.0f, 0.0f);
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            if (*(char **)(arg0 + 0x20) != 0) {
                *(*(char **)(arg0 + 0x20) + 4) = 3;
                *(char **)(arg0 + 0x20) = 0;
            }
        }
        break;
    case 3:
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x6B) = 0;
            if (*(unsigned char *)(arg0 + 0xD) & 2) {
                *(short *)(arg1 + 0x5A) = 0;
            } else {
                *(short *)(arg1 + 0x5A) = (func_00122BB8() >> 6) & 0x1F;
            }
        }
        break;
    }
    func_00132490(arg0, arg1);
}
