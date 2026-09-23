// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003610

//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x70003610
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate the scratchpad high half (0x7000) into delay slots that the target leaves as nop.
// The `// SPAD:` directive above opts THIS file's expected object into the
// same %hi/%lo relocations (build.py _symbolize_scratchpad); the address stays
// out of the global _SPAD_SYMS because other matched files name the symbol
// yet store through literals. objdiff 100.0% (m2-matching lane).
// The trailing func_00128830 call in state 3 stages its 1.0f argument through
// an int (idiom-31) to reproduce the original argument order.

extern void func_001287F0(int a0, int a1, int a2, float f0);
extern void func_00128830(int a0, float f0, float f1, float f2);
extern float func_001B12B0(float a, float b, float c);
extern void func_001B5360(int a0);
extern void func_001C3DB0(void *a0, void *a1, void *a2, void *a3);
extern void func_001031E0(void *a0, void *a1);
extern int D_70003610;
extern int D_70003620;

void func_0012D580(unsigned char *arg0, unsigned char *arg1, int arg2) {
    int st = arg0[7];
    switch (st) {
    case 0:
        if (arg2 != 0) return;
        *(int *)(arg1 + 0xD8) = 0;
        if (*(int *)(arg1 + 0xE4) == 0x300) {
            func_00128830((int)arg0, 0.0f, 0.0f, -3.5f);
            func_001287F0((int)arg0, (int)arg1, 7, 0.0f);
        } else {
            func_001287F0((int)arg0, (int)arg1, 8, 0.0f);
        }
        arg0[7] = arg0[7] + 1;
        break;
    case 1:
        if (*(short *)(arg1 + 0xF8) == 8 || (*(short *)(arg1 + 0xF4) & 0x1000)) {
            D_70003610 = 0;
            *(int *)0x70003614 = 0x3F800000;
            *(int *)0x70003618 = 0;
            *(int *)0x7000361C = 0x3F800000;
            func_001C3DB0(arg1 + 0x80, &D_70003610, arg1 + 0x70, &D_70003620);
            func_001031E0(arg1 + 0x70, &D_70003620);
            *(int *)(arg1 + 0x80) = 0;
            *(int *)(arg1 + 0x84) = 0x3F800000;
            *(int *)(arg1 + 0x88) = 0;
            *(int *)(arg1 + 0x8C) = 0x3F800000;
            *(int *)(arg1 + 0xE4) = 0x500;
            *(int *)(arg1 + 0xF0) = 0;
            if (*(short *)(arg1 + 0xF8) == 7) {
                func_00128830((int)arg0, 0.0f, 5.0f, 0.5f);
                func_001287F0((int)arg0, (int)arg1, 0xF, 0.0f);
                *(int *)(arg0 + 0xC0) = 0x3FC90FDB;
            }
            arg0[7] = arg0[7] + 1;
        }
        break;
    case 2:
        func_001B5360((int)arg0);
        *(float *)(arg0 + 0xC0) = func_001B12B0(1.5707964f, *(float *)(arg0 + 0xC0), 0.06981317f);
        if (*(int *)(arg1 + 0xE4) == 0x100) {
            func_001287F0((int)arg0, (int)arg1, 9, 0.0f);
            *(int *)(arg0 + 0xC0) = 0;
            arg0[7] = arg0[7] + 1;
            *(int *)(arg1 + 0xD8) = 0;
            *(short *)(arg1 + 0xF4) = 0;
        }
        break;
    case 3:
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            int oi = 1;
            float o = (float)oi;
            func_00128830((int)arg0, 0.0f, 0.0f, o);
            func_001287F0((int)arg0, (int)arg1, 0, 0.0f);
            func_001287F0((int)arg0, (int)arg1, 2, 6.0f);
            if (arg0[4] == 2) {
                arg0[5] = arg0[5] + 1;
                arg0[6] = 0;
            } else if (arg0[5] == 8) {
                arg0[5] = 1;
                arg0[6] = 0;
            } else {
                arg0[6] = arg0[6] + 1;
            }
            arg0[7] = 0;
        }
        break;
    }
}
