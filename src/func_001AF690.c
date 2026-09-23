// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane, 82.6% -> 100%): plain call arguments under
// mwcc 2.3.3; the 991202 build scheduled the a0/a2 setup of the first call
// differently from the target.
//
// Semantics (target 0x001AF690): three func_00121A28(buffer, 0, size) calls
// (the target passes a1 = 0 each time) over D_008101E0 (0xD0 bytes),
// D_008101D0 (0x10) and D_00810130 (0xA0), then D_0081060C = 1.0f.

extern char D_008101E0[];
extern char D_008101D0[];
extern char D_00810130[];
extern float D_0081060C;
extern void func_00121A28(void *, int, int);

void func_001AF690(void) {
    func_00121A28(D_008101E0, 0, 0xD0);
    func_00121A28(D_008101D0, 0, 0x10);
    func_00121A28(D_00810130, 0, 0xA0);
    D_0081060C = 1.0f;
}
