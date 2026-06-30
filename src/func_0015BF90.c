// NEARMISS func_0015BF90  (vram 0x0015BF90, 0x1C8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two CW-vs-mwcc compiler artifacts (not the clean-store nop): (1) min(a,b) lowers to bc1fl branch-likely + dead duplicated mov.s in the target, mwcc-2.3.3 emits plain bc1t (branch-likely-sense/dead-block branch-lowering wall); (2) instruction scheduling of the scratchpad stores in the ==0x41 block...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Per-frame update on arg0 (entity) gated by state byte +0x1F0 != 0x19. Copies arg0+0xB0 matrix to
// scratchpad D_700038A0, computes min of the two child +0xC4 floats (children at +0x154/+0x158),
// stashes it at 0x70003A20 and 0x700038A4. If gate 0x70003B8D set and state==0x41: build a fixed
// 0x700038B0 block, decrement +38A4 by 1.0, blend via func_001F9100. Else: blend D_700038B0<-D_700038A0,
// -=100.0 on +38B4, and if func_0019A570 succeeds copy a source struct (*0x700031D0)+0x24/28/2C into the
// block and blend again.
extern void func_00102948(void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern void func_001F9100(char *, void *, void *, float, ...);
extern int D_700031B0;
extern int D_700038A0;
extern int D_700038B0;

void func_0015BF90(char *arg0) {
    float a, b;
    char *p;

    if (*(unsigned char *)(arg0 + 0x1F0) != 0x19) {
        func_00102948(&D_700038A0, arg0 + 0xB0);
        a = *(float *)(*(char **)(arg0 + 0x154) + 0xC4);
        b = *(float *)(*(char **)(arg0 + 0x158) + 0xC4);
        if (a >= b) a = b;
        *(float *)0x70003A20 = a;
        *(float *)0x700038A4 = a;
        if (*(unsigned char *)0x70003B8D != 0 && *(unsigned char *)(arg0 + 0x1F0) == 0x41) {
            *(int *)0x700038B0 = 0;
            *(float *)0x700038B4 = 1.0f;
            *(int *)0x700038B8 = 0;
            *(int *)0x700038BC = 0x3F800000;
            *(float *)0x700038A4 -= 1.0f;
            func_001F9100(arg0 + 0xB0, &D_700038A0, &D_700038B0, 4.2f);
            return;
        }
        func_00102948(&D_700038B0, &D_700038A0);
        *(float *)0x700038B4 -= 100.0f;
        if (func_0019A570(&D_700038A0, &D_700038B0, 6, 0) != 0) {
            func_00102948(&D_700038A0, &D_700031B0);
            p = *(char **)0x700031D0;
            *(float *)0x700038B0 = *(float *)(p + 0x24);
            *(float *)0x700038B4 = *(float *)(p + 0x28);
            *(float *)0x700038B8 = *(float *)(p + 0x2C);
            *(int *)0x700038BC = 0x3F800000;
            func_001F9100(arg0 + 0xB0, &D_700038A0, &D_700038B0, 4.2f, p);
        }
    }
}
