// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame enemy "flee/panic" behavior handler. arg0 is the actor, arg1 the
// paired control record. Branches on the flag byte at arg0+0xD.
//   flags & 2 (special/blocked): if the pitch(?) field arg0+0xB4 has dropped below
//     50.0 it resets the actor's aim block (arg1+0x6A=0; bumps the counter at
//     arg0+0x36 = arg0+0x34 + 1; installs a canned aim quat 0x44079333 / 55.0f /
//     0x438A3333 / 1.0f at arg0+0xB0..0xBC). Otherwise, once the global gauge
//     D_00810354 has reached >= 47.0 and func_001B1EA0(0, tbl D_00810360,
//     D_00245020, 0x10) succeeds, it arms the 0x258 timer at arg1+0x56 and returns 1.
//     Either way it clears arg1+0x56 and returns 0.
//   flags & 2 clear, timer arg1+0x56 running: if the range field arg1+0x44 is
//     within 60.0 it clears the retry counter arg1+0x6F; else it counts the retry
//     up and, at 0x78 (120), gives up (clears both the timer and the counter).
//   idle: if arg0+0xA bit0 is set it arms the 0x78 timer and returns 1.
//   otherwise it computes the bearing to the linked target at arg0+0x13C
//     (atan2 via func_0011E620, minus pi/2, through func_001B1470) and feeds it to
//     the aim/turn test func_001B3F10(self, bearing, 20.0f). On a hit it counts the
//     retry up and, once it reaches the per-difficulty limit D_00275398[D_0081050C&3],
//     re-arms the 0x78 timer and returns 1; a miss clears the retry counter.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 91.3%. Two keys: (1) -sdatathreshold 4 keeps the 4-byte difficulty table
// D_00275398 gp-relative, while D_00810354/D_00810360/D_00245020/D_0081050C are
// over-declared as arrays > 4 bytes so they stay %hi/%lo absolute, matching the
// target's addressing exactly; (2) the func_001B3F10 tail folds its miss branch and
// both success/fall-through paths onto one shared `return 0`, reproducing the single
// epilogue the target schedules. The `flags & 1` if/else deliberately holds two
// identical arms -- the target genuinely emits the bearing computation twice.
// objdiff 100.0 vs build/expected/func_001333F0.o.
extern float func_0011E620(float y, float x);
extern float func_001B1470(float a);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern int func_001B3F10(char *p, float a, float b);
extern char D_00245020[8];
extern unsigned char D_00275398[4];
extern float D_00810354[8];
extern char D_00810360[8];
extern unsigned char D_0081050C[8];

int func_001333F0(char *arg0, char *arg1) {
    unsigned char flags;
    unsigned char c;
    float ang;
    char *sub;

    flags = *(unsigned char *)(arg0 + 0xD);
    if (flags & 2) {
        if (!(*(float *)(arg0 + 0xB4) < 50.0f)) {
            if (!(D_00810354[0] < 47.0f)) {
                if (func_001B1EA0(0, D_00810360, D_00245020, 0x10) != 0) {
                    *(unsigned short *)(arg1 + 0x56) = 0x258;
                    return 1;
                }
            }
        } else {
            *(char *)(arg1 + 0x6A) = 0;
            *(short *)(arg0 + 0x36) = *(short *)(arg0 + 0x34) + 1;
            *(int *)(arg0 + 0xB0) = 0x44079333;
            *(float *)(arg0 + 0xB4) = 55.0f;
            *(int *)(arg0 + 0xB8) = 0x438A3333;
            *(int *)(arg0 + 0xBC) = 0x3F800000;
        }
        *(unsigned short *)(arg1 + 0x56) = 0;
        return 0;
    }
    if (*(unsigned short *)(arg1 + 0x56) != 0) {
        if (*(float *)(arg1 + 0x44) <= 60.0f) {
            *(unsigned char *)(arg1 + 0x6F) = 0;
        } else {
            c = *(unsigned char *)(arg1 + 0x6F) + 1;
            *(unsigned char *)(arg1 + 0x6F) = c;
            if ((int)(c & 0xFF) >= 0x78) {
                *(unsigned short *)(arg1 + 0x56) = 0;
                *(unsigned char *)(arg1 + 0x6F) = 0;
            }
        }
        return 0;
    }
    if (*(unsigned char *)(arg0 + 0xA) & 1) {
        *(unsigned short *)(arg1 + 0x56) = 0x78;
        *(unsigned char *)(arg1 + 0x6F) = 0;
        return 1;
    }
    if (flags & 1) {
        sub = *(char **)(arg0 + 0x13C);
        ang = func_001B1470(func_0011E620(*(float *)(sub + 0xB0), *(float *)(sub + 0xB8)) - 1.5707964f);
    } else {
        sub = *(char **)(arg0 + 0x13C);
        ang = func_001B1470(func_0011E620(*(float *)(sub + 0xB0), *(float *)(sub + 0xB8)) - 1.5707964f);
    }
    if (func_001B3F10(arg0, ang, 20.0f) != 0) {
        c = *(unsigned char *)(arg1 + 0x6F) + 1;
        *(unsigned char *)(arg1 + 0x6F) = c;
        if ((int)(c & 0xFF) >= D_00275398[D_0081050C[0] & 3]) {
            *(unsigned short *)(arg1 + 0x56) = 0x78;
            *(unsigned char *)(arg1 + 0x6F) = 0;
            return 1;
        }
    } else {
        *(unsigned char *)(arg1 + 0x6F) = 0;
    }
    return 0;
}
