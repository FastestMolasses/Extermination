// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Camera/aim setup: init two structs at D_700036A0/D_700036D0 from arg0's
// +0xC0/+0xB0 sub-objects, then three passes - each writes a 4-float param
// block at 0x700038A0 (x = 0 / -4.5 / +4.5, fixed y = 24.51f, z = 3.62f,
// w = 1.0f), builds a transformed vector at D_700038B0 via func_001026A0,
// and OR-accumulates func_001760C0(arg0, vec, 1, 9.99f). Returns whether any
// pass reported a hit. The four stores to 0x700038A0..AC are absolute
// (literal-address pointers, no relocation) while the address passed to the
// func_001026A0 call is the relocated D_700038A0 symbol - matching the
// target's mixed absolute-store + symbol-arg pattern.

extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_001031E0(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern int func_001760C0(void *, void *, int, float);
extern char D_700036A0;
extern char D_700036D0;
extern char D_700038A0;
extern char D_700038B0;

int func_0017E510(char *arg0) {
    int acc;

    acc = 0;
    func_001029C0(&D_700036A0);
    func_00102C58(&D_700036A0, &D_700036A0, arg0 + 0xC0);
    func_001031E0(&D_700036D0, arg0 + 0xB0);
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x41C4147B;
    *(int *)0x700038A8 = 0x4067AE14;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
    acc = acc | func_001760C0(arg0, &D_700038B0, 1, 9.989999771118164f);
    *(int *)0x700038A0 = 0xC0900000;
    *(int *)0x700038A4 = 0x41C4147B;
    *(int *)0x700038A8 = 0x4067AE14;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
    acc = acc | func_001760C0(arg0, &D_700038B0, 1, 9.989999771118164f);
    *(int *)0x700038A0 = 0x40900000;
    *(int *)0x700038A4 = 0x41C4147B;
    *(int *)0x700038A8 = 0x4067AE14;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
    acc = acc | func_001760C0(arg0, &D_700038B0, 1, 9.989999771118164f);
    return acc != 0 ? 1 : 0;
}
