// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame timer/state driver for an entity (arg0) and its work struct (arg1).
// If the active-timer short at arg1+0x70 is nonzero: while the float at
// arg1+0x5C is <= 70.0 it zeroes the frame counter at arg1+0x78; otherwise it
// bumps that counter and, once it reaches 0xB4 (180), clears both the timer
// (arg1+0x70) and the counter. If the timer is zero: when bit0 of the entity
// flags byte at arg0+0xA is set it arms the timer to 0xF0 and zeroes the
// counter; otherwise it computes a target yaw from the camera/view struct
// (*(D_00275B40+0x3C)) fields +0xB0/+0xB8 via func_0011E620, subtracts pi/2
// (1.5707964) and feeds func_001B1470, then asks func_001B3F10(entity, yaw,
// 18.0) whether the turn is done. If done, it bumps the counter and once it
// reaches the per-difficulty threshold D_002753C8[D_0081050C & 3] it arms the
// timer to 0xF0 and zeroes the counter; if the turn is not done it zeroes the
// counter.
//
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 99.74% (sole residual is 991202 coloring the slti/slt result in `at`
// where the target uses `v1`; 2.3.3 colors it `v1` and is byte-identical).
// func_001B3F10's 3rd arg is the float 18.0 (0x41900000), not an int; the far
// globals D_0081050C / D_002753C8 are over-declared as arrays so they take
// absolute %hi/%lo addressing rather than gp-relative under -sdatathreshold 4.
extern float func_0011E620(float a, float b);
extern float func_001B1470(float a);
extern int func_001B3F10(char *p, float a, float b);
extern unsigned char D_002753C8[4];
extern char *D_00275B40;
extern unsigned char D_0081050C[8];

void func_001418F0(char *arg0, char *arg1) {
    char *p;
    short t;

    if (*(short *)(arg1 + 0x70) != 0) {
        if (*(float *)(arg1 + 0x5C) <= 70.0f) {
            *(short *)(arg1 + 0x78) = 0;
        } else {
            t = *(short *)(arg1 + 0x78) + 1;
            *(short *)(arg1 + 0x78) = t;
            if (t >= 0xB4) {
                *(short *)(arg1 + 0x70) = 0;
                *(short *)(arg1 + 0x78) = 0;
            }
        }
        return;
    }
    if (*(unsigned char *)(arg0 + 0xA) & 1) {
        *(short *)(arg1 + 0x70) = 0xF0;
        *(short *)(arg1 + 0x78) = 0;
        return;
    }
    p = *(char **)(D_00275B40 + 0x3C);
    if (func_001B3F10(arg0,
            func_001B1470(func_0011E620(*(float *)(p + 0xB0), *(float *)(p + 0xB8)) - 1.5707964f),
            18.0f) != 0) {
        t = *(short *)(arg1 + 0x78) + 1;
        *(short *)(arg1 + 0x78) = t;
        if (t >= D_002753C8[D_0081050C[0] & 3]) {
            *(short *)(arg1 + 0x70) = 0xF0;
            *(short *)(arg1 + 0x78) = 0;
        }
    } else {
        *(short *)(arg1 + 0x78) = 0;
    }
}
