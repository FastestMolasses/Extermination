// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), decomp2 lane round 3 (was a 94.17% NEARMISS).
// Camera action 2 (aim): state byte cam + 1.
//   0: clear +8, advance, clear +2 and the zoom ramp +0x68, func_001912B0(player),
//      clear +0x8B, copy player + 0xA0 to D_70003040, fall into 1.
//   1: unless the player code (+0x230) is 0xC or 0x29, go to state 3; else
//      func_00198050(cam, player) and, when player + 0x1F1 == 1, advance,
//      func_001999C0(player, 0) and func_00198440(cam, player, 1).
//   2: for code 0xC / 0x29: func_00198440(cam, player, 0), and when
//      func_001999C0(player, 1) == 1 ramp +0x68 by +-0.02 within [0, 1] from pad
//      bits 4 / 1 of D_00810E70. Otherwise func_001999C0(player, 1); for code
//      0xD / 0x2A restore D_70003040 (if +0x8B == 0), set +6 = 1, +1 = 4 and
//      func_00197870(cam, player, 1); for any other code reset the zoom
//      (func_001D2610(0.0f), func_001D2830(1, 1) when D_00810CA7 == 9) and
//      func_00197490(cam, player, 0).
//   3: advance, reset the zoom as above, fall into 4: func_00197490(cam, player, 0).
// How it matches: func_00102948 takes two pointers (the old C's extra 1 was the
// $a2 of the following func_00197870 call, set in a delay slot); every case
// ends in `break` (idiom-27), which gives the target's empty delay slots the old
// header called idiom-13 walls; the +0.02 ramp is `+=` (idiom-21).

extern void func_00102948(void *, void *);
extern void func_001912B0(char *);
extern void func_00197490(char *, char *, int);
extern void func_00197870(char *, char *, int);
extern void func_00198050(char *, char *);
extern void func_00198440(float *, int, int);
extern int func_001999C0(char *, int);
extern void func_001D2610(float);
extern void func_001D2830(int, int);
extern unsigned char D_00810CA7;
extern unsigned short D_00810E70;
extern int D_70003040;

void func_00198650(char *arg0, char *arg1) {
    unsigned char st = *(unsigned char *)(arg0 + 1);
    float f;

    switch (st) {
    default:
        break;
    case 0:
        *(short *)(arg0 + 8) = 0;
        *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
        *(char *)(arg0 + 2) = 0;
        *(float *)(arg0 + 0x68) = 0.0f;
        func_001912B0(arg1);
        *(unsigned char *)(arg0 + 0x8B) = 0;
        func_00102948(&D_70003040, arg1 + 0xA0);
        /* fallthrough */
    case 1:
        if (*(int *)(arg1 + 0x230) != 0xC && *(int *)(arg1 + 0x230) != 0x29) {
            *(unsigned char *)(arg0 + 1) = 3;
            break;
        }
        func_00198050(arg0, arg1);
        if (*(unsigned char *)(arg1 + 0x1F1) == 1) {
            *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
            func_001999C0(arg1, 0);
            func_00198440((float *)arg0, (int)arg1, 1);
        }
        break;
    case 2: {
        int st230 = *(int *)(arg1 + 0x230);
        if (st230 != 0xC && st230 != 0x29) {
            func_001999C0(arg1, 1);
            st230 = *(int *)(arg1 + 0x230);
            if (st230 == 0xD || st230 == 0x2A) {
                if (*(unsigned char *)(arg0 + 0x8B) == 0) {
                    func_00102948(arg1 + 0xA0, &D_70003040);
                }
                *(unsigned char *)(arg0 + 0x6) = 1;
                *(unsigned char *)(arg0 + 1) = 4;
                func_00197870(arg0, arg1, 1);
                break;
            }
            *(float *)(arg0 + 0x68) = 0.0f;
            func_001D2610(0.0f);
            if (D_00810CA7 == 9) {
                func_001D2830(1, 1);
            }
            func_00197490(arg0, arg1, 0);
            break;
        }
        func_00198440((float *)arg0, (int)arg1, 0);
        if (func_001999C0(arg1, 1) == 1) {
            if (D_00810E70 & 4) {
                f = *(float *)(arg0 + 0x68) += 0.02f;
                if (!(f <= 1.0f)) {
                    *(float *)(arg0 + 0x68) = 1.0f;
                }
            } else if (D_00810E70 & 1) {
                f = *(float *)(arg0 + 0x68) - 0.02f;
                *(float *)(arg0 + 0x68) = f;
                if (f < 0.0f) {
                    *(float *)(arg0 + 0x68) = 0.0f;
                }
            }
        }
        break;
    }
    case 3:
        *(unsigned char *)(arg0 + 1) = st + 1;
        *(float *)(arg0 + 0x68) = 0.0f;
        func_001D2610(0.0f);
        if (D_00810CA7 == 9) {
            func_001D2830(1, 1);
        }
        /* fallthrough */
    case 4:
        func_00197490(arg0, arg1, 0);
        break;
    }
}
