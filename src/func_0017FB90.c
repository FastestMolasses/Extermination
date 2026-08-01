// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Ladder/climb camera-and-pose setup for the actor at arg0. Reads the active view
// block at scratchpad 0x700031D0, computes atan2(-view->+0x3C, view->+0x34) via
// func_0011E620 and caches it at 0x70003A20, then stores func_001B1470(pi/2 + that)
// as the actor's facing angle at +0xC4. Builds the actor TRS matrix
// (build_trs_matrix on +0xD0 / +0xB0 / +0xC0 / +0x60), seeds the scratch vec4 at
// 0x700038A0 with {0, 0, 5.0f, 1.0f}, transforms it into the actor position via
// func_001026A0, drops the actor 0.2 units in Y, and sets the state bytes
// (+0x25C = 1, +5 = 5, +6 = 0, +0x1F0 = 0x0B).
//
// The 0x7000xxxx scratchpad addresses are raw constant pointers, NOT relocatable
// symbols -- except func_001026A0's 3rd arg, which is the real data symbol
// D_700038A0. The rotation constant is 1.5707964f (bits 0x3FC90FDB); 1.5707963f
// rounds to 0x3FC90FDA and misses. The Y nudge must be spelled as the compound
// assignment `+= -0.2f` (idiom-26) to get `add.s $f0,$f1,$f0` with the loaded
// value as fs; the expanded `x = x + -0.2f` swaps the add.s source registers.
extern float func_0011E620(float y, float x);
extern float func_001B1470(float a);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void build_trs_matrix(void *a0, void *a1, void *a2, void *a3);
extern int D_700038A0;

void func_0017FB90(char *arg0) {
    char *p;
    float c;

    p = *(char **)0x700031D0;
    *(volatile float *)0x70003A20 =
        func_0011E620(-(*(float *)(p + 0x3C)), *(float *)(p + 0x34));
    c = 1.5707964f;
    *(float *)(arg0 + 0xC4) = func_001B1470(c + *(volatile float *)0x70003A20);
    build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, arg0 + 0xC0, arg0 + 0x60);
    *(volatile int *)0x700038A0 = 0;
    *(volatile int *)0x700038A4 = 0;
    *(volatile int *)0x700038A8 = 0x40A00000;
    *(volatile int *)0x700038AC = 0x3F800000;
    func_001026A0(arg0 + 0xB0, arg0 + 0xD0, &D_700038A0);
    *(float *)(arg0 + 0xB4) += -0.2f;
    *(char *)(arg0 + 0x25C) = 1;
    *(char *)(arg0 + 5) = 5;
    *(char *)(arg0 + 6) = 0;
    *(char *)(arg0 + 0x1F0) = 0xB;
}
