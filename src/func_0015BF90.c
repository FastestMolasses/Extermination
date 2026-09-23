// NEARMISS func_0015BF90  (vram 0x0015BF90, 0x1C8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.99% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// 20 rows: a/b FP coloring of the min (target f0/f1, mwcc f2/f0) and scheduling of the 0x700038A4 reload in the state-0x41 block. m2-matching fixed the min lowering (ternary) and symbolized 0x70003A20/0x70003B8D.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Per-frame update on arg0 (entity) gated by state byte +0x1F0 != 0x19.
// func_00102948 is a quadword copy (dst, src). Copies the +0xB0 position to
// scratchpad D_700038A0, takes the smaller of the two child +0xC4 floats
// (children at +0x154/+0x158) and stores it at 0x70003A20 and 0x700038A4.
// If gate 0x70003B8D is set and state == 0x41: sets D_700038B0 to
// (0, 1.0, 0, 1.0), lowers +0x38A4 by 1.0 and calls func_001F9100(arg0+0xB0,
// D_700038A0, D_700038B0, 4.2f). Otherwise copies D_700038A0 to D_700038B0,
// lowers its y (+0x38B4) by 100.0, and if the segment probe func_0019A570
// (flags 6, 0) hits, copies D_700031B0 to D_700038A0, takes the vector at
// +0x24..+0x2C of the object at *0x700031D0 (w = 1.0) and calls
// func_001F9100 with that object as a fifth argument.
//
// m2-matching lane (90.62% -> 92.99%): the min is the ternary
// `a = (a < b) ? a : b`, which lowers to the target's bc1fl + `b join` + dead
// mov.s. 0x70003A20 and 0x70003B8D are relocated externs (idiom-32; both in the
// global _SPAD_SYMS). Remaining: a/b FP coloring (target f0/f1, mwcc f2/f0) and
// scheduling of the 0x700038A4 reload in the state-0x41 block (20 rows).
extern void func_00102948(void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern void func_001F9100(char *, void *, void *, float, ...);
extern int D_700031B0;
extern int D_700038A0;
extern int D_700038B0;
extern float D_70003A20;
extern unsigned char D_70003B8D;

void func_0015BF90(char *arg0) {
    float a, b;
    char *p;

    if (*(unsigned char *)(arg0 + 0x1F0) != 0x19) {
        func_00102948(&D_700038A0, arg0 + 0xB0);
        a = *(float *)(*(char **)(arg0 + 0x154) + 0xC4);
        b = *(float *)(*(char **)(arg0 + 0x158) + 0xC4);
        a = (a < b) ? a : b;
        D_70003A20 = a;
        *(float *)0x700038A4 = a;
        if (D_70003B8D != 0 && *(unsigned char *)(arg0 + 0x1F0) == 0x41) {
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
