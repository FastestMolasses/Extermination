// NEARMISS func_0017F9E0  (vram 0x0017F9E0, 0x1A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single speculative delay-slot fill (body byte-identical): mwcc hoists the post-if merge block's `lui at,0x7000` (address setup for the D_700038A0/B0 averages) into the `b` (skip-else) delay slot, where the original CW left a `nop`. Sole residual. Known CW-vs-mwcc branch-delay-slot scheduling wall...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Aim/orient setup for actor arg0 against the camera/target record at *0x700031D0.
// Computes a base heading: func_0011E620(-record[0x3C], record[0x34]) (atan2-like)
// stored at the scratch float 0x70003A20, then func_001B1470(PI/2 + that) (angle
// normalize), latched back to 0x70003A20 and into arg0+0x218. The turn delta
// 0x70003A20 - arg0+0xC4 (normalized) is stored at 0x70003A24; its sign chooses a
// branch: if >= 0, arg0+0xC4 = normalize(arg0+0x218 - PI/2), arg0+0x2F1 = 0, and
// two func_0019F680(&D_700038A0/B0, *0x700031D0, 2/3) calls; if < 0, arg0+0xC4 =
// normalize(PI/2 + arg0+0x218), arg0+0x2F1 = 1, with func_0019F680 args 0/1.
// Both arms then set arg0+0xB0 = (D_700038A0 + D_700038B0)/2, arg0+0xB8 =
// (D_700038A8 + D_700038B8)/2 (the func_0019F680 calls fill those vec slots),
// set arg0[5]=0x1B, arg0[6]=0, arg0+0x1F0=0x2F, arg0+0x1F1=0, and call
// func_00174A50(self, 0.0f).
//
// NEARMISS: mwcc 2.3.3 reaches 99.381 (991202 95.571). Body byte-identical; the
// sole residual is a single speculative delay-slot fill: mwcc hoists the merge
// block's `lui at,0x7000` into the `b` (skip-else) delay slot, where the original
// CW left a `nop`. Known CW-vs-mwcc branch-delay-slot scheduling wall that 2.3.3
// does not fix. Logic fully recovered.
extern float func_0011E620(float a, float b);
extern float func_001B1470(float a);
extern void func_0019F680(void *p, int a, int b);
extern void func_00174A50(char *p, float f);
extern char D_700038A0;
extern char D_700038B0;

void func_0017F9E0(char *arg0) {
    char *p;
    float t;

    p = *(char **)0x700031D0;
    *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x3C), *(float *)(p + 0x34));
    t = func_001B1470(1.5707964f + *(float *)0x70003A20);
    *(float *)0x70003A20 = t;
    *(float *)(arg0 + 0x218) = t;
    t = func_001B1470(*(float *)0x70003A20 - *(float *)(arg0 + 0xC4));
    *(float *)0x70003A24 = t;
    if (!(t < 0.0f)) {
        *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0x218) - 1.5707964f);
        *(char *)(arg0 + 0x2F1) = 0;
        func_0019F680(&D_700038A0, *(int *)0x700031D0, 2);
        func_0019F680(&D_700038B0, *(int *)0x700031D0, 3);
    } else {
        *(float *)(arg0 + 0xC4) = func_001B1470(1.5707964f + *(float *)(arg0 + 0x218));
        *(char *)(arg0 + 0x2F1) = 1;
        func_0019F680(&D_700038A0, *(int *)0x700031D0, 0);
        func_0019F680(&D_700038B0, *(int *)0x700031D0, 1);
    }
    *(float *)(arg0 + 0xB0) = (*(float *)0x700038A0 + *(float *)0x700038B0) / 2.0f;
    *(float *)(arg0 + 0xB8) = (*(float *)0x700038A8 + *(float *)0x700038B8) / 2.0f;
    *(char *)(arg0 + 5) = 0x1B;
    *(char *)(arg0 + 6) = 0;
    *(char *)(arg0 + 0x1F0) = 0x2F;
    *(char *)(arg0 + 0x1F1) = 0;
    func_00174A50(arg0, 0.0f);
}
