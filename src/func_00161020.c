// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.50% NEARMISS).
// Fixes read from the original instructions:
//  - the guard failures in states 1 and 2 (D_0028A9A0 set, func_001607D0 or
//    func_00160220 nonzero) RETURN at once and skip the shared per-frame tail;
//    the old C broke out of the switch and ran the tail
//  - func_001607D0 takes the entity pointer (the old C passed the state byte)
//  - func_00174AC0's result is used unmasked here: declared as returning int
//  - the sub-state test on e[7] is a switch (cases 0 and 1)
//  - the e+0x240 test against 0.0f uses a `float zero` local (operand order)
//
// Entity behaviour state machine dispatched on the state byte at e+6.
// State 0 initialises (state 1, sub-state 0, clears e+0x38 / e+0x25C, blends
// to clip with 12.0, delay counter e+0x28 = 300). State 1 waits: when
// func_00174AC0(e, 0) reports done it advances and calls func_0017B5C0;
// otherwise sub-state 0 counts e+0x28 down (only while e+0x236 is clear and
// bit 0 of e+0x235 is clear) and on expiry plays clip 0x15D, and sub-state 1
// waits for flag 0x1000 at e+0x200 to restart the countdown. State 2 waits
// for flag 0x8000 to clear, then goes to state 0x63 when e+0x240 is zero,
// otherwise hands over (mode 1) or clears e+0x204. States 0x63/0x64 blend
// and return to state 1. Every non-returning path then runs the per-frame
// tail: func_001764E0, e+0xB4 -= 0.2, func_00175900(e, 1), func_001756E0,
// func_001796C0.

extern short D_0028A9A0;
extern int func_00160220(unsigned char *e);
extern int func_001607D0(unsigned char *e);
extern int func_001749A0(unsigned char *e, int id, int z, float f);
extern void func_00174A50(unsigned char *e, float blend);
extern int func_00174AC0(unsigned char *e, int n);
extern int func_001756E0(unsigned char *e);
extern unsigned char func_00175900(unsigned char *e, int f);
extern void func_001764E0(unsigned char *e);
extern void func_001796C0(unsigned char *e);
extern void func_0017B5C0(unsigned char *e);

void func_00161020(unsigned char *e)
{
    unsigned char st;

    st = e[6];
    switch (st) {
    case 0:
        e[6] = st + 1;
        e[7] = 0;
        *(int *)(e + 0x38) = 0;
        e[0x25C] = 0;
        func_00174A50(e, 12.0f);
        *(short *)(e + 0x28) = 0x12C;
        break;
    case 1:
        if (D_0028A9A0 != 0) {
            return;
        }
        if (func_001607D0(e) != 0) {
            return;
        }
        if (func_00160220(e) != 0) {
            return;
        }
        if (func_00174AC0(e, 0) != 0) {
            e[6] = e[6] + 1;
            func_0017B5C0(e);
            break;
        }
        switch (e[7]) {
        case 0:
            if (e[0x236] == 0 && !(e[0x235] & 1)) {
                if ((*(short *)(e + 0x28))-- == 0) {
                    e[7] = e[7] + 1;
                    func_001749A0(e, 0x15D, 1, 8.0f);
                }
            }
            break;
        case 1:
            if (*(int *)(e + 0x200) & 0x1000) {
                e[7] = 0;
                *(short *)(e + 0x28) = 0x12C;
                func_00174A50(e, 8.0f);
            }
            break;
        }
        break;
    case 2:
        if (func_001607D0(e) != 0) {
            return;
        }
        if (func_00160220(e) != 0) {
            return;
        }
        func_00174AC0(e, 1);
        if (*(int *)(e + 0x200) & 0x8000) {
            break;
        }
        {
        float zero = 0.0f;
        if (*(float *)(e + 0x240) == zero) {
            e[6] = 0x63;
        } else if (e[0x25D] == 0) {
            e[5] = 1;
            e[6] = 0;
            e[0x1F0] = 1;
            e[0x1F1] = 1;
        } else {
            *(int *)(e + 0x204) = 0;
        }
        }
        break;
    case 0x63:
        e[6] = st + 1;
        func_00174A50(e, 8.0f);
        break;
    case 0x64:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            e[6] = 1;
            e[7] = 0;
            *(short *)(e + 0x28) = 0x12C;
        }
        break;
    }

    func_001764E0(e);
    *(float *)(e + 0xB4) += -0.2f;
    func_00175900(e, 1);
    func_001756E0(e);
    func_001796C0(e);
}
