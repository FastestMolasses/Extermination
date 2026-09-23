// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x70003B86
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// objdiff is 100.0% once build.py _SPAD_SYMS lists these addresses (so the
// expected object carries the same relocations); the linked bytes are identical.
//
// The lbu-vs-lb narrowing on arg1+0x56 is fixed by typing func_00187EC0's 2nd
// param `unsigned char`.

//
// Proximity/collision check between entity arg0 and entity arg1. Using
// func_0011DF78 (fabs), tests |arg0.x(0xA0)-arg1.x(0xB0)| <= bounds[0],
// |arg0.z(0xA8)-arg1.z(0xB8)| <= bounds[2], |arg0.y(0xA4)-arg1.y(0xB4)| <=
// 1.5 + bounds[1], where bounds = *(arg1+0x30). On all-in-range, branches on the
// state byte arg1+0xD: state 0 -> sets arg1+0xA=1, fires func_00187EC0(6, arg1+0x56),
// and if (arg1+0xB && D_00810707!=1 && arg0[0]==1) sets arg0+0x22C=5.0f bits and
// arg0[0]=3; state 1 -> func_00187EC0(7, 0). Always clears the short at 0x70003B86.
extern short D_70003B86;
extern float func_0011DF78(float a);
extern void func_00187EC0(char a0, unsigned char a1);
extern unsigned char D_00810707;

void func_001A8840(char *arg0, char *arg1) {
    unsigned char t;

    if (func_0011DF78(*(float *)(arg0 + 0xA0) - *(float *)(arg1 + 0xB0)) <= **(float **)(arg1 + 0x30) &&
        func_0011DF78(*(float *)(arg0 + 0xA8) - *(float *)(arg1 + 0xB8)) <= *(float *)(*(char **)(arg1 + 0x30) + 8) &&
        func_0011DF78(*(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4)) <= 1.5f + *(float *)(*(char **)(arg1 + 0x30) + 4)) {
        t = *(unsigned char *)(arg1 + 0xD);
        if (t == 0) {
            *(char *)(arg1 + 0xA) = 1;
            func_00187EC0(6, *(unsigned char *)(arg1 + 0x56));
            if (*(unsigned char *)(arg1 + 0xB) != 0 && D_00810707 != 1 && *(unsigned char *)arg0 == 1) {
                *(int *)(arg0 + 0x22C) = 0x40A00000;
                *(char *)arg0 = 3;
            }
        } else if (t == 1) {
            func_00187EC0(7, 0);
        }
        D_70003B86 = 0;
    }
}
