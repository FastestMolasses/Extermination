// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), decomp2 lane round 3 (was an 83.20% NEARMISS).
// Weapon sight switch: arg0 is the player, arg1 a flag (0 takes the clear
// path of each arm, nonzero the setup path). With arg1 set, func_001D2040(1, 0)
// first. Dispatches on D_00810CA4:
//   2 (missile launcher): clear func_001DB800(5), or func_001DB830(1, 0xFF, 1.0f),
//     sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER(1, 1.0f), func_00199770(player).
//   1: clear func_001DB800(4), or func_001DB9D0(1, 1.0f), func_001DC610(1, 1.0f),
//     func_001DC890(1, zoom) with zoom = *(float *)(player[0x20] + 0x1F0 + 0x24).
//   0: clear func_001DB800(3), or func_001DB9D0(1, 1.0f),
//     sub_DELTA_AUTO_SIGHT_SYS(1, 1.0f), func_001DC020(1, zoom).
//   These three return 0 at once.
//   Otherwise on D_00810CA7: 8: clear func_001DB800(1), or func_001DB9D0(1, 1.0f),
//     sub_TACTICAL_ADVANCED(1, 1.0f), func_001DBCB0(1, D_00810248),
//     func_001D2610(D_00810248) and the result 1; 9: clear func_001DB800(2),
//     func_0021B9A0(4, 0.0f, 1000000.0f), func_0021B9A0(5, 0.0f, 10000000.0f),
//     func_001D2830(1, 0), or func_001DB830(1, 0xFF, 1.0f),
//     sub_NIGHT_VISION_SYSTEM(1, 1.0f); other: clear func_001DB800(0).
//   Only this second group reaches the D_00810CA5 == 6 call
//   func_0022E7F0(&D_008102B0); then the result is returned.
// Body corrections from the original instructions (the port's em_camera_aim.c
// already follows them): the D_00810CA4 arms skip the D_00810CA5 test (the old
// C ran it for every arm); func_001D2830 gets (1, 0); every sight call passes
// 1.0f in $f12. The six sight setup functions (func_001DB830,
// sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER, func_001DC610, sub_DELTA_AUTO_SIGHT_SYS,
// sub_TACTICAL_ADVANCED, sub_NIGHT_VISION_SYSTEM) never read it: their
// definitions now take it as an unused float. func_001DB9D0 (int, float) does
// read it: the 1.0f is its dt. func_001DB800 (which never reads $a0) gets the
// value each arm sets in $a0 (5, 4, 3, then 1 and 2, the values already in
// $a0 / $a1, and 0), which the old header called "dead constants"; the 2 in $a1
// is the D_00810CA4 compare constant.

extern void func_001D2040(int a0, int a1);
extern void func_001DB800(int a0);
extern void func_001DB830(int a0, int a1, float f);
extern void sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER(int a0, float f);
extern void func_00199770(char *arg0);
extern void func_001DB9D0(int a0, float a1);
extern void func_001DC610(int a0, float f);
extern void func_001DC890(int a0, float a1);
extern void sub_DELTA_AUTO_SIGHT_SYS(int a0, float f);
extern void func_001DC020(int a0, float a1);
extern void sub_TACTICAL_ADVANCED(int a0, float f);
extern void func_001DBCB0(int a0, float a1);
extern void func_001D2610(float a0);
extern void func_0021B9A0(int a0, float a1, float a2);
extern void func_001D2830(int a0, int a1);
extern void sub_NIGHT_VISION_SYSTEM(int a0, float f);
extern void func_0022E7F0(void *a0);
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA7;
extern float D_00810248;
extern int D_008102B0;

int func_001999C0(char *arg0, int arg1) {
    char *scratch;
    int result;

    scratch = (char *)(*(int *)(arg0 + 0x20) + 0x1F0);
    result = 0;
    if (arg1 != 0) {
        func_001D2040(1, 0);
    }
    if (D_00810CA4 == 2) {
        if (arg1 == 0) {
            func_001DB800(5);
        } else {
            func_001DB830(1, 0xFF, 1.0f);
            sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER(1, 1.0f);
            func_00199770(arg0);
        }
        goto done;
    } else if (D_00810CA4 == 1) {
        if (arg1 == 0) {
            func_001DB800(4);
        } else {
            func_001DB9D0(1, 1.0f);
            func_001DC610(1, 1.0f);
            func_001DC890(1, *(float *)(scratch + 0x24));
        }
        goto done;
    } else if (D_00810CA4 == 0) {
        if (arg1 == 0) {
            func_001DB800(3);
        } else {
            func_001DB9D0(1, 1.0f);
            sub_DELTA_AUTO_SIGHT_SYS(1, 1.0f);
            func_001DC020(1, *(float *)(scratch + 0x24));
        }
        goto done;
    } else if (D_00810CA7 == 8) {
        if (arg1 == 0) {
            func_001DB800(1);
        } else {
            func_001DB9D0(1, 1.0f);
            sub_TACTICAL_ADVANCED(1, 1.0f);
            func_001DBCB0(1, D_00810248);
            func_001D2610(D_00810248);
            result = 1;
        }
    } else if (D_00810CA7 == 9) {
        if (arg1 == 0) {
            func_001DB800(2);
            func_0021B9A0(4, 0.0f, 1000000.0f);
            func_0021B9A0(5, 0.0f, 10000000.0f);
            func_001D2830(1, 0);
        } else {
            func_001DB830(1, 0xFF, 1.0f);
            sub_NIGHT_VISION_SYSTEM(1, 1.0f);
        }
    } else if (arg1 == 0) {
        func_001DB800(0);
    }
    if (D_00810CA5 == 6) {
        func_0022E7F0(&D_008102B0);
    }
done:
    return result;
}
