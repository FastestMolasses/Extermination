// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: one step of a scripted camera / cutscene command, dispatched on the
// int command id at arg2+8 via jtbl_0026DF70 (9 entries, default returns 1).
// arg0 and arg1 are unused by this function (only $a2 is read).  D_008101E0 is
// the camera block; it is materialised into a base register once up front.
// The return value is a "done" flag: 1 = advance, 0 = keep waiting.
//   0: wait until the float at cam+0x74 has reached cam+0x78; when it has,
//      reset the rig -- func_001B0250(), func_0021B9A0(0, 0.0, 0.0),
//      func_001D2830(2, 0), func_001D25F0(480.0) -- and store the
//      (0, -1, 0, 1) quad at D_008105F0..D_008105FC.  Returns 0 while waiting.
//   7: timed wait -- done once cam+0x74 has reached the float at arg2+0xC.
//   1: func_001B0460(1).
//   2,3: retarget with the current cam+0xC value; 4: with -14.0; 5: with -20.0.
//        All three then func_0018D7B0(cam,5), func_0018D7B0(cam,1) and set the
//        short at cam+0xA0 to 0x78.
//   6: copy D_008105D0 -> cam+0x10 and D_008105E0 -> cam+0x20, set cam+0x54 to
//      100.0 + D_00810354 and cam+0x50 to D_00810354, cam+0xA0 = 0x78.
//   8: set the flag byte D_008101E4.
//
// Case bodies are emitted in the source order 0, 7, 1, 2/3, 4, 5, 6, 8 -- that
// is the order the target lays them out, and it is what the jump table indexes.
//
// The four D_008105Fx quad slots are declared volatile so the post-RA list
// scheduler keeps them in program order; without it mwcc groups the two
// `sw zero` stores (F0, F8) together, which is the only way this block differs.
//

extern void func_00102948(void *dst, void *src);
extern void func_0018CBD0(char *a, char *b, float f);
extern void func_0018D7B0(char *a, int b);
extern void func_001B0250(void);
extern void func_001B0460(int a);
extern void func_001D25F0(float f);
extern void func_001D2830(int a, int b);
extern void func_0021B9A0(int a, float x, float y);

extern char  D_008101E0[];
extern char  D_008101E4;
extern char  D_008102B0[];
extern float D_00810354;
extern char  D_008105D0[];
extern char  D_008105E0[];
extern volatile float D_008105F0;
extern volatile float D_008105F4;
extern volatile float D_008105F8;
extern volatile float D_008105FC;

int func_001B7B30(int arg0, int arg1, char *arg2) {
    char *cam;
    int ret;

    cam = D_008101E0;
    switch (*(int *)(arg2 + 8)) {
    case 0:
        if (!(*(float *)(cam + 0x74) < *(float *)(cam + 0x78))) {
            func_001B0250();
            func_0021B9A0(0, 0.0f, 0.0f);
            func_001D2830(2, 0);
            func_001D25F0(480.0f);
            D_008105F0 = 0.0f;
            D_008105F4 = -1.0f;
            D_008105F8 = 0.0f;
            D_008105FC = 1.0f;
            return 1;
        } else {
            ret = 0;
        }
        return ret;
    case 7:
        if (*(float *)(cam + 0x74) >= *(float *)(arg2 + 0xC)) {
            ret = 1;
        } else {
            ret = 0;
        }
        return ret;
    case 1:
        func_001B0460(1);
        break;
    case 2:
    case 3:
        func_0018CBD0(cam, D_008102B0, *(float *)(cam + 0xC));
        func_0018D7B0(cam, 5);
        func_0018D7B0(cam, 1);
        *(short *)(cam + 0xA0) = 0x78;
        break;
    case 4:
        func_0018CBD0(cam, D_008102B0, -14.0f);
        func_0018D7B0(cam, 5);
        func_0018D7B0(cam, 1);
        *(short *)(cam + 0xA0) = 0x78;
        break;
    case 5:
        func_0018CBD0(cam, D_008102B0, -20.0f);
        func_0018D7B0(cam, 5);
        func_0018D7B0(cam, 1);
        *(short *)(cam + 0xA0) = 0x78;
        break;
    case 6:
        func_00102948(cam + 0x10, D_008105D0);
        func_00102948(cam + 0x20, D_008105E0);
        *(float *)(cam + 0x54) = 100.0f + D_00810354;
        *(float *)(cam + 0x50) = D_00810354;
        *(short *)(cam + 0xA0) = 0x78;
        break;
    case 8:
        D_008101E4 = 1;
        return 1;
    }
    return 1;
}
