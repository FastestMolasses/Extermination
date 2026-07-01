// NEARMISS func_0014CDD0  (vram 0x0014CDD0, 0x408 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 94.65% (233; 991202=84.36%). Body/logic and all data fully recovered (verified against sibling func_001B2B10.c/func_00110C48.c prototypes and FINDINGS.md field docs). Residual causes: (1) mwcc cross-jump/tail-merges the top-level cnt==0 early-exit block into one of the five identical later 'arg0[...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Attack/recoil state driver. arg0 = entity, arg1 = weapon/attack block. If
// arg1+0x18 (a burst/ammo counter) is zero, enters state 2 (idle) and
// returns. Otherwise decrements it and dispatches: if arg0+0xB4 (a timer) is
// <= 5.0f, fires a one-shot muzzle-flash/sound cue (func_00102948 seeds the
// scratch vec from arg0+0xB0, func_001EFD20(0x8000005F,...) plays a sound,
// func_001FBD50(arg0, 0x872, 0, 800.0f) triggers an effect) and enters state
// 3. Else if arg1+0x41 is zero (fresh burst), seeds the scratch quat
// identity, blends it against arg0+0xB0 via func_001B2B10/func_001028B8, and
// tests func_0019AD00(arg0,&scratch,6); on hit, plays the per-weapon-type
// impact cue (D_00245CE0[arg0[0xD]]) via func_001EFD90 and enters state 2.
// If arg1+0x24 is negative (still recovering), builds a second scratch pose
// pair and tests func_0019AB20(arg0,&scratch,&scratch2,7); on hit, same
// impact-cue-and-state-2 exit. Otherwise falls through to the shared
// "block_11" guard: func_0021BE40(&D_008102B0,arg0)==0 (no player hit) AND
// func_001A7B80(arg0)!=0 (some secondary check) triggers the impact cue,
// enters state 2, sets the player's D_008102B0 mode bit 2, and seeds two
// difficulty-scaled globals D_008104D4/D_008104DC (harder values when
// D_0081070A is set). If that guard doesn't fire, checks arg0+0x36 (a
// windup counter): if zero, returns; else clamps it against arg0+0x34 (a
// budget) — if the windup has exceeded budget, plays the impact cue and
// enters state 2; otherwise sets arg0+0 = 1 (attack committed) and folds
// the windup back into the budget (arg0+0x34 -= arg0+0x36, then zeroes
// arg0+0x36).
extern void func_00102948(void *dst, void *src);
extern void func_001EFD20(int a, void *b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_001B2B10(char *arg0, int arg1, int arg2);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019AD00(char *p, void *a, int b);
extern void func_001EFD90(int a, void *b, void *c);
extern int func_0019AB20(void *p, void *a, void *b, int c);
extern int func_0021BE40(void *a, char *p);
extern int func_001A7B80(char *p);

extern int D_00245CE0[];
extern unsigned char D_008102B0;
extern int D_008104D4;
extern int D_008104DC;
extern unsigned char D_0081070A;
extern char D_700038A0[];
extern char D_700038B0[];

void func_0014CDD0(char *arg0, char *arg1) {
    int cnt;
    short wind;
    short budget;

    cnt = *(int *)(arg1 + 0x18);
    if (cnt == 0) {
        *(char *)(arg0 + 4) = 2;
    } else {
        *(int *)(arg1 + 0x18) = cnt - 1;

        if (*(float *)(arg0 + 0xB4) <= 5.0f) {
            func_00102948(D_700038A0, arg0 + 0xB0);
            *(int *)0x700038A4 = 0x41200000;
            func_001EFD20(0x8000005F, D_700038A0);
            func_001FBD50(arg0, 0x872, 0, 800.0f);
            *(char *)(arg0 + 4) = 3;
            return;
        }

        if (*(char *)(arg1 + 0x41) == 0) {
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0;
            *(int *)0x700038A8 = 0x40800000;
            *(int *)0x700038AC = 0x3F800000;
            func_001B2B10(arg0, (int)D_700038A0, (int)D_700038A0);
            func_001028B8(D_700038A0, arg0 + 0xB0, D_700038A0);
            if (func_0019AD00(arg0, D_700038A0, 6) != 0) {
                *(int *)0x700038A8 = 0;
                *(int *)0x700038A4 = 0;
                *(int *)0x700038A0 = 0;
                *(int *)0x700038AC = 0x3F800000;
                func_001EFD90(D_00245CE0[*(unsigned char *)(arg0 + 0xD)], arg0 + 0xB0, D_700038A0);
                *(char *)(arg0 + 4) = 2;
                return;
            }

            if (*(float *)(arg1 + 0x24) < 0.0f) {
                *(int *)0x700038A0 = 0;
                *(int *)0x700038B0 = 0;
                *(int *)0x700038A4 = 0xC0000000;
                *(int *)0x700038A8 = 0;
                *(int *)0x700038AC = 0x3F800000;
                *(int *)0x700038B4 = 0xC1000000;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x3F800000;
                func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
                if (func_0019AB20(arg0, D_700038A0, D_700038B0, 7) != 0) {
                    *(int *)0x700038A8 = 0;
                    *(int *)0x700038A4 = 0;
                    *(int *)0x700038A0 = 0;
                    *(int *)0x700038AC = 0x3F800000;
                    func_001EFD90(D_00245CE0[*(unsigned char *)(arg0 + 0xD)], arg0 + 0xB0, D_700038A0);
                    *(char *)(arg0 + 4) = 2;
                    return;
                }
            }
        }

        if (func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            *(int *)0x700038A8 = 0;
            *(int *)0x700038A4 = 0;
            *(int *)0x700038A0 = 0;
            *(int *)0x700038AC = 0x3F800000;
            func_001EFD90(D_00245CE0[*(unsigned char *)(arg0 + 0xD)], arg0 + 0xB0, D_700038A0);
            *(char *)(arg0 + 4) = 2;
            D_008102B0 |= 2;
            if (D_0081070A != 0) {
                D_008104D4 = 0x41A00000;
                D_008104DC = 0x41C80000;
            } else {
                D_008104D4 = 0x41700000;
                D_008104DC = 0x41A00000;
            }
            return;
        }

        wind = *(short *)(arg0 + 0x36);
        if (wind == 0) {
            return;
        }
        budget = *(short *)(arg0 + 0x34);
        if ((wind & 0xFFF) >= budget) {
            *(int *)0x700038A8 = 0;
            *(int *)0x700038A4 = 0;
            *(int *)0x700038A0 = 0;
            *(int *)0x700038AC = 0x3F800000;
            func_001EFD90(D_00245CE0[*(unsigned char *)(arg0 + 0xD)], arg0 + 0xB0, D_700038A0);
            *(char *)(arg0 + 4) = 2;
            return;
        }
        *(char *)arg0 = 1;
        *(short *)(arg0 + 0x34) = budget - (wind & 0xFFF);
        *(short *)(arg0 + 0x36) = 0;
    }
}
