// NEARMISS func_0017E510  (vram 0x0017E510, 0x1CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Arg-setup-order/scheduling: in func_00102C58(&D_700036A0,&D_700036A0,arg0+0xC0) the target emits a1 (the address) before a2 (s1+0xC0); mwcc 233 emits the cheap reg-add (a2) first. Precomputing arg0+0xC0 into a temp and hoisting the &D_700036A0 address both leave the emit order unchanged. Body+con...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.55% (mwcc233). Body+structure fully recovered. Sole residual:
// in func_00102C58(&D_700036A0,&D_700036A0,arg0+0xC0) the target sets a1
// (the address) before a2 (s1+0xC0); mwcc 233 sets a2 (the cheap reg-add)
// first. Stable arg-setup-order / scheduling artifact - precomputing
// arg0+0xC0 into a temp and hoisting the &D_700036A0 address both leave the
// a1/a2 emit order unchanged. The four stores to 0x700038A0..AC are absolute
// (literal-address pointers, no relocation) while the address passed to the
// func_001026A0/func_00102C58 calls is the relocated D_700038A0 symbol -
// matching the target's mixed absolute-store + symbol-arg pattern.
// Camera/aim setup: init two structs at D_700036A0/D_700036D0 from arg0's
// +0xC0/+0xB0 sub-objects, then three passes - each writes a 4-float param
// block at 0x700038A0 (x=0/-4.5/+4.5, fixed y/z/w), builds a matrix via
// func_001026A0, and OR-accumulates func_001760C0(arg0,...,9.99f). Returns
// !!(any pass nonzero).
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, int);
extern void func_001031E0(void *, int);
extern void func_001026A0(void *, void *, void *);
extern int func_001760C0(int, void *, int, float);
extern char D_700036A0;
extern char D_700036D0;
extern char D_700038A0;
extern char D_700038B0;

int func_0017E510(int arg0) {
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
