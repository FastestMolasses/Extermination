// NEARMISS func_00151940  (vram 0x00151940, 0x504 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Scheduling/regalloc artifacts: (1) `*-1.0f` negate emitted with commuted mul operands (mul f0,f1,f0 vs target f0,f0,f1) at 2 sites; (2) `D_008102B0|=2` uses a different scratch reg; (3) terminal (p+5)/(p+6) store-pairs scheduled in opposite branch-delay-slot order. Body byte-identical otherwise (...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 97.27% (mwcc 2.3.3). Body/logic fully recovered. Residuals are pure
// codegen artifacts: (1) the `* -1.0f` negate is emitted as `mul.s f0,f1,f0`
// (operand commute) vs target `mul.s f0,f0,f1` at two sites; (2) `D_008102B0|=2`
// uses a different scratch reg (regalloc); (3) the terminal `(p+5)=N;(p+6)=0`
// store/branch pairs are scheduled in the opposite slot order. Everything else
// is byte-identical modulo a one-instruction offset cascade. 233 > 991202.
// Enemy turn/face state-step (state in *(u8*)(p+6)): case0 inits anim+fields;
// case1 advances heading toward target using gyro-derived angle, runs LOS/range
// checks (func_0021BE40/func_001A7B80/func_001B1560/func_001B15D0) and picks the
// next behaviour state written to *(p+5).

extern void anim_clip_init(char *p, int clip, float speed, float d);
extern int func_001026A0(int, int, int);
extern int func_001029C0(void *);
extern int func_00102BB0(void *, void *, float);
extern int func_00122BB8(void);
extern int func_001A7B80(char *);
extern float func_001B1240(char *, float, float);
extern float func_001B12B0(float, float, float);
extern float func_001B1470(float);
extern int func_001B1560(char *, float *, float);
extern float func_001B15D0(char *, float *);
extern int func_001B55E0(char *, int);
extern int func_0021BE40(unsigned char *, char *);

extern unsigned char D_008102B0;
extern char D_008102BF;
extern int D_00810320;
extern int D_00810324;
extern int D_00810328;
extern int D_0081032C;
extern float D_00810360;
extern float D_00810368;
extern int D_008104D4;
extern unsigned char D_0081070A;
extern int D_700036A0;

void func_00151940(char *arg0, char *arg1) {
    unsigned char st;
    int flag;
    float a;
    float z;

    st = *(unsigned char *)(arg0 + 6);
    flag = 0;
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 4) = 0;
        *(char *)(arg1 + 0x53) = 1;
        *(float *)(arg1 + 0x20) = 0.0f;
        *(int *)(arg1 + 0x34) = 0;
        *(int *)(arg1 + 0x30) = 0;
        *(int *)(arg1 + 0x3C) = 0;
        *(int *)(arg1 + 0x38) = 0;
        anim_clip_init(arg0, 0, 10.0f, (z = 0.0f));
        if (*(int *)(arg0 + 0x20) != 0) {
            *(char *)(*(int *)(arg0 + 0x20) + 5) = 0xA;
        }
        break;
    case 1:
        *(float *)(arg1 + 0x20) = 0.008726646f + 0.017453292f * ((float)((func_00122BB8() >> 4) & 0xFF) / 255.0f);
        a = *(float *)(arg0 + 0xC4);
        if (!(a <= -1.5533431f) && a < 1.5533431f) {
            if (*(char *)(arg1 + 0x55) & 4) {
                flag = 1;
            }
            if (!(*(float *)(arg0 + 0xB8) < 120.0f)) {
                *(float *)(arg1 + 0x2C) = 0.0f;
            } else if (a == *(float *)(arg1 + 0x2C)) {
                *(float *)0x70003A20 = 0.39269909f + 0.39269909f * ((float)((func_00122BB8() >> 0x13) & 0xFF) / 255.0f);
                if (!(*(float *)(arg0 + 0xC4) < 0.0f)) {
                    *(float *)0x70003A20 = *(float *)0x70003A20 * -1.0f;
                }
                *(float *)(arg1 + 0x2C) = *(float *)0x70003A20;
            }
        } else {
            if (*(char *)(arg1 + 0x55) & 8) {
                flag = 1;
            }
            if (a == *(float *)(arg1 + 0x2C)) {
                *(float *)0x70003A20 = 0.39269909f + 0.39269909f * ((float)((func_00122BB8() >> 0xF) & 0xFF) / 255.0f);
                if (*(float *)(arg0 + 0xC4) < 0.0f) {
                    *(float *)0x70003A20 = *(float *)0x70003A20 * -1.0f;
                }
                *(float *)(arg1 + 0x2C) = func_001B1470(3.1415927f + *(float *)0x70003A20);
            }
        }
        if (func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            D_008102B0 |= 2;
            D_008102BF = 2;
            if (D_0081070A != 0) {
                D_008104D4 = 0x41C80000;
            } else {
                D_008104D4 = 0x41A00000;
            }
            *(float *)0x70003A20 = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)0x70003A20);
            D_00810320 = 0;
            D_00810324 = 0;
            D_00810328 = 0x3F800000;
            D_0081032C = 0x3F800000;
            func_001026A0((int)&D_00810320, (int)&D_700036A0, (int)&D_00810320);
            func_001B55E0(arg0, 1);
        }
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x2C), *(float *)(arg0 + 0xC4), *(float *)(arg1 + 0x20));
        if (flag != 0) {
            *(char *)(arg0 + 5) = 1;
            *(char *)(arg0 + 6) = 0;
            break;
        }
        if (*(short *)(arg1 + 0x44) == 0 && *(char *)(arg1 + 0x58) != 0) {
            *(char *)(arg0 + 5) = 5;
            *(char *)(arg0 + 6) = 0;
            break;
        }
        if (*(short *)(arg1 + 0x42) == 0 && *(int *)0x700031F4 < 0x14) {
            if (*(short *)(arg1 + 0x44) == 0) {
                if ((func_00122BB8() >> 0x12) & 1) {
                    *(char *)(arg0 + 5) = 4;
                } else {
                    *(char *)(arg0 + 5) = 5;
                }
            } else {
                *(char *)(arg0 + 5) = 4;
            }
            *(char *)(arg0 + 6) = 0;
            break;
        }
        if (*(char *)(arg1 + 0x57) == 0 && *(char *)(arg1 + 0x58) == 0 &&
            func_001B1560(arg0, &D_00810360, 1.483529806f) != 0 &&
            func_001B15D0(arg0 + 0xB0, &D_00810360) <= 90.0f) {
            *(char *)(arg0 + 5) = 3;
            *(char *)(arg0 + 6) = 0;
        }
        break;
    }
}
