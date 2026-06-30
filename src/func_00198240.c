// NEARMISS func_00198240  (vram 0x00198240, 0x200 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.40% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Branch delay-slot fill artifact. Body byte-identical except the `r == 1` dispatch branch: target emits `beq v0,v1,L344` with an EMPTY (nop) delay slot, leaving the fall-through `lui at,0x7000` (start of the *(int*)0x700031D0 deref for the &0x2000 check) as the next instruction; mwcc 2.3.3 fills t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
        p = *(char **)0x700031D4;
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
