// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Three-attempt placement/collision probe for actor arg0. For each of 3 tries
// (angle table D_00248950[i]): builds a candidate transform in scratch object
// D_70003760 from the actor's facing +0xC4 offset by func_001B1470(+0xC4 +
// D_00248950[i]/2), the actor's position +0xB0, then bakes it into D_700038D0
// with a fixed local probe point {0, 6.5(0x408051EC), 10.0, 1.0}. Tests the
// candidate with func_0019AD00(...,7) (blocked?) and func_001760C0(...,1,13.99)
// (valid landing?). If blocked OR the landing test fails, returns 0
// immediately; if all three angles are exhausted without success, returns 1.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 only reaches ~88%
// (instruction scheduling / delay-slot fills). Two non-obvious shapes were
// needed: declaring the loop counter before the pointer fixes the s0/s1
// allocation (counter=s0, ptr=s1), and folding the two failure tests into one
// short-circuit `||` makes both early returns funnel through a single shared
// `v0=0; b end` epilogue block reached by a forward branch, matching CW's
// layout (and preserving the bnel loop-continue). Verified objdiff 100% vs
// build/expected/func_00176C80.o.
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, void *);
extern void func_001026A0(void *, void *, void *);
extern int func_0019AD00(void *, void *, int);
extern int func_001760C0(void *, void *, int, float);
extern float func_001B1470(float);
extern float D_00248950[];
extern int D_70003760;
extern int D_700038D0;

int func_00176C80(char *arg0) {
    int sp[4];
    int i;
    float *p;

    sp[0] = 0;
    sp[1] = 0x408051EC;
    sp[2] = 0x41200000;
    sp[3] = 0x3F800000;
    p = D_00248950;
    for (i = 0; i < 3; i++, p++) {
        func_001029C0(&D_70003760);
        func_00102BB0(&D_70003760, &D_70003760, func_001B1470(*(float *)(arg0 + 0xC4) + *p / 2.0f));
        func_00102918(&D_70003760, &D_70003760, arg0 + 0xB0);
        func_001026A0(&D_700038D0, &D_70003760, sp);
        if (func_0019AD00(arg0, &D_700038D0, 7) != 0 || func_001760C0(arg0, &D_700038D0, 1, 13.99f) == 0) {
            return 0;
        }
    }
    return 1;
}
