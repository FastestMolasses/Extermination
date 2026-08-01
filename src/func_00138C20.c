// func_00138C20 — byte-identical match.
//
// objdiff reports slightly under 100% here, and objdiff is WRONG about it.
// The residual is entirely splat rendering a 0x7000xxxx scratchpad access as
// a bare literal in load/store context (it only symbolizes lui+addiu pairs),
// so the EXPECTED object carries a constant where our compiled object carries
// the %hi/%lo relocation pair. Both encode the same bytes once relocated.
// Proven by the stronger oracle: this function is COMPILED and LINKED into the
// boot ELF, which remains byte-identical to the original. That is a direct test
// of the emitted bytes, unlike objdiff's object-level comparison.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// REQUIRES the scratchpad-symbol opt-in to be extended to 0x70003B8A:
//   tools/decomp/build.py  _SPAD_SYMS += "0x70003B8A"
//   config/SCUS_971.12.lcf  D_70003B8A = 0x70003B8A;
// Without it objdiff reports 99.99% (reloc-vs-literal spelling only); the LINKED
// bytes are identical either way. CAUTION: src/func_0013C8C0.c, func_00146F80.c and
// func_001BF6B0.c also mention D_70003B8A — check each before enabling globally.
//
// SEMANTICS: enemy wander/approach brain (arg0 = actor, arg1 = brain block).
// Clamps the arg1+0x44 speed ramp (0.4f cap, +0.05f accel at arg1+0x48), then
// dispatches on actor[6]. State 0 (idle wander): every 64th frame, when the player
// is farther than 70 units, promotes to state 1; otherwise re-rolls the yaw
// (arg1+0x58) and pitch (arg1+0x5C) targets on the arg1+0x20 / arg1+0x22 timers.
// State 1 (approach): drops back to state 0 inside 30 units, else steers yaw/pitch
// at the player with func_001B1240/func_001B1270. Shared tail: the actor+0xD hit
// flag runs the arg1+0x30 stun counter to 0x97 and resets; actor+0xA (or a 0x78
// line-of-sight countdown at arg1+0x2C) switches to state 2 and plays SFX 0x816;
// the arg1+0x86 idle-chirp timer plays 0x826; finally the yaw (actor+0xC4) and
// pitch (arg1+0x50) are slewed by func_001B12B0 and the pose is published by
// func_0013BBB0/func_0013BA20.
//
// Two idioms are load-bearing: (a) D_70003B8A as a symbol, which stops mwcc
// speculating that load's address-`lui` into the two state-dispatch delay slots;
// (b) idiom-28 — the stun test is spelled `t > 0x96`, not `t >= 0x97`, which puts
// the compare temp in $at as the target does.

extern void func_0013C8C0(void);
extern int func_00122BB8(void);
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B1270(void *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_0011DF78(float a);
extern int func_001B13F0(void *a, void *b, float f);
extern int func_001B2F70(void *a, void *b);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern int func_0021BE40(void *a, void *b);
extern int func_0019AFE0(unsigned char *a, void *b, void *c, int d);
extern void func_0013BBB0(unsigned char *a, unsigned char *b);
extern void func_0013BA20(unsigned char *a, unsigned char *b);
extern float D_00810360;
extern float D_008102B0;
extern short D_70003B8A;
extern float D_700038A0;
extern float D_700038B0;

void func_00138C20(unsigned char *arg0, unsigned char *arg1) {
    func_0013C8C0();
    if (*(float *)(arg1 + 0x44) < 0.4f) {
        *(int *)(arg1 + 0x48) = 0x3D4CCCCD;
    } else {
        *(int *)(arg1 + 0x48) = 0;
        *(float *)(arg1 + 0x44) = 0.4f;
    }

    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        if (!((*(int *)0x70003B68 + D_70003B8A) & 0x3F)) {
            *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
            *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
            *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
            *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
            *(float *)0x700038B0 = *(float *)(arg1 + 0);
            *(float *)0x700038B4 = *(float *)(arg1 + 4);
            *(float *)0x700038B8 = *(float *)(arg1 + 8);
            *(float *)0x700038BC = *(float *)(arg1 + 0xC);
            if (func_001B13F0(&D_700038A0, &D_700038B0, 70.0f) == 0) {
                *(unsigned char *)(arg0 + 6) = (*(unsigned char *)(arg0 + 6)) + 1;
                *(short *)(arg1 + 0x20) = 0;
                goto tail;
            }
        }
        if (*(signed char *)(arg1 + 0x80) & 0xC) {
            *(short *)(arg1 + 0x20) = 0x3C;
        } else {
            unsigned short t = *(unsigned short *)(arg1 + 0x20);
            if (t != 0) {
                *(unsigned short *)(arg1 + 0x20) = t - 1;
            } else {
                *(unsigned short *)(arg1 + 0x20) = ((func_00122BB8() >> 0x11) & 0x7F) + 0x3C;
                *(float *)(arg1 + 0x58) = (6.2831855f * ((float)((func_00122BB8() >> 9) & 0xFF) / 255.0f)) - 3.1415927f;
            }
        }
        if (*(signed char *)(arg1 + 0x80) & 3) {
            *(short *)(arg1 + 0x22) = 0x3C;
        } else {
            unsigned short t = *(unsigned short *)(arg1 + 0x22);
            if (t != 0) {
                *(unsigned short *)(arg1 + 0x22) = t - 1;
                if (!(*(float *)(arg1 + 0x5C) < 0.0f)) {
                    goto tail;
                }
                if (func_001B2F70(arg0 + 0xB0, &D_700038A0) == 0) {
                    goto tail;
                }
                if (20.0f + *(float *)0x700038A0 <= *(float *)(arg0 + 0xB4)) {
                    goto tail;
                }
                *(float *)(arg1 + 0x5C) = func_0011DF78(*(float *)(arg1 + 0x5C));
            } else {
                *(unsigned short *)(arg1 + 0x22) = ((func_00122BB8() >> 0xC) & 0x7F) + 0x3C;
                *(float *)(arg1 + 0x5C) = (2.4434612f * ((float)((func_00122BB8() >> 6) & 0xFF) / 255.0f)) - 1.2217306f;
                if (!(*(float *)(arg1 + 0x5C) < 0.0f)) {
                    goto tail;
                }
                if (func_001B2F70(arg0 + 0xB0, &D_700038A0) == 0) {
                    goto tail;
                }
                if (20.0f + *(float *)0x700038A0 <= *(float *)(arg0 + 0xB4)) {
                    goto tail;
                }
                *(float *)(arg1 + 0x5C) = func_0011DF78(*(float *)(arg1 + 0x5C));
            }
        }
        break;
    case 1:
        if (!((*(int *)0x70003B68 + D_70003B8A) & 0x3F)) {
            *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
            *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
            *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
            *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
            *(float *)0x700038B0 = *(float *)(arg1 + 0);
            *(float *)0x700038B4 = *(float *)(arg1 + 4);
            *(float *)0x700038B8 = *(float *)(arg1 + 8);
            *(float *)0x700038BC = *(float *)(arg1 + 0xC);
            if (func_001B13F0(&D_700038A0, &D_700038B0, 30.0f) != 0) {
                *(unsigned char *)(arg0 + 6) = 0;
                *(short *)(arg1 + 0x20) = 0;
                goto tail;
            }
        }
        if (!(*(signed char *)(arg1 + 0x80) & 0xC)) {
            *(float *)(arg1 + 0x58) = func_001B1240(arg0 + 0xB0, *(float *)(arg1 + 0), *(float *)(arg1 + 8));
        }
        if (*(signed char *)(arg1 + 0x80) & 3) {
            *(short *)(arg1 + 0x20) = 0x78;
        } else {
            unsigned short t = *(unsigned short *)(arg1 + 0x20);
            if (t != 0) {
                *(unsigned short *)(arg1 + 0x20) = t - 1;
            } else {
                *(float *)(arg1 + 0x5C) = -1.0f * func_001B1270(arg0 + 0xB0, *(float *)(arg1 + 4), *(float *)(arg1 + 8));
            }
        }
        break;
    }

tail:
    if (*(unsigned char *)(arg0 + 0xD) & 1) {
        unsigned short t = *(unsigned short *)(arg1 + 0x30) + 1;
        *(unsigned short *)(arg1 + 0x30) = t;
        if (t > 0x96) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
        }
    }
    if (*(unsigned char *)(arg0 + 0xA) & 1) {
        *(char *)(arg0 + 5) = 2;
        *(unsigned char *)(arg0 + 6) = 0;
        *(short *)(arg1 + 0x22) = 0;
        *(short *)(arg1 + 0x20) = 0;
        func_001FBD50(arg0, 0x816, 0, 300.0f);
    } else if (*(unsigned short *)(arg1 + 0x2E) == 0) {
        if (func_001B13F0(&D_00810360, arg0 + 0xB0, 150.0f) != 0 &&
            func_0021BE40(&D_008102B0, arg0) == 0 &&
            func_0019AFE0(arg0, arg0 + 0xB0, &D_00810360, 6) == 0) {
            unsigned short t = *(unsigned short *)(arg1 + 0x2C) + 1;
            *(unsigned short *)(arg1 + 0x2C) = t;
            if (t >= 0x78) {
                *(char *)(arg0 + 5) = 2;
                *(unsigned char *)(arg0 + 6) = 0;
                *(short *)(arg1 + 0x22) = 0;
                *(short *)(arg1 + 0x20) = 0;
                func_001FBD50(arg0, 0x816, 0, 300.0f);
            }
        } else {
            *(short *)(arg1 + 0x2C) = 0;
        }
    }
    if (*(signed char *)(arg1 + 0x86) == 0) {
        *(signed char *)(arg1 + 0x86) = func_00122BB8() >> 4;
        func_001FBD50(arg0, 0x826, 0, 300.0f);
    }
    *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x58), *(float *)(arg0 + 0xC4), 3.1415927f / 120.0f);
    *(float *)(arg1 + 0x50) = func_001B12B0(*(float *)(arg1 + 0x5C), *(float *)(arg1 + 0x50), 0.0314159244f);
    func_0013BBB0(arg0, arg1);
    func_0013BA20(arg0, arg1);
}
