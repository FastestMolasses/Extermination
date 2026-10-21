// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// TU_STATIC: func_001F6AC0
//
// Byte-matched (objdiff 100%), decomp lane round 7. Three corrections to the
// former NEARMISS body (78.73%):
//  - The selector dispatch is a sparse switch. The target compares 0x1100,
//    0xE00, 0x200, 0x100, 2, 1, 0x1301, 0 (reverse label order) with NOP
//    delay slots, lays out the case 0 body before the 0x1301 body, and sends
//    the do-nothing states to the shared exit (MATCHING_GUIDE idiom-33).
//  - func_001F6640 takes the slot pointer: the target calls it with a0 still
//    holding p0 (slot 0) or p1 (slot 1) from the preceding func_001F6AC0 call.
//    The old C called it with no argument. Likewise state 0 hands the
//    func_001F6760 descriptor to func_001F66F0 (a0 kept across func_001F6AC0).
//  - func_001F6AC0 is compiled as a static function of this file. The original
//    compiler saw it earlier in the same translation unit and knew it leaves a0
//    intact, so it kept the slot pointer in a0 across that call: that gives
//    the branch-likely on the slot 0 test and the D_0081079E load into a1.
//    tools/decomp/tu_static.py removes the static copy after checking it
//    equals the original func_001F6AC0; the call links to its own unit.
//
// Level/state dispatcher keyed on the 16-bit selector (D_00810700<<8 |
// D_00810701). The states {0x1100, 0xE00, 0x200, 0x100, 2, 1} and every
// unlisted value do nothing. State 0x1301, with the two object slots
// D_0025D270 (p0) / D_0025D2C0 (p1): if the sub-mode D_00810702 is 2, 4, 5 or
// 8 it runs func_001F6850 when either slot is "ready" (func_001F6AC0 != 0);
// otherwise it runs func_001F6640(p0) when slot 0 is not ready and neither
// D_00810778 nor D_0081077B is 0xFF, then func_001F6640(p1) when slot 1 is not
// ready and D_0081079E is 0xFF. State 0: when D_0081075D == 0xFF and the
// descriptor from func_001F6760 is ready, runs func_001F66F0 on it.

extern void func_001F6640(void *p);
extern void func_001F66F0(void *p);
extern int *func_001F6760(void);
extern void func_001F6850(void);
extern int D_0025D270;
extern int D_0025D2C0;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_0081075D;
extern unsigned char D_00810778;
extern unsigned char D_0081077B;
extern unsigned char D_0081079E;

static int func_001F6AC0(int *a0);  /* same-TU callee, see TU_STATIC */
#include "src/func_001F6AC0.c"

void func_001F6BB0(void) {
    int mode;
    int *p0;
    int *p1;

    mode = (D_00810700 << 8) + D_00810701;
    switch (mode) {
    case 0:
        if (D_0081075D == 0xFF) {
            p0 = func_001F6760();
            if (func_001F6AC0(p0) != 0) {
                func_001F66F0(p0);
            }
        }
        break;
    case 0x1301:
        p0 = &D_0025D270;
        p1 = &D_0025D2C0;
        if (D_00810702 == 2 || (unsigned int)(D_00810702 - 4) <= 1 || D_00810702 == 8) {
            if (func_001F6AC0(p0) != 0 || func_001F6AC0(p1) != 0) {
                func_001F6850();
            }
        } else {
            if (func_001F6AC0(p0) == 0 && D_00810778 != 0xFF && D_0081077B != 0xFF) {
                func_001F6640(p0);
            }
            if (func_001F6AC0(p1) == 0 && D_0081079E == 0xFF) {
                func_001F6640(p1);
            }
        }
        break;
    case 1:
    case 2:
    case 0x100:
    case 0x200:
    case 0xE00:
    case 0x1100:
        break;
    }
}
