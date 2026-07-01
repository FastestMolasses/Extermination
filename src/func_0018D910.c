// NEARMISS func_0018D910  (vram 0x0018D910, 0x408 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 81.89% (233; 991202=78.47%). Logic and every branch/call/global fully recovered (a large FP-heavy camera-probe solver with a nested D_00810700==0x12 sub-branch and a shared-merge tail). Residual is a genuine mwcc FP list-scheduling wall: at each of the three 'copy probe + adjust Y by +-200' block...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Camera/look-ahead target height solver. arg0 = camera state, arg1 = target
// entity, arg2 = collision mask. Builds a probe segment at D_700038F0: from
// arg1+0xA0, blended (func_001028D0 x2 bracketing a func_00102760 normalize)
// toward arg0+0x10, with its Y offset bumped by +11.0. Casts that probe
// (D_700038A0, Y-200) via func_0019A910; if it hits AND the hit object's
// flags (+0x1A) have bit 0x7000 set, seeds 0x70003A38 from the hit distance
// (+6.0 if arg0+0x5C==1.0, else +17.0); otherwise 0x70003A38 = arg0+0x50 -
// 200.0. Casts a second probe (D_700038B0, Y+200) the same way; if it hits
// AND the hit flags have bit 0x8800 set, 0x70003A3C = hitDist - 1.0 and
// jumps straight to the merge. Otherwise: if the current room mode
// (D_00810700) is 0x12, casts a third probe built from arg1+0xB0/+0xB4(+200)/
// +0xB8 out of arg1+0xB0; on a hit, rebuilds the probe from the global
// current-entity (*0x700031D0)+0x24/0x28/0x2C, measures func_00102738
// distance, and picks 0x70003A3C = hitDist - 1.0 if distance > 0.2, else
// arg0+0x14 + 200.0 (arm shared with the non-0x12 / miss cases). If the room
// mode isn't 0x12 (or the third probe missed), 0x70003A3C = arg0+0x14 +
// 200.0. Finally clamps 0x70003A38 to at most 0x70003A3C - 3.0, and writes
// the pair back to arg0+0x50/+0x54.
extern void func_00102948(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern int func_0019A910(void *a, void *b, int mask);
extern float func_00102738(void *a, void *b);

extern unsigned char D_00810700;
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038F0[];

void func_0018D910(char *arg0, char *arg1, int arg2) {
    float probe;
    float y;
    float dist;
    float k;

    func_00102948(D_700038F0, arg1 + 0xA0);
    k = 11.0f;
    *(float *)0x700038F4 = *(float *)0x700038F4 + k;
    func_001028D0(D_700038F0, arg0 + 0x10, D_700038F0);
    func_00102760(D_700038F0, D_700038F0);
    func_001028D0(D_700038F0, arg0 + 0x10, D_700038F0);

    {
        float x = *(float *)0x700038F0;
        float c = 200.0f;
        float y2 = *(float *)0x700038F4;
        float z = *(float *)0x700038F8;
        y2 = y2 - c;
        *(float *)0x700038A0 = x;
        *(float *)0x700038A4 = y2;
        *(float *)0x700038A8 = z;
        *(float *)0x700038AC = 1.0f;
    }

    if (func_0019A910(D_700038F0, D_700038A0, arg2) != 0 &&
        (*(short *)(*(char **)0x700031D0 + 0x1A) & 0x7000) != 0) {
        y = *(float *)0x700031B4;
        if (*(float *)(arg0 + 0x5C) == 1.0f) {
            y = y + 6.0f;
        } else {
            y = y + 17.0f;
        }
    } else {
        y = *(float *)(arg0 + 0x50) - 200.0f;
    }
    *(float *)0x70003A38 = y;

    {
        float x = *(float *)0x700038F0;
        float c = 200.0f;
        float y2 = *(float *)0x700038F4;
        float z = *(float *)0x700038F8;
        y2 = c + y2;
        *(float *)0x700038B0 = x;
        *(float *)0x700038B4 = y2;
        *(float *)0x700038B8 = z;
        *(float *)0x700038BC = 1.0f;
    }

    if (func_0019A910(D_700038F0, D_700038B0, arg2) != 0 &&
        (*(short *)(*(char **)0x700031D0 + 0x1A) & 0x8800) != 0) {
        dist = *(float *)0x700031B4;
        *(float *)0x70003A3C = dist;
        dist = dist - 1.0f;
    } else if (D_00810700 == 0x12) {
        *(float *)0x700038B0 = *(float *)(arg1 + 0xB0);
        *(float *)0x700038B4 = *(float *)(arg1 + 0xB4) + 200.0f;
        *(float *)0x700038B8 = *(float *)(arg1 + 0xB8);
        *(int *)0x700038BC = 0x3F800000;
        if (func_0019A910(arg1 + 0xB0, D_700038B0, arg2) != 0) {
            *(int *)0x700038B0 = 0;
            *(float *)0x700038B4 = -1.0f;
            *(int *)0x700038B8 = 0;
            *(int *)0x700038BC = 0x3F800000;
            probe = *(float *)(*(char **)0x700031D0 + 0x24);
            *(float *)0x700038F0 = probe;
            probe = *(float *)(*(char **)0x700031D0 + 0x28);
            *(float *)0x700038F4 = probe;
            probe = *(float *)(*(char **)0x700031D0 + 0x2C);
            *(float *)0x700038F8 = probe;
            *(int *)0x700038FC = 0x3F800000;
            dist = func_00102738(D_700038B0, D_700038F0);
            *(float *)0x70003A3C = dist;
            if (dist <= 0.2f) {
                dist = *(float *)(arg0 + 0x14) + 200.0f;
            } else {
                dist = *(float *)0x700031B4;
                *(float *)0x70003A3C = dist;
                dist = dist - 1.0f;
            }
        } else {
            dist = *(float *)(arg0 + 0x14) + 200.0f;
        }
    } else {
        dist = *(float *)(arg0 + 0x14) + 200.0f;
    }
    *(float *)0x70003A3C = dist;

    if (*(float *)0x70003A38 > dist) {
        *(float *)0x70003A38 = dist - 3.0f;
    }
    *(float *)(arg0 + 0x50) = *(float *)0x70003A38;
    *(float *)(arg0 + 0x54) = *(float *)0x70003A3C;
}
