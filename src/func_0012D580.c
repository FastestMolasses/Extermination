// NEARMISS func_0012D580  (vram 0x0012D580, 0x2C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// branch-delay-slot scheduling permutation: target (CW 2.3.1) leaves explicit NOPs in the switch-dispatch beq a3,v1 delay slots (cases 1/2/0) and two inner branches; mwcc 2.3.3 fills those slots by hoisting the next li/lui, shrinking the function and shifting all branch offsets. Not the clean-store...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
            *(int *)0x70003610 = 0;
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
            func_00128830((int)arg0, 0.0f, 0.0f, 1.0f);
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
