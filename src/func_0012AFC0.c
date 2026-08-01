// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: per-state driver for actor arg0 with its controller block arg1,
// dispatched on the state byte at arg0+6 (jump table jtbl_0026D040, 6 states).
// func_001C2770(arg0, arg1, early) is polled once up front (early = state < 4)
// and returns a mode code; mode 0 or 8 runs func_001C3D60(arg0, arg1) as the
// tail. arg0+0x28 is a short dwell counter, arg0+0xC4 the current yaw,
// arg1+0xD0 a short countdown, arg1+0xD8 the move speed and arg1+0xE8 the
// target yaw.
// State 0 picks a countdown of 0x78 or 0xF0 from func_00122BB8 bit 0; if
// func_00128600(0) it plays anim 1 and jumps to state 3, otherwise it picks a
// random target yaw (2*pi * (func_00122BB8() & 0xF0) / 256) and plays anim 2.
// State 1 turns toward the target yaw at 0.06981317 rad/frame and, once
// reached, advances and starts moving at 0.2. States 2-5 all first probe
// forward with func_0012ADC0(arg0, arg0+0xB0, D_008102B0+0xA0, 120.0), bumping
// arg0+0x28 while blocked and calling func_00128640 after 0x5B blocked frames.
// State 2 then either (mode 8) faces away by pi and restarts at state 1, or
// probes arg1+0x50 at -30.0 and either enters state 4 or counts arg1+0xD0 down
// to state 0. States 3 and 5 just count arg1+0xD0 down to state 0; state 4
// turns toward the target yaw as state 1.
//
// NOTE: two spellings are load-bearing. (1) `base` must be DECLARED FIRST among
// the locals -- mwcc colours the callee-saved pointer/int pair in declaration
// order, and the original holds &D_008102B0 in s0 with the mode code in s1.
// (2) the dwell guards must be spelled `> 0x5A`, not `>= 0x5B`: the respelling
// is what makes mwcc 2.3.3 emit the compare into `$at` (`slti at, v0, 0x5b`)
// rather than into the value register.

extern int func_00122BB8(void);
extern int func_00128600(int a);
extern void func_00128640(char *p);
extern void func_001287F0(char *p, char *q, int a, float f);
extern int func_0012ADC0(char *p, char *a, char *b, float f);
extern float func_001B12B0(float a, float b, float c);
extern float func_001B1470(float a);
extern int func_001C2770(char *p, char *q, int a);
extern void func_001C3D60(char *p, char *q);
extern char D_008102B0[];

void func_0012AFC0(char *arg0, char *arg1) {
    char *base;
    int st;
    int mode;
    short t;
    float yaw;

    base = D_008102B0;
    st = *(unsigned char *)(arg0 + 6);
    if (st < 4) {
        mode = func_001C2770(arg0, arg1, 1);
    } else {
        mode = func_001C2770(arg0, arg1, 0);
    }
    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(short *)(arg0 + 0x28) = 0;
        *(float *)(arg1 + 0xD8) = 0.0f;
        if (func_00122BB8() & 1) {
            *(short *)(arg1 + 0xD0) = 0x78;
        } else {
            *(short *)(arg1 + 0xD0) = 0xF0;
        }
        if (func_00128600(0) != 0) {
            func_001287F0(arg0, arg1, 1, 8.0f);
            *(unsigned char *)(arg0 + 6) = 3;
        } else {
            *(float *)(arg1 + 0xE8) = func_001B1470((6.2831855f * (float)(func_00122BB8() & 0xF0)) / 256.0f);
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001287F0(arg0, arg1, 2, 8.0f);
        }
        break;
    case 1:
        yaw = func_001B12B0(*(float *)(arg1 + 0xE8), *(float *)(arg0 + 0xC4), 0.06981317f);
        *(float *)(arg0 + 0xC4) = yaw;
        if (yaw == *(float *)(arg1 + 0xE8)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(float *)(arg1 + 0xD8) = 0.2f;
        }
        break;
    case 2:
        if (func_0012ADC0(arg0, arg0 + 0xB0, base + 0xA0, 120.0f) != 0) {
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) + 1;
            if (*(short *)(arg0 + 0x28) > 0x5A) {
                func_00128640(arg0);
                break;
            }
        } else {
            *(short *)(arg0 + 0x28) = 0;
        }
        if (mode != 8) {
            if (func_0012ADC0(arg0, arg0 + 0xB0, arg1 + 0x50, -30.0f) != 0) {
                *(unsigned char *)(arg0 + 6) = 4;
                *(float *)(arg1 + 0xD8) = 0.0f;
            } else {
                t = *(short *)(arg1 + 0xD0) - 1;
                *(short *)(arg1 + 0xD0) = t;
                if (t == 0) {
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(float *)(arg1 + 0xD8) = 0.0f;
                }
            }
        } else {
            *(float *)(arg1 + 0xE8) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
            *(short *)(arg1 + 0xD0) = 0x28;
            *(unsigned char *)(arg0 + 6) = 1;
            *(float *)(arg1 + 0xD8) = 0.0f;
        }
        break;
    case 3:
        if (func_0012ADC0(arg0, arg0 + 0xB0, base + 0xA0, 120.0f) != 0) {
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) + 1;
            if (*(short *)(arg0 + 0x28) > 0x5A) {
                func_00128640(arg0);
                break;
            }
        } else {
            *(short *)(arg0 + 0x28) = 0;
        }
        t = *(short *)(arg1 + 0xD0) - 1;
        *(short *)(arg1 + 0xD0) = t;
        if (t == 0) {
            *(unsigned char *)(arg0 + 6) = 0;
        }
        break;
    case 4:
        if (func_0012ADC0(arg0, arg0 + 0xB0, base + 0xA0, 120.0f) != 0) {
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) + 1;
            if (*(short *)(arg0 + 0x28) > 0x5A) {
                func_00128640(arg0);
                break;
            }
        } else {
            *(short *)(arg0 + 0x28) = 0;
        }
        yaw = func_001B12B0(*(float *)(arg1 + 0xE8), *(float *)(arg0 + 0xC4), 0.06981317f);
        *(float *)(arg0 + 0xC4) = yaw;
        if (yaw == *(float *)(arg1 + 0xE8)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(float *)(arg1 + 0xD8) = 0.2f;
        }
        break;
    case 5:
        if (func_0012ADC0(arg0, arg0 + 0xB0, base + 0xA0, 120.0f) != 0) {
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) + 1;
            if (*(short *)(arg0 + 0x28) > 0x5A) {
                func_00128640(arg0);
                break;
            }
        } else {
            *(short *)(arg0 + 0x28) = 0;
        }
        t = *(short *)(arg1 + 0xD0) - 1;
        *(short *)(arg1 + 0xD0) = t;
        if (t == 0) {
            *(unsigned char *)(arg0 + 6) = 0;
        }
        break;
    }
    if (mode == 0 || mode == 8) {
        func_001C3D60(arg0, arg1);
    }
}
