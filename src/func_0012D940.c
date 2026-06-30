// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state grenade/projectile behavior driver, dispatched on the state byte at
// arg0+6 (4-way switch 0/1/2/3). Calls func_001C2770(arg0, arg1, 6) up front and
// at the end runs func_001C3D60(arg0, arg1) only if that returned 0.
// State 0: seeds heading arg0+0xC4 from D_00810374 (+pi if the low 3 bits of
//   arg1+0xF6 are >= 4), perturbs it by pi*((rand()&0x1F)-0x10)/180, normalizes
//   via func_001B1470, resets the linked record at arg1 (0xD8=0.5, 0xE4=0x400,
//   identity-ish fields at 0x70..0x88) via func_0012E070+func_001287F0(...,0x1B),
//   then advances the state and marks arg0+0=1.
// State 1: integrates position arg0+0xB0/0xB8 by speed arg0+0x38 along the
//   heading (cos via func_0011E2A8, sin via func_0011DE90), runs func_001B5360,
//   then if arg1+0xE4 == 0x100 picks a follow-up clip from (rand()>>6)&3 and
//   re-states accordingly; else if low nibble set and func_00128640()==0 resets.
// States 2/3: once arg1+0xF4 has the 0x1000 bit, snap orient via
//   func_00128830(self,0,0,1.0) + two func_001287F0 clips (state 2 also flips the
//   heading by pi through func_001B1470), then if func_00128640()==0 reset.
//
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 reaches
// 83.1%. Two keys: (1) func_001C2770 takes the literal 6 as its THIRD arg (it is
// emitted as `addiu a2,zero,6` with a0/a1 left holding the inbound arg0/arg1),
// so it is prototyped `int func_001C2770();` and called func_001C2770(arg0,arg1,6);
// (2) the result must be computed BEFORE the state byte is read (chk= call; then
// st = *(arg0+6)) to reproduce the prologue's load-after-call ordering.
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern int func_00122BB8(void);
extern int func_00128640(char *p);
extern void func_001287F0(char *a, char *b, int idx, float f);
extern void func_00128830(char *a, float x, float y, float z);
extern void func_0012E070(char *p);
extern float func_001B1470(float a);
extern void func_001B5360(char *p);
extern int func_001C2770();
extern void func_001C3D60(char *a, char *b);
extern float D_00810374;

void func_0012D940(char *arg0, char *arg1) {
    int chk;
    int e4;
    int kind;
    unsigned char st;

    chk = func_001C2770(arg0, arg1, 6);
    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        if ((*(short *)(arg1 + 0xF6) & 7) >= 4) {
            *(float *)(arg0 + 0xC4) = 3.1415927f + D_00810374;
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0xC4) + (3.1415927f * (float)((func_00122BB8() & 0x1F) - 0x10)) / 180.0f;
        } else {
            *(float *)(arg0 + 0xC4) = D_00810374;
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0xC4) + (3.1415927f * (float)((func_00122BB8() & 0x1F) - 0x10)) / 180.0f;
        }
        *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4));
        func_0012E070(arg1);
        *(float *)(arg1 + 0xD8) = 0.5f;
        func_001287F0(arg0, arg1, 0x1B, 0.0f);
        *(int *)(arg1 + 0xE4) = 0x400;
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(int *)(arg1 + 0xF0) = 0;
        *(int *)(arg1 + 0x70) = 0;
        *(int *)(arg1 + 0x74) = 0;
        *(float *)(arg1 + 0x78) = 1.0f;
        *(int *)(arg1 + 0x80) = 0;
        *(float *)(arg1 + 0x84) = 1.0f;
        *(int *)(arg1 + 0x88) = 0;
        *(char *)(arg0 + 0) = 1;
        break;
    case 1:
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)(arg0 + 0x38) * func_0011E2A8(*(float *)(arg0 + 0xC4));
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)(arg0 + 0x38) * func_0011DE90(*(float *)(arg0 + 0xC4));
        func_001B5360(arg0);
        e4 = *(int *)(arg1 + 0xE4);
        if (e4 == 0x100) {
            *(int *)(arg0 + 0xC0) = 0;
            *(int *)(arg1 + 0xD8) = 0;
            *(short *)(arg1 + 0xF4) = 0;
            kind = (func_00122BB8() & 0xC0) >> 6;
            switch (kind) {
            case 0:
                func_001287F0(arg0, arg1, 0x11, 0.0f);
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                break;
            case 1:
                func_001287F0(arg0, arg1, 0x1E, 0.0f);
                *(unsigned char *)(arg0 + 6) = 3;
                break;
            case 2:
                func_001287F0(arg0, arg1, 0x1A, 0.0f);
                *(char *)(arg0 + 0) = 2;
                *(char *)(arg0 + 4) = 2;
                *(char *)(arg0 + 5) = 4;
                *(unsigned char *)(arg0 + 6) = 0;
                break;
            case 3:
                func_001287F0(arg0, arg1, 0x12, 0.0f);
                *(unsigned char *)(arg0 + 6) = 3;
                break;
            }
        } else if ((e4 & 0xF) && func_00128640(arg0) == 0) {
            *(char *)(arg0 + 5) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 7) = 0;
        }
        break;
    case 2:
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            func_00128830(arg0, 0.0f, 0.0f, 1.0f);
            func_001287F0(arg0, arg1, 0, 0.0f);
            func_001287F0(arg0, arg1, 1, 6.0f);
            *(float *)(arg0 + 0xC4) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
            if (func_00128640(arg0) == 0) {
                *(char *)(arg0 + 5) = 1;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 7) = 0;
            }
        }
        break;
    case 3:
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            func_00128830(arg0, 0.0f, 0.0f, 1.0f);
            func_001287F0(arg0, arg1, 0, 0.0f);
            func_001287F0(arg0, arg1, 1, 6.0f);
            if (func_00128640(arg0) == 0) {
                *(char *)(arg0 + 5) = 1;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 7) = 0;
            }
        }
        break;
    }
    if (chk == 0) {
        func_001C3D60(arg0, arg1);
    }
}
