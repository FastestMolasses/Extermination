// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane, NEARMISS 81.55% -> 100%): the recorded
// "store-scheduling artifact" was mwcc hoisting the two `sw zero` stores
// (0x320/0x324) and the pointer store to D_008102C4 past their neighbours.
// Declaring every stored global volatile pins all stores to source order,
// which is the target's order (same lever as func_001B0C60).
//
// Semantics (target 0x001AF5C0): func_00121A28(D_008102B0, 0, 0x320); then
// D_008102C4 = D_008102B0, D_008102B2 = 0, eight floats (0x310..0x31C,
// 0x330..0x33C) = 1.0f, words 0x320/0x324 = 0, floats 0x328/0x32C = 1.0f,
// shorts 0x344 = -1 and 0x346 = 0x3D; finally func_001D8BF0(D_008102B0, 1).

extern void func_00121A28(void *, int, int);
extern void func_001D8BF0(void *, int);

extern char D_008102B0[];
extern void *volatile D_008102C4;
extern volatile char D_008102B2;
extern volatile float D_00810310, D_00810314, D_00810318, D_0081031C;
extern volatile float D_00810330, D_00810334, D_00810338, D_0081033C;
extern volatile int D_00810320, D_00810324;
extern volatile float D_00810328, D_0081032C;
extern volatile short D_00810344, D_00810346;

void func_001AF5C0(void) {
    func_00121A28(D_008102B0, 0, 0x320);
    D_008102C4 = D_008102B0;
    D_008102B2 = 0;
    D_00810310 = 1.0f;
    D_00810314 = 1.0f;
    D_00810318 = 1.0f;
    D_0081031C = 1.0f;
    D_00810330 = 1.0f;
    D_00810334 = 1.0f;
    D_00810338 = 1.0f;
    D_0081033C = 1.0f;
    D_00810320 = 0;
    D_00810324 = 0;
    D_00810328 = 1.0f;
    D_0081032C = 1.0f;
    D_00810344 = -1;
    D_00810346 = 0x3D;
    func_001D8BF0(D_008102B0, 1);
}
