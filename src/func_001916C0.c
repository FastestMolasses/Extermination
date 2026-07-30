// NEARMISS func_001916C0  (vram 0x001916C0, 0x674 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 98.80% / 409 vs 409 instructions, identical instruction multiset, ZERO logic or control-flow differences. Two residual sites, both pure post-RA scheduling: (1) at 0x1917DC the -20/-10 clamp-limit if/else merge: target emits `b .L001917EC` + `nop`, mwcc 2.3.3 tail-duplicates the merge block's `lui...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: camera placement for the current player state. arg0 is the camera
// block, arg1 the actor, arg2 the camera mode (0 = smooth follow, 2 = snap, other =
// hold). The actor's animation/state id at arg1+0x230 selects one of seven camera
// presets via jump table jtbl_0026D990 (48 entries; the ids not listed fall through
// to the default preset). arg0+0x20 is the camera target point, arg0+0x24 its
// height, arg0+0x28 a secondary axis, arg0+0x8C a height bias, arg0+0x64 a tuning
// value, arg0+0xA0 a settle countdown, arg0+0xC an angle. arg1+0xA0/0xA4/0xA8 and
// arg1+0xB0/0xB4/0xB8 are the two candidate anchor points. 0x70003A20 is the
// scratchpad slot holding the computed yaw.
// Each preset either snaps the target (mode 2) or eases toward it with
// func_0018C6A0, then sets the height with func_0018C4B0 (mode 0) or writes it
// directly (mode 2). The first preset (states 0,1,3,14,20,21,22 - and 2,4,15 which
// share the alternate height) additionally recomputes the yaw: in snap mode from the
// planar length of the offset vector, otherwise as D_00810690 minus |arg0+0xC|, then
// clamps it against -20 (when arg0+0x64 == -46.8) or -10, reflecting and capping at
// -7. States 0,1,2 also set the "fast settle" flag used by the tail.
// The tail decrements the settle countdown; when it hits zero func_0018C0C0 releases
// the camera, otherwise the camera eases into D_008105E0 and the residual height
// error is closed at 1/20 (fast) or 1/4 per frame.

extern int func_001028D0(char *dst, char *a, char *b);
extern float func_0011DF78(float a);
extern float func_0011E748(float a);
extern void func_0018C0C0(char *e);
extern int func_0018C4B0(char *d, float a, float b);
extern int func_0018C6A0(char *src, char *dst, float t);
extern char D_008105E0[];
extern float D_008105E4;
extern float D_00810690;
extern char D_700038B0[];

void func_001916C0(char *arg0, char *arg1, int arg2) {
    int mode;
    int flag;
    float lim;
    float t;
    float u;
    float d;
    short n;

    mode = *(int *)(arg1 + 0x230);
    flag = 0;
    switch (mode) {
    case 0:
    case 1:
        flag = 1;
        /* fallthrough */
    case 3:
    case 14:
    case 20:
    case 21:
    case 22:
        if (arg2 == 2) {
            *(float *)(arg0 + 0x20) = *(float *)(arg1 + 0xA0);
            *(float *)(arg0 + 0x28) = *(float *)(arg1 + 0xA8);
        } else {
            func_0018C6A0(arg1 + 0xA0, arg0 + 0x20, 2.0f);
        }
        if (arg2 == 2) {
            func_001028D0(D_700038B0, arg0 + 0x20, arg0 + 0x10);
            *(float *)0x70003A20 = func_0011E748(*(float *)0x700038B0 * *(float *)0x700038B0 +
                                                 *(float *)0x700038B8 * *(float *)0x700038B8);
        } else {
            *(float *)0x70003A20 = D_00810690 - func_0011DF78(*(float *)(arg0 + 0xC));
        }
        if (*(float *)(arg0 + 0x64) == -46.8f) {
            lim = -20.0f;
        } else {
            lim = -10.0f;
        }
        t = *(float *)0x70003A20;
        if (t < lim) {
            u = lim + (lim - t);
            *(float *)0x70003A20 = u;
            if (!(u <= -7.0f)) {
                *(float *)0x70003A20 = -7.0f;
            }
        }
        if (arg2 == 0) {
            func_0018C4B0(arg0 + 0x20,
                          11.0f + (*(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C) +
                                   0.3f * *(float *)0x70003A20),
                          4.0f);
        } else if (arg2 == 2) {
            *(float *)(arg0 + 0x24) = 11.0f + (*(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C) +
                                               0.3f * *(float *)0x70003A20);
        }
        break;
    case 10:
    case 25:
        if (arg2 == 2) {
            *(float *)(arg0 + 0x20) = *(float *)(arg1 + 0xA0);
            *(float *)(arg0 + 0x28) = *(float *)(arg1 + 0xA8);
        } else {
            func_0018C6A0(arg1 + 0xA0, arg0 + 0x20, 2.0f);
        }
        if (arg2 == 0) {
            func_0018C4B0(arg0 + 0x20,
                          11.0f + *(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C), 4.0f);
        } else if (arg2 == 2) {
            *(float *)(arg0 + 0x24) = 11.0f + *(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C);
        }
        break;
    case 19:
        if (arg2 == 2) {
            *(float *)(arg0 + 0x20) = *(float *)(arg1 + 0xA0);
            *(float *)(arg0 + 0x28) = *(float *)(arg1 + 0xA8);
        } else {
            func_0018C6A0(arg1 + 0xA0, arg0 + 0x20, 2.0f);
        }
        if (arg2 == 0) {
            func_0018C4B0(arg0 + 0x20, *(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C), 4.0f);
        } else if (arg2 == 2) {
            *(float *)(arg0 + 0x24) = *(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C);
        }
        break;
    case 2:
        flag = 1;
        /* fallthrough */
    case 4:
    case 15:
        if (arg2 == 2) {
            *(float *)(arg0 + 0x20) = *(float *)(arg1 + 0xA0);
            *(float *)(arg0 + 0x28) = *(float *)(arg1 + 0xA8);
        } else {
            func_0018C6A0(arg1 + 0xA0, arg0 + 0x20, 2.0f);
        }
        if (arg2 == 0) {
            func_0018C4B0(arg0 + 0x20,
                          11.0f + (*(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C)), 4.0f);
        } else if (arg2 == 2) {
            *(float *)(arg0 + 0x24) = 11.0f + (*(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C));
        }
        break;
    case 8:
    case 12:
    case 13:
    case 38:
    case 39:
    case 41:
    case 42:
    case 47:
        if (arg2 == 2) {
            *(float *)(arg0 + 0x20) = *(float *)(arg1 + 0xA0);
            *(float *)(arg0 + 0x28) = *(float *)(arg1 + 0xA8);
        } else {
            func_0018C6A0(arg1 + 0xA0, arg0 + 0x20, 2.0f);
        }
        if (arg2 == 0) {
            func_0018C4B0(arg0 + 0x20, *(float *)(arg1 + 0xB4) + *(float *)(arg0 + 0x8C), 4.0f);
        } else if (arg2 == 2) {
            *(float *)(arg0 + 0x24) = *(float *)(arg1 + 0xB4) + *(float *)(arg0 + 0x8C);
        }
        break;
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
        if (arg2 == 2) {
            *(float *)(arg0 + 0x20) = *(float *)(arg1 + 0xA0);
            *(float *)(arg0 + 0x28) = *(float *)(arg1 + 0xA8);
        } else {
            func_0018C6A0(arg1 + 0xA0, arg0 + 0x20, 2.0f);
        }
        if (arg2 == 0) {
            func_0018C4B0(arg0 + 0x20,
                          11.0f + *(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C), 4.0f);
        } else if (arg2 == 2) {
            *(float *)(arg0 + 0x24) = 11.0f + *(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C);
        }
        break;
    case 5:
    case 6:
    case 7:
    case 9:
    case 11:
    case 16:
    case 17:
    case 18:
    case 23:
    case 24:
    case 26:
    case 27:
    case 28:
    case 37:
    case 40:
    case 43:
    case 44:
    case 45:
    case 46:
    default:
        if (arg2 == 2) {
            *(float *)(arg0 + 0x20) = *(float *)(arg1 + 0xB0);
            *(float *)(arg0 + 0x28) = *(float *)(arg1 + 0xB8);
        } else {
            func_0018C6A0(arg1 + 0xB0, arg0 + 0x20, 1.5f);
        }
        if (arg2 == 0) {
            func_0018C4B0(arg0 + 0x20, *(float *)(arg1 + 0xB4) + *(float *)(arg0 + 0x8C), 4.0f);
        } else if (arg2 == 2) {
            *(float *)(arg0 + 0x24) = *(float *)(arg1 + 0xB4) + *(float *)(arg0 + 0x8C);
        }
        break;
    }
    n = *(short *)(arg0 + 0xA0);
    if (n != 0) {
        *(short *)(arg0 + 0xA0) = n - 1;
    }
    if (arg2 != 2) {
        if (*(short *)(arg0 + 0xA0) == 0) {
            func_0018C0C0(arg0);
            return;
        }
        func_0018C6A0(arg0 + 0x20, D_008105E0, 1.0f);
        d = *(float *)(arg0 + 0x24) - D_008105E4;
        if (func_0011DF78(d) <= 0.15f) {
            *(short *)(arg0 + 0xA0) = 0;
        }
        if (flag != 0) {
            func_0018C4B0(D_008105E0, *(float *)(arg0 + 0x24), func_0011DF78(d) / 20.0f);
        } else {
            func_0018C4B0(D_008105E0, *(float *)(arg0 + 0x24), func_0011DF78(d) / 4.0f);
        }
    }
}
