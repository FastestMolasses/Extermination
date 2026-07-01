// NEARMISS func_0017E250  (vram 0x0017E250, 0x2B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.42% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Arg-setup-order/scheduling wall on func_00102C58(&D_700036A0,&D_700036A0,arg0+0xC0): target emits a1 (the address) before a2 (the cheap reg-add arg0+0xC0), mwcc233 emits a2 first, flipping the following float-add's register (fv0 vs fv1). Same wall already parked on sibling func_0017E510.c (99.55%...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Sibling of func_0017E510 (same camera/aim-clearance probe family). Builds two scratch structs at
// D_700036A0 (from arg0+0xC0) and D_700036D0 (from arg1), nudges D_700036D0's y by +20.5f, and bails
// out (returns 1) if func_001760C0(arg0, &D_700036D0, 1, 14.0f) reports a hit. Otherwise runs THREE
// passes, each writing a pair of float param blocks at 0x700038A0/0x700038B0 (x = 0 / -3.0 / +3.0,
// fixed y/z/w), matrix-transforming both via func_001026A0, and OR-accumulating
// func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7) (a segment/collision probe, mask 7). Returns the
// OR of the three probe results.

extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, int);
extern void func_001031E0(void *, int);
extern void func_001026A0(void *, void *, void *);
extern int func_001760C0(int, void *, int, float);
extern int func_0019AFE0(int, void *, void *, int);
extern char D_700036A0;
extern char D_700036D0;
extern char D_700038A0;
extern char D_700038B0;
extern char D_700038C0;
extern char D_700038D0;

int func_0017E250(int arg0, int arg1) {
    int acc;

    acc = 0;
    func_001029C0(&D_700036A0);
    func_00102C58(&D_700036A0, &D_700036A0, arg0 + 0xC0);
    func_001031E0(&D_700036D0, arg1);
    *(float *)0x700036D4 = *(float *)0x700036D4 + 20.5f;
    if (func_001760C0(arg0, &D_700036D0, 1, 14.0f) != 0) {
        return 1;
    }
    *(int *)0x700038A0 = 0;
    *(int *)0x700038B0 = 0;
    *(int *)0x700038A4 = 0x408051EC;
    *(int *)0x700038B4 = 0x408051EC;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40A00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, &D_700036A0, &D_700038A0);
    func_001026A0(&D_700038D0, &D_700036A0, &D_700038B0);
    acc = acc | func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7);
    *(int *)0x700038A0 = 0xC0400000;
    *(int *)0x700038B0 = 0xC0400000;
    *(int *)0x700038A4 = 0x408051EC;
    *(int *)0x700038B4 = 0x408051EC;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40A00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, &D_700036A0, &D_700038A0);
    func_001026A0(&D_700038D0, &D_700036A0, &D_700038B0);
    acc = acc | func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7);
    *(int *)0x700038A0 = 0x40400000;
    *(int *)0x700038B0 = 0x40400000;
    *(int *)0x700038A4 = 0x408051EC;
    *(int *)0x700038B4 = 0x408051EC;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40A00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, &D_700036A0, &D_700038A0);
    func_001026A0(&D_700038D0, &D_700036A0, &D_700038B0);
    acc = acc | func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7);
    return acc;
}
