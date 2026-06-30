// NEARMISS func_0014A620  (vram 0x0014A620, 0x110 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.76% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation/scheduling artifact. Body 100% logically correct (3-way state dispatch on self[5] -> func_0014A730/A9A0/ACC0, then conditional reset block guarded by other[0x86], then the fixed call chain func_0014BE20/AE80/AFA0/anim_advance_time/func_00131ED0/001C68C0/001B17A0 and the indire...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (96.8% on mwcc 2.3.3): body logically complete & faithful; sole
// residual is a register-allocation/scheduling artifact (param-save reg
// coloring swap self<->other s0/s1, plus one delay-slot fill choice on the
// func_0021C040 call). Permuter territory.
//
// Per-state object tick. Dispatches on the state byte at self[5] (3-way: 0/1/2)
// to func_0014A730 / func_0014A9A0 / func_0014ACC0. If the "dirty" flag other[0x86]
// is set, runs a reset: func_0021C040(D_008102B0, self), clears other[0x86], and
// if the global D_008106BD == 1 clears it too. Then runs the fixed per-frame chain
// func_0014BE20 / func_0014AE80 / func_0014AFA0 (all (self,other)), records
// anim_advance_time(self, *(float*)(other+0x40)) into *(int*)(other+0x20), calls
// func_00131ED0 / func_001C68C0 / func_001B17A0 (self), and finally the per-object
// virtual via the function pointer at *(self+0x4C) called with self.

extern void func_0014A730(void);
extern void func_0014A9A0(void);
extern void func_0014ACC0(void);
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
        func_0014A730();
        break;
    case 1:
        func_0014A9A0();
        break;
    case 2:
        func_0014ACC0();
        break;
    default:
        goto check;
    }

check:
    if (other[0x86]) {
        func_0021C040(D_008102B0, self);
        other[0x86] = 0;
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
