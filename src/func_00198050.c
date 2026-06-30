// NEARMISS func_00198050  (vram 0x00198050, 0x1EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.37% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Scheduling/FP-coloring near-miss. Body + control flow fully recovered. Residual: mwcc loads the addend of each `dst += src` float update before the destination (target loads addend-first then dest then `add dst,addend`) and fills the func_001026A0 branch delay slot with the 0x7000360C store; sour...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 88.4% (mwcc 2.3.3; pinned 991202 build 85.0%). Body and control flow
// fully recovered; residual is pure mwcc instruction-scheduling / FP-register
// coloring, not a logic gap.
//
// Per-frame transform/animation step for an entity (arg0) driven by a source
// object (arg1). Saves arg1+0x20 (s0) for the late tail call. Resets the
// transform scratch at arg0+0x30 (func_00102948 with the D_70003B50 identity),
// then func_001029C0/func_00102C58 build a matrix from it. Computes a target
// pose into arg0+0x10..0x18:
//   x(0x10) = arg1[0xA0] + -2.0 * sin?/trig(arg1[0xC4])   via func_0011E2A8
//   y(0x14) = 6.0 + arg1[0xB4]
//   z(0x18) = arg1[0xA8] + -2.0 * trig(arg1[0xC4])        via func_0011DE90
// 'flag' accumulates clamp/approach results: func_0018C850/func_0018C920 step
// arg0+0x10 and arg0+0x20 toward the target by rate 0.2 (0x3E4CCCCD), OR-ing each
// return into flag; flag is shifted left 4 between the two coordinate groups.
// Global vec at 0x70003600 is set to (0,0,20.0f,0) before func_001026A0 advances
// arg0+0x20 by arg0+0x10..0x18. A frame counter at arg0+8 is bumped; once it
// reaches 0xF the tail handler func_00198240(arg1, saved arg1[0x20]) fires.
//
// Wall (not the clean-store nop): mwcc's scheduler loads the addend of each
// `dst += src` float update before the destination and fills the func_001026A0
// delay slot with the 0x7000360C store; the source cannot force that exact
// slot-fill / FP pair-coloring. Permuter-class scheduling near-miss.
extern void func_001026A0(char *, void *, void *);
extern void func_00102948(char *, void *);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, char *);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern int func_0018C850(void *, float, float);
extern int func_0018C920(char *, void *, float);
extern void func_00198240(char *, int);
extern int D_008105D0;
extern int D_008105E0;
extern int D_70003400;
extern int D_70003600;
extern int D_70003B50;

int func_00198050(char *arg0, char *arg1) {
    int s0;
    int flag;

    s0 = *(int *)(arg1 + 0x20);
    flag = 0;
    func_00102948(arg0 + 0x30, &D_70003B50);
    func_001029C0(&D_70003400);
    func_00102C58(&D_70003400, &D_70003400, arg0 + 0x30);
    *(float *)(arg0 + 0x10) = *(float *)(arg1 + 0xA0) + -2.0f * func_0011E2A8(*(float *)(arg1 + 0xC4));
    *(float *)(arg0 + 0x14) = 6.0f + *(float *)(arg1 + 0xB4);
    *(float *)(arg0 + 0x18) = *(float *)(arg1 + 0xA8) + -2.0f * func_0011DE90(*(float *)(arg1 + 0xC4));
    flag |= func_0018C850(&D_008105D0, *(float *)(arg0 + 0x14), 0.2f);
    flag |= func_0018C920(arg0 + 0x10, &D_008105D0, 0.2f);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(int *)0x70003608 = 0x41A00000;
    *(int *)0x7000360C = 0;
    func_001026A0(arg0 + 0x20, &D_70003400, &D_70003600);
    *(float *)(arg0 + 0x20) = *(float *)(arg0 + 0x10) + *(float *)(arg0 + 0x20);
    *(float *)(arg0 + 0x24) = *(float *)(arg0 + 0x14) + *(float *)(arg0 + 0x24);
    *(float *)(arg0 + 0x28) = *(float *)(arg0 + 0x18) + *(float *)(arg0 + 0x28);
    flag = flag << 4;
    flag |= func_0018C850(&D_008105E0, *(float *)(arg0 + 0x24), 0.2f);
    flag |= func_0018C920(arg0 + 0x20, &D_008105E0, 0.2f);
    *(short *)(arg0 + 8) = *(short *)(arg0 + 8) + 1;
    if (*(short *)(arg0 + 8) >= 0xF) {
        func_00198240(arg1, s0);
    }
    return flag;
}
