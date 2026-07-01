// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Combat-target state machine dispatched on the state byte at arg0+6 (states
// 0-3). State 0: bumps the state, clears the substate byte +7, clip +0x2FE
// countdown, and the timer short +0x28, kicks clip 0x35 via
// func_001749A0(arg0, 0x35, 0, 0.0f), copies the 0x24C float into +0x26C, and
// zeroes the int at +0x2EC. State 1: decrements the +0x2FE countdown byte
// (resetting the timer +0x28 when it hits 0), re-evaluates clip via
// func_00174AC0; if func_001754E0(arg0, 8) fires or the global flag byte at
// 0x70003B8D is set, advances state, kicks clip 0x38 at blend 4.0f, sets
// D_008106BD, and zeroes the float at +0x224. Otherwise re-copies +0x24C into
// +0x26C, and if +0x224 is nonzero calls func_0021C350(arg0): if the result
// float +0x220 is <= 0 the target is downed (fields +4/+5/+6/+0x1F0 set and
// early return), else it arms a hit-reaction sequence (func_001B61C0,
// func_001FBD50, clip 0x36) and sets substate +7 = 1. A nested substate
// switch on +7 sets +0x204 to 0x40000000/0x3F000000 depending on the timer
// +0x28 (substate 0), or on substate 1 watches flag bit 0x1000 of +0x200 to
// fire clip 0x35 and clear the substate. State 2: advances to state 3 unless
// flag bit 0x8000 of +0x200 is set. State 3: on flag bit 0x1000 of +0x200,
// resets the 0x20E timer/0x25C byte and calls func_0017C540(). All paths
// (except the state-1 early-return) fall through to
// func_00179880(arg0, arg0+0x2EC); func_00175900(arg0, 1).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 75.1%. Two keys: (1) the first func_001749A0 call in state 0 leaves
// $a0 untouched from the caller's arg0 (mwcc never re-materializes it), so
// the C call must pass `arg0` explicitly even though the disassembly never
// shows an explicit a0 move for that jal; (2) the `+0x224 != 0.0f` compare
// needs the literal folded into a named `float zero = 0.0f;` local to get the
// target's `c.eq.s f1,f0` operand order (a raw `!= 0.0f` literal emits the
// operands swapped).
extern void func_001749A0(char *p, int a, int b, float f);
extern void func_00174AC0(char *p, int a);
extern int func_001754E0(char *p, int a);
extern void func_00175900(char *p, int a);
extern void func_00179880(char *p, char *q);
extern void func_0017C540(void);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_0021C350(char *p);
extern unsigned char D_008106BD;

void func_002208C0(char *arg0) {
    unsigned char st;
    unsigned char st2;
    char cnt;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(unsigned char *)(arg0 + 7) = 0;
        func_001749A0(arg0, 0x35, 0, 0.0f);
        *(short *)(arg0 + 0x28) = 0;
        *(char *)(arg0 + 0x2FE) = 0;
        func_00174AC0(arg0, 0);
        *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        *(int *)(arg0 + 0x2EC) = 0;
        break;
    case 1:
        cnt = *(char *)(arg0 + 0x2FE);
        *(char *)(arg0 + 0x2FE) = cnt - 1;
        if (cnt == 0) {
            *(char *)(arg0 + 0x2FE) = 0;
            *(short *)(arg0 + 0x28) = 0;
        }
        func_00174AC0(arg0, 0);
        if (func_001754E0(arg0, 8) != 0 || *(unsigned char *)0x70003B8D != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001749A0(arg0, 0x38, 0, 4.0f);
            D_008106BD = 1;
            *(float *)(arg0 + 0x224) = 0.0f;
            break;
        }
        *(float *)(arg0 + 0x26C) = *(float *)(arg0 + 0x24C);
        {
            float zero = 0.0f;
            if (*(float *)(arg0 + 0x224) != zero) {
                func_0021C350(arg0);
                if (*(float *)(arg0 + 0x220) <= 0.0f) {
                    *(char *)(arg0 + 4) = 2;
                    *(char *)(arg0 + 5) = 1;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(char *)(arg0 + 0x1F0) = 0x40;
                    return;
                }
                func_001B61C0(0, 0xC0, 5, 1);
                func_001FBD50(arg0, 0x154, 0, 300.0f);
                func_001749A0(arg0, 0x36, 1, 1.0f);
                *(unsigned char *)(arg0 + 7) = 1;
            }
        }
        st2 = *(unsigned char *)(arg0 + 7);
        switch (st2) {
        case 0:
            if (*(short *)(arg0 + 0x28) != 0) {
                *(int *)(arg0 + 0x204) = 0x40000000;
            } else {
                *(int *)(arg0 + 0x204) = 0x3F000000;
            }
            break;
        case 1:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_001749A0(arg0, 0x35, 0, 1.0f);
                *(unsigned char *)(arg0 + 7) = 0;
            }
            break;
        }
        break;
    case 2:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
        }
        break;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(char *)(arg0 + 0x25C) = 0;
            func_0017C540();
        }
        break;
    }
    func_00179880(arg0, arg0 + 0x2EC);
    func_00175900(arg0, 1);
}
