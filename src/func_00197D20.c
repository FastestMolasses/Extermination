// NEARMISS func_00197D20  (vram 0x00197D20, 0x324 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// One compare: the inner switch's last compare (41) is a branch-if-equal to the 41 / 12 body plus a branch to the default, where the target has one branch-if-not-equal to the default; label orders, the default's position, if/else and goto forms and mwcc 2.4 / 991202 tried (an if/else form changes delay-slot filling in the whole function).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// TU_STATIC: func_00191210

//
// Camera hold step (decomp2 lane round 3: body corrected, 89.28% -> 96.36%).
// State byte cam + 1:
//   0: advance, clear +2 and +0x8B, copy player + 0xA0 to D_70003040, fall into 1.
//   1: for player code 0xD / 0x2A advance when player + 0x1F1 == 1 and run
//      func_00197740(cam, player); any other code goes to state 3. Then
//      func_0018D7B0(cam, 2), ease D_008105D0 toward cam + 0x10 (4.0), and when
//      D_008105D4 < player + 0xA4 plus 23.0 and d = func_0011E748(dx*dx + dz*dz)
//      (dx, dz from the player to D_008105D0 / D_008105D8; d is stored at
//      0x70003A20) is below 8.0, re-place D_008105D0 / D_008105D8 at 8.0 times
//      func_0011E2A8 / func_0011DE90 of the angle
//      func_001B1470(func_0011E620(dx, dz)) (stored at 0x70003A24, and re-read
//      from there for the second call). func_00191210().
//   4: clear the zoom (+0x68, func_001D2610(0.0f); for D_00810CA7 == 9 also
//      func_0021B9A0(0, 0.0f, 0.0f) and func_001D2830(1, 1)), fall into 2.
//   2: code 0xC / 0x29: restore D_70003040 (if +0x8B == 0), +6 = 2, +1 = 2,
//      func_001999C0(player, 0), func_00198440(cam, player, 2),
//      func_001912B0(player); 0xD / 0x2A: func_00197870(cam, player, 0) and
//      func_0022E7F0(player) when D_00810CA5 == 6; other: state 3.
//      func_00191210().
//   3: func_00197490(cam, player, 1).
// Body correction from the original instructions: state 3 calls
// func_00197490(cam, player, 1) (the target leaves both incoming arguments in
// $a0 / $a1 and sets $a2; the old C passed only the 1). The port's
// em_camera_aim.c already follows the instructions. Also: every case ends in
// `break` (idiom-27) and the inner switch labels are 41, 12 / default / 42, 13
// (the target compares 13, 42, 12, 41).
// Decomp2 lane round 5 (96.36% -> 98.71%): func_00191210 is a same-TU static
// callee (`// TU_STATIC:`), which reproduces the register use around its two
// calls, and the angle stored at 0x70003A24 is the relocated extern
// D_70003A24 (idiom-32), so the target loads the re-read for func_0011DE90
// before the D_008105D0 store.

extern void func_00102948(void *dst, void *src);
extern float func_0011DE90(float a); /* cos */
extern float func_0011E2A8(float a); /* sin */
extern float func_0011E620(float a, float b);
extern float func_0011E748(float a);
extern void func_0018C4B0(void *dst, float val, float rate);
extern void func_0018C6A0(void *dst, void *src, float rate);
extern int func_0018D7B0(char *a, int b);
static void func_00191210(void);
#include "src/func_00191210.c"
extern void func_001912B0(char *a);
extern void func_00197490(char *, char *, int);
extern void func_00197740(char *a, char *b);
extern void func_00197870(char *a, char *b, int c);
extern void func_00198440(float *, int, int);
extern void func_001999C0(char *a, int b);
extern float func_001B1470(float a);
extern void func_001D2610(float a);
extern void func_001D2830(int a, int b);
extern void func_0021B9A0(int a, float b, float c);
extern void func_0022E7F0(char *a);

extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA7;
extern char D_70003040[];
extern float D_70003A24;

void func_00197D20(char *arg0, char *arg1) {
    unsigned char st;
    int mode;
    float dx, dz, d2, ang;

    st = *(unsigned char *)(arg0 + 1);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 1) = st + 1;
        *(char *)(arg0 + 2) = 0;
        *(unsigned char *)(arg0 + 0x8B) = 0;
        func_00102948(&D_70003040, arg1 + 0xA0);
        /* fallthrough */
    case 1:
        mode = *(int *)(arg1 + 0x230);
        if (mode != 0xD && mode != 0x2A) {
            *(unsigned char *)(arg0 + 1) = 3;
        } else {
            if (*(unsigned char *)(arg1 + 0x1F1) == 1) {
                *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
            }
            func_00197740(arg0, arg1);
        }
        func_0018D7B0(arg0, 2);
        func_0018C6A0(arg0 + 0x10, &D_008105D0, 4.0f);
        func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 4.0f);
        if (D_008105D4 < 23.0f + *(float *)(arg1 + 0xA4)) {
            dx = D_008105D0 - *(float *)(arg1 + 0xA0);
            dz = D_008105D8 - *(float *)(arg1 + 0xA8);
            d2 = func_0011E748(dx * dx + dz * dz);
            *(float *)0x70003A20 = d2;
            if (d2 < 8.0f) {
                ang = func_001B1470(func_0011E620(dx, dz));
                D_70003A24 = ang;
                D_008105D0 = *(float *)(arg1 + 0xA0) + 8.0f * func_0011E2A8(ang);
                D_008105D8 = *(float *)(arg1 + 0xA8) + 8.0f * func_0011DE90(D_70003A24);
            }
        }
        func_00191210();
        break;
    case 4:
        *(int *)(arg0 + 0x68) = 0;
        func_001D2610(0.0f);
        if (D_00810CA7 == 9) {
            func_0021B9A0(0, 0.0f, 0.0f);
            func_001D2830(1, 1);
        }
        /* fallthrough */
    case 2:
        mode = *(int *)(arg1 + 0x230);
        switch (mode) {
        case 41:
        case 12:
            if (*(unsigned char *)(arg0 + 0x8B) == 0) {
                func_00102948(arg1 + 0xA0, &D_70003040);
            }
            *(char *)(arg0 + 6) = 2;
            *(unsigned char *)(arg0 + 1) = 2;
            func_001999C0(arg1, 0);
            func_00198440((float *)arg0, (int)arg1, 2);
            func_001912B0(arg1);
            break;
        default:
            *(unsigned char *)(arg0 + 1) = 3;
            break;
        case 42:
        case 13:
            func_00197870(arg0, arg1, 0);
            if (D_00810CA5 == 6) {
                func_0022E7F0(arg1);
            }
            break;
        }
        func_00191210();
        break;
    case 3:
        func_00197490(arg0, arg1, 1);
        break;
    }
}
