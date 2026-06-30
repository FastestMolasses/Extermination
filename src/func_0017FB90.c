// NEARMISS func_0017FB90  (vram 0x0017FB90, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register-coloring permutation: single commutative add.s where target uses the pi/2 constant (in $f1) as first source operand `add.s f0,f1,f0` but mwcc colors it into $f3 -> `add.s f0,f0,f3`. Body 100% correct (99.82% overall). Not the clean-store nop; permuter class.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 99.82% (mwcc 2.3.3; 991202 = 94.07%). Body/structure 100% correct.
// The SOLE residual is one commutative add.s FP-register coloring: the target
// emits `add.s f0, f1, f0` (the pi/2 constant, mtc1'd into f1, as the FIRST
// source operand), while mwcc colors the constant into f3 and emits
// `add.s f0, f0, f3`. Tried temp const, assign-in-arg, and loaded-into-temp
// reorders -- all hold at 99.82 or regress. FP-coloring permutation wall.
//
// Two keys that DID land: (1) the 0x7000xxxx EE scratchpad addresses are raw
// constant pointers (`*(volatile T*)0x7000NNNN`), NOT relocatable symbols --
// except the func_001026A0 3rd arg, which is the real data symbol D_700038A0;
// (2) the rotation constant is 1.5707964f -> bits 0x3FC90FDB (1.5707963f
// rounds to 0x3FC90FDA and misses).
//
// Reads view ptr at 0x700031D0, computes atan2(-(*(p+0x3C)), *(p+0x34)) via
// func_0011E620, caches it at 0x70003A20, then stores func_001B1470(pi/2 + that)
// to arg0+0xC4. Builds a TRS matrix (build_trs_matrix on arg0+0xD0/B0/C0/60),
// seeds the 4-float block at 0x700038A0 {0,0,5.0f,1.0f}, runs func_001026A0,
// nudges arg0+0xB4 by -0.2f, and sets the state bytes at arg0+0x25C/5/6/0x1F0.
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
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -0.2f;
    *(char *)(arg0 + 0x25C) = 1;
    *(char *)(arg0 + 5) = 5;
    *(char *)(arg0 + 6) = 0;
    *(char *)(arg0 + 0x1F0) = 0xB;
}
