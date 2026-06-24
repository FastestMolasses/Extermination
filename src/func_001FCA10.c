// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// State-machine tick on global mode D_002821B4. Mode 1 = active: sub-state
// switch on D_002821B0 (0/1 idle; 3 -> func_001FD0E0(&D_002821B0,2); 2 ->
// countdown D_002821BC, else when func_001FDB80(0,2)==1 advance mode to 2;
// 4 -> if D_00282240==0x64 cache func_001FCF90 result in D_00282248 and call
// func_001FCF60, else func_001FCB90; 16 -> func_001FC9B0()). Mode 2 = teardown:
// func_001FDB80(1,2) then func_001FC9B0(). Returns 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (84.7%):
// the 991202 residual is idiom-13 clean-store delay-slot fill; 2.3.3 leaves the
// nop and is byte-identical. Verified objdiff 100% vs build/expected/func_001FCA10.o.
// Notes: case order 1,0,2 reproduces CW's switch dispatch order (test 2, then 0,
// then 1); func_001FC9B0 is a void-arg call (no a0/a1 reload).
extern int func_001FC9B0(void);
extern int func_001FCB90(int, int, int, int);
extern int func_001FCF60(int, int, int);
extern int func_001FCF90(int, int, int);
extern int func_001FD0E0(int *, int);
extern int func_001FDB80(int, int);
extern int D_002821B0;
extern int D_002821B4;
extern int D_002821B8;
extern int D_002821BC;
extern int D_00282240;
extern int D_00282244;
extern int D_00282248;

int func_001FCA10(void) {
    switch (D_002821B4) {
    case 1:
        switch (D_002821B0) {
        case 0:
        case 1:
            break;
        case 3:
            func_001FD0E0(&D_002821B0, 2);
            break;
        case 2:
            if (D_002821BC > 0) {
                D_002821BC -= 1;
            } else if (func_001FDB80(0, 2) == 1) {
                D_002821B4 = 2;
            }
            break;
        case 4:
            if (D_00282240 == 0x64) {
                D_00282248 = func_001FCF90(D_002821B8, D_00282244, D_00282240);
                func_001FCF60(D_002821B8, 0xA8, 0xBE);
            } else {
                func_001FCB90(0x8A, 0xA8, D_00282240, D_002821B8);
            }
            break;
        case 16:
            func_001FC9B0();
            break;
        }
        break;
    case 0:
        break;
    case 2:
        func_001FDB80(1, 2);
        func_001FC9B0();
        break;
    }
    return 0;
}
