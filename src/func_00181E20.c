// NEARMISS func_00181E20  (vram 0x00181E20, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.12% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residual artifacts at 97.125% on mwcc233 (best). (1) FP even/odd register-coloring on the '*0x700038C4 += 25.0f' update: target loads 0x700038C4 into the odd half (fv0f) and the 25.0f constant into fv0, mwcc 2.3.3 colors them fv0/fv1. (2) result-register placement: the target sets the return ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, float *);
extern void func_001026A0(void *, void *, void *);
extern void func_001031E0(void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];

int func_00181E20(char *arg0, int arg1, float fparg0) {
    int ret;
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 1.0f + fparg0;
    *(int *)0x700038AC = 0x3F800000;
    if (arg1 == 0) {
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0x218));
        func_00102918(D_700036A0, D_700036A0, (float *)(arg0 + 0xB0));
        func_001026A0(D_700038B0, D_700036A0, D_700038A0);
    } else {
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
    }
    func_001031E0(D_700038C0, D_700038B0);
    *(float *)0x700038C4 = *(float *)0x700038C4 + 25.0f;
    ret = 0;
    if (func_0019A570(D_700038B0, D_700038C0, 4, 0) != 0) {
        if (*(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x1E) {
            ret = 1;
        } else {
            ret = 0;
        }
    }
    return ret;
}
