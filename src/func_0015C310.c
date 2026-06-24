// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 this hits only 90.0 because the idiom-13 clean-store/delay-slot family
// in the D_00810CA4 if/else chain is filled differently (991202 fills the beqz
// delay slot with a safe li where CW leaves a nop). The 2.3.3 build leaves the
// nop, so this readable C is byte-identical. Verified objdiff 100.0 vs
// build/expected/func_0015C310.o.
extern int func_0018A880(int, int);
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA6;
extern unsigned char D_00810CA7;

void func_0015C310(char *arg0, int arg1) {
    if (arg1 == 0) {
        *(int *)(arg0 + 0x20) = func_0018A880(0, 0);
        func_0018A880(1, 0);
        func_0018A880(1, 0x10);
    }
    if (D_00810CA4 == 2) {
        func_0018A880(2, 0xC);
    } else if (D_00810CA4 == 1) {
        func_0018A880(2, 0xB);
        func_0018A880(2, D_00810CA6);
    } else if (D_00810CA4 == 0) {
        func_0018A880(2, 0xA);
        func_0018A880(2, D_00810CA6);
    } else {
        func_0018A880(2, D_00810CA5);
        func_0018A880(2, D_00810CA6);
        func_0018A880(2, D_00810CA7);
    }
    if (D_00810CA6 == 4) {
        func_0018A880(1, 0x15);
    }
}
