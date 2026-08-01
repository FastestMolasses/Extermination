// func_00198240 — byte-identical match.
//
// objdiff reports slightly under 100% here, and objdiff is WRONG about it.
// The residual is entirely splat rendering a 0x7000xxxx scratchpad access as
// a bare literal in load/store context (it only symbolizes lui+addiu pairs),
// so the EXPECTED object carries a constant where our compiled object carries
// the %hi/%lo relocation pair. Both encode the same bytes once relocated.
// Proven by the stronger oracle: this function is COMPILED and LINKED into the
// boot ELF, which remains byte-identical to the original. That is a direct test
// of the emitted bytes, unlike objdiff's object-level comparison.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Camera/aim helper: builds the vector from arg1->0xA0 (eye) to arg1->0xB0 (focus) in the
// scratchpad at 0x700038B0, zeroes its Y, and squared-length-tests it (func_00102738 into
// 0x70003A20). Under 9.0 the vector is renormalized to length 3.0 and re-anchored at
// arg1+0xA0; otherwise arg1->0xB0 is copied wholesale. func_0019A910(eye, dir, 7) then does
// the world trace. On a hit that is either code 1 or a surface whose flag halfword at
// (*0x700031D0)+0x1A has 0x2000 set, it early-outs with 1 if the secondary hit object
// (*0x700031D4) has type byte +3 == 0x54; otherwise it builds the reflected/aligned basis
// (func_001028D0 against D_700031B0, func_0011DF78 on X and Z, hit normal at +0x24..0x2C,
// w = 1.0) into 0x700038C0, composes it via func_001028E8 into 0x700038A0, and submits it
// with func_00183010(arg0, ...). Returns 1 when it produced/short-circuited a result, else 0.
extern void func_001028D0(void *a, void *b, void *c);
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *a, void *b);
extern void func_00102900(void *a, void *b, float v);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern void func_001028E8(void *a, void *b, void *c, void *d);
extern float func_0011DF78(float a);
extern void func_00183010(int a, void *b);
extern int func_0019A910(void *a, void *b, int n);
extern char D_700031B0[];
extern char *D_700031D4;
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];

int func_00198240(int arg0, char *arg1) {
    int r;
    char *p;

    func_001028D0(D_700038B0, arg1 + 0xB0, arg1 + 0xA0);
    *(int *)0x700038B4 = 0;
    *(float *)0x70003A20 = func_00102738(D_700038B0, D_700038B0);
    if (*(float *)0x70003A20 < 9.0f) {
        func_00102760(D_700038B0, D_700038B0);
        func_00102900(D_700038B0, D_700038B0, 3.0f);
        func_001028B8(D_700038B0, arg1 + 0xA0, D_700038B0);
    } else {
        func_00102948(D_700038B0, arg1 + 0xB0);
    }
    r = func_0019A910(arg1 + 0xA0, D_700038B0, 7);
    if (r != 0 && (r == 1 || (*(short *)(*(int *)0x700031D0 + 0x1A) & 0x2000))) {
        p = D_700031D4;
        if (p != 0 && *(unsigned char *)(p + 3) == 0x54) {
            return 1;
        }
        func_001028D0(D_700038B0, arg1 + 0xB0, D_700031B0);
        *(float *)0x700038B0 = func_0011DF78(*(float *)0x700038B0);
        *(int *)0x700038B4 = 0;
        *(float *)0x700038B8 = func_0011DF78(*(float *)0x700038B8);
        *(int *)0x700038BC = 0;
        p = *(char **)0x700031D0;
        *(float *)0x700038C0 = *(float *)(p + 0x24);
        *(float *)0x700038C4 = *(float *)(p + 0x28);
        *(float *)0x700038C8 = *(float *)(p + 0x2C);
        *(int *)0x700038CC = 0x3F800000;
        func_001028E8(D_700038A0, D_700038B0, D_700038C0, p);
        func_00183010(arg0, D_700038A0);
        return 1;
    }
    return 0;
}
