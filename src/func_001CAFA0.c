// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Sets up a DMA/display-list record through the gp-rel global D_00275670:
// caches its 0x1C cursor (saved), runs a sequence of subsystem calls
// (func_001D8C20(3), func_001C7420(arg0,0x3F5,3), func_001D40D0(arg1),
// func_001D1F20(3), func_001D1FF0(3,1)), then writes a tag into the current
// cursor slot (byte 0x60 at +3, zero the word at +4 and the half at +0),
// advances the cursor by 0x10, kicks func_001CB760(&D_007635C0, 0xFFD000,
// saved, 0x60), and finishes with func_001D8C20(0).
//
// mwcc 2.3.3 matches byte-identical; 991202 reorders the leading argument
// setup (call-scheduling residual ~80%). D_00275670 is a gp-rel pointer
// (-sdatathreshold 4 -> %gp_rel, CSE'd into one base load); D_007635C0 is
// sized [64] to exceed the threshold so it stays hi/lo, matching the target.
extern void func_001C7420(int a, int b, int c);
extern void func_001CB760(void *a, int b, char *c, int d);
extern void func_001D1F20(int a);
extern void func_001D1FF0(int a, int b);
extern void func_001D40D0(int a);
extern void func_001D8C20(int a);
extern char *D_00275670;
extern char D_007635C0[64];

void func_001CAFA0(int arg0, int arg1) {
    char *base;
    char *saved;

    base = D_00275670;
    saved = *(char **)(base + 0x1C);
    func_001D8C20(3);
    func_001C7420(arg0, 0x3F5, 3);
    func_001D40D0(arg1);
    func_001D1F20(3);
    func_001D1FF0(3, 1);
    base = D_00275670;
    *(char *)(*(char **)(base + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(base + 0x1C) + 4) = 0;
    *(short *)(*(char **)(base + 0x1C) + 0) = 0;
    *(char **)(base + 0x1C) = *(char **)(base + 0x1C) + 0x10;
    func_001CB760(D_007635C0, 0xFFD000, saved, 0x60);
    func_001D8C20(0);
}
