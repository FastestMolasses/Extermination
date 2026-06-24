// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Actor sub-state dispatcher. If actor byte +0x14 == 1, poll func_00227C60():
// codes 0/1/2 return 0/1/2 directly; code 3 sets bytes +1=+2=2, calls
// func_00225CF0(actor,3,7), decrements global D_00810CB2 by 4, runs
// func_00121A28(actor+0x6C,0,8), clears byte +0xA, returns 0. Otherwise call
// sub__02d_02d_02d() then func_00229170(actor): codes 0/1 -> 0, 2 -> 1, 3 -> 2,
// code 4 sets byte +0x15=1 and +1=1, calls func_00225A20/func_001FE8D0/
// func_00225CF0(actor,2,7), returns 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (81.5%):
// 991202 residual is the idiom-13 clean-store delay-slot fill; 2.3.3 is
// byte-identical. Verified objdiff 100% vs build/expected/func_00226E90.o.
// Note: the code-3 / code-4 arms use `break` (fall to the single trailing
// `return 0`) so CW's shared v0=0 epilogue tail-merges with the last store in
// the branch delay slot.
extern int func_00121A28(char *, int, int);
extern int func_001FE8D0(void);
extern int func_00225A20(void);
extern int func_00225CF0(char *, int, int);
extern int func_00227C60(void);
extern int func_00229170(char *);
extern int sub__02d_02d_02d(void);
extern short D_00810CB2;

int func_00226E90(char *arg0) {
    int v0;

    if (*(unsigned char *)(arg0 + 0x14) == 1) {
        v0 = func_00227C60();
        switch (v0) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            *(char *)(arg0 + 1) = 2;
            *(char *)(arg0 + 2) = 2;
            func_00225CF0(arg0, 3, 7);
            D_00810CB2 -= 4;
            func_00121A28(arg0 + 0x6C, 0, 8);
            *(char *)(arg0 + 0xA) = 0;
            break;
        }
    } else {
        sub__02d_02d_02d();
        v0 = func_00229170(arg0);
        switch (v0) {
        case 0:
            return 0;
        case 1:
            return 0;
        case 2:
            return 1;
        case 3:
            return 2;
        case 4:
            *(char *)(arg0 + 0x15) = 1;
            *(char *)(arg0 + 1) = 1;
            func_00225A20();
            func_001FE8D0();
            func_00225CF0(arg0, 2, 7);
            break;
        }
    }
    return 0;
}
