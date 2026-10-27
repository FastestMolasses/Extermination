// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.76% NEARMISS).
// Fixes read from the original instructions:
//  - the three state handlers take (self, other) like their definitions in src/
//    (declared (void) before); the arguments are already in place, so no moves
//    are emitted, but the declaration changes register allocation.
//  - the dirty flag other[0x86] is cleared BEFORE func_0021C040 is called (the
//    original schedules that store into the call's delay slot).
//  - the switch default is a plain break (no goto label).
//
// Per-state object tick. Dispatches on the state byte at self[5] (3-way: 0/1/2)
// to func_0014A730 / func_0014A9A0 / func_0014ACC0 (all (self,other)). If the "dirty" flag other[0x86]
// is set, runs a reset: clears other[0x86], calls func_0021C040(D_008102B0, self), and
// if the global D_008106BD == 1 clears it too. Then runs the fixed per-frame chain
// func_0014BE20 / func_0014AE80 / func_0014AFA0 (all (self,other)), records
// anim_advance_time(self, *(float*)(other+0x40)) into *(int*)(other+0x20), calls
// func_00131ED0 / func_001C68C0 / func_001B17A0 (self), and finally the per-object
// virtual via the function pointer at *(self+0x4C) called with self.

extern void func_0014A730(char *a, char *b);
extern void func_0014A9A0(char *a, char *b);
extern void func_0014ACC0(char *a, char *b);
extern void func_0021C040(char *a, char *b);
extern void func_0014BE20(char *a, char *b);
extern void func_0014AE80(char *a, char *b);
extern void func_0014AFA0(char *a, char *b);
extern int anim_advance_time(char *a, float t);
extern void func_00131ED0(char *a);
extern void func_001C68C0(char *a);
extern void func_001B17A0(char *a);

extern char D_008102B0[];
extern unsigned char D_008106BD;

void func_0014A620(char *self, char *other)
{
    void (*fn)(char *);

    switch ((unsigned char)self[5]) {
    case 0:
        func_0014A730(self, other);
        break;
    case 1:
        func_0014A9A0(self, other);
        break;
    case 2:
        func_0014ACC0(self, other);
        break;
    default:
        break;
    }

    if (other[0x86]) {
        other[0x86] = 0;
        func_0021C040(D_008102B0, self);
        if (D_008106BD == 1) {
            D_008106BD = 0;
        }
    }
    func_0014BE20(self, other);
    func_0014AE80(self, other);
    func_0014AFA0(self, other);
    *(int *)(other + 0x20) = anim_advance_time(self, *(float *)(other + 0x40));
    func_00131ED0(self);
    func_001C68C0(self);
    func_001B17A0(self);
    fn = *(void (**)(char *))(self + 0x4C);
    fn(self);
}
