// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// State dispatcher. Reads a mode word from the global control block
// (*(int *)(D_00275670 + 0x246C)). For modes 1,3,4,5,6 it forwards the mode
// value to func_001D8C30 and returns. For any other mode it publishes the
// address of D_00817BC0 into the gp-rel slot D_00275688 and runs the
// three-stage setup func_001D8130 / func_001D8340 / func_001D8690 over the
// four incoming arguments.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): byte-identical (objdiff 100.0 vs
// build/expected/func_001D88B0.o); the pinned 991202 build caps at 75.3%
// (saved-reg/scheduling divergence). Keys to the match: D_00275670 and
// D_00275688 are gp-rel small-data (sdatathreshold 8); func_001D8340 takes a
// 5th argument (arg0), which is why the target promotes arg0 into the 4th
// callee-saved register (s3) and grows the frame to 0x50.
extern char *D_00275670;
extern void *D_00275688;
extern char D_00817BC0[];
extern void func_001D8130(int, int);
extern void func_001D8340(int, int, int, int, int);
extern void func_001D8690(int, int, int, int);
extern void func_001D8C30(int);

void func_001D88B0(int arg0, int arg1, int arg2, int arg3) {
    int v = *(int *)(D_00275670 + 0x246C);
    if (v == 1) {
        func_001D8C30(1);
        return;
    }
    if (v == 3) {
        func_001D8C30(3);
        return;
    }
    if (v == 4) {
        func_001D8C30(4);
        return;
    }
    if (v == 5) {
        func_001D8C30(5);
        return;
    }
    if (v == 6) {
        func_001D8C30(6);
        return;
    }
    D_00275688 = &D_00817BC0;
    func_001D8130(0x20, arg0);
    func_001D8340(0, arg1, arg2, 0x20, arg0);
    func_001D8690(arg1, arg2, arg3, 0x20);
}
