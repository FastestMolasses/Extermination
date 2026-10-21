// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), decomp2 lane round 3 (was an 83.62% NEARMISS).
// Initializes the subsystem struct *D_00275670 for slot arg0: stores the slot
// at +0x9C, sets four char* table pointers into the D_0028F700 table at per-slot
// strides (+0x10: arg0*0x60800 + 0x8000, +0x14: arg0*0x95760 + 0xC9000,
// +0x18: arg0*0x70000 + 0x1F3EC0, +0x1C: arg0*0x100000 + 0x2D3EC0), zeroes the
// four ints at +0x50..+0x5C, then runs func_001CBA40(), func_001D1F20(1),
// func_001D2040(1, 0), func_001D1FF0(1, 1),
// func_001CB8A0(&D_007635C0, 0, self, self + 4) and func_001D2DE0(0, 0).
// func_001CBA40 is an empty function that takes nothing: the old C passed it
// (self, the +0x18 pointer), which made mwcc set $a0/$a1 for the call; the
// "list-scheduler interleave" the old header recorded was that.
// -sdatathreshold 4 keeps D_00275670 gp-relative; the far tables are declared
// as arrays for absolute addressing.

extern void func_001CB8A0(void *a, int b, char *c, char *d);
extern void func_001CBA40(void);
extern void func_001D1F20(int a);
extern void func_001D1FF0(int a, int b);
extern void func_001D2040(int a, int b);
extern void func_001D2DE0(int a, int b);
extern char *D_00275670;
extern char D_0028F700[0x100];
extern char D_007635C0[0x100];

void func_001D1AE0(int arg0) {
    *(int *)(D_00275670 + 0x9C) = arg0;
    *(char **)(D_00275670 + 0x10) = D_0028F700 + arg0 * 0x60800 + 8 + 0x7FF8;
    *(char **)(D_00275670 + 0x14) = D_0028F700 + arg0 * 0x95760 + 0xC9000;
    *(char **)(D_00275670 + 0x18) = D_0028F700 + arg0 * 0x70000 + 0x1F3EC0;
    *(char **)(D_00275670 + 0x1C) = D_0028F700 + arg0 * 0x100000 + 0x2D3EC0;
    *(int *)(D_00275670 + 0x50) = 0;
    *(int *)(D_00275670 + 0x5C) = 0;
    *(int *)(D_00275670 + 0x58) = 0;
    *(int *)(D_00275670 + 0x54) = 0;
    func_001CBA40();
    func_001D1F20(1);
    func_001D2040(1, 0);
    func_001D1FF0(1, 1);
    func_001CB8A0(D_007635C0, 0, D_00275670, D_00275670 + 4);
    func_001D2DE0(0, 0);
}
