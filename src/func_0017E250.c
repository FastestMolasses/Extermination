// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038B0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.42% NEARMISS).
// Fixes read from the original instructions:
// - Callee prototypes follow their definitions in src/ (pointer arguments:
//   func_00102C58's third and func_001031E0's second parameter, arg0 is a
//   char *). With the pointer add, the call's argument setup order matches;
//   the "arg-setup-order wall" was a wrong prototype.
// - The two param blocks are relocated scratchpad int arrays D_700038A0[] /
//   D_700038B0[] (idiom-32, opted in with // SPAD:), so mwcc no longer
//   speculates the first store's upper half into the bail-out branch's slot.
// - The +20.5f nudge of D_700036D0's y is a compound `+=`, which gives the
//   original float register order of the add.
//
// Sibling of func_0017E510 (same camera/aim-clearance probe family). Builds two scratch structs at
// D_700036A0 (from arg0+0xC0) and D_700036D0 (from arg1), nudges D_700036D0's y by +20.5f, and bails
// out (returns 1) if func_001760C0(arg0, &D_700036D0, 1, 14.0f) reports a hit. Otherwise runs THREE
// passes, each writing a pair of float param blocks D_700038A0/D_700038B0 (x = 0 / -3.0 / +3.0,
// fixed y/z/w), matrix-transforming both via func_001026A0, and OR-accumulating
// func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7) (a segment/collision probe, mask 7). Returns the
// OR of the three probe results.

extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_001031E0(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern int func_001760C0(void *, void *, int, float);
extern int func_0019AFE0(void *, void *, void *, int);
extern char D_700036A0;
extern char D_700036D0;
extern int D_700038A0[];
extern int D_700038B0[];
extern char D_700038C0;
extern char D_700038D0;

int func_0017E250(char *arg0, void *arg1) {
    int acc;

    acc = 0;
    func_001029C0(&D_700036A0);
    func_00102C58(&D_700036A0, &D_700036A0, arg0 + 0xC0);
    func_001031E0(&D_700036D0, arg1);
    *(float *)0x700036D4 += 20.5f;
    if (func_001760C0(arg0, &D_700036D0, 1, 14.0f) != 0) {
        return 1;
    }
    D_700038A0[0] = 0;
    D_700038B0[0] = 0;
    D_700038A0[1] = 0x408051EC;
    D_700038B0[1] = 0x408051EC;
    D_700038A0[2] = 0;
    D_700038A0[3] = 0x3F800000;
    D_700038B0[2] = 0x40A00000;
    D_700038B0[3] = 0x3F800000;
    func_001026A0(&D_700038C0, &D_700036A0, D_700038A0);
    func_001026A0(&D_700038D0, &D_700036A0, D_700038B0);
    acc = acc | func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7);
    D_700038A0[0] = 0xC0400000;
    D_700038B0[0] = 0xC0400000;
    D_700038A0[1] = 0x408051EC;
    D_700038B0[1] = 0x408051EC;
    D_700038A0[2] = 0;
    D_700038A0[3] = 0x3F800000;
    D_700038B0[2] = 0x40A00000;
    D_700038B0[3] = 0x3F800000;
    func_001026A0(&D_700038C0, &D_700036A0, D_700038A0);
    func_001026A0(&D_700038D0, &D_700036A0, D_700038B0);
    acc = acc | func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7);
    D_700038A0[0] = 0x40400000;
    D_700038B0[0] = 0x40400000;
    D_700038A0[1] = 0x408051EC;
    D_700038B0[1] = 0x408051EC;
    D_700038A0[2] = 0;
    D_700038A0[3] = 0x3F800000;
    D_700038B0[2] = 0x40A00000;
    D_700038B0[3] = 0x3F800000;
    func_001026A0(&D_700038C0, &D_700036A0, D_700038A0);
    func_001026A0(&D_700038D0, &D_700036A0, D_700038B0);
    acc = acc | func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 7);
    return acc;
}
