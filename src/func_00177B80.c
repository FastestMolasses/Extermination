// NEARMISS func_00177B80  (vram 0x00177B80, 0x168 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Matching wave 2026-10-28 (95.07% -> 95.31%): the three callees were declared without prototypes; with their src/ prototypes (func_0019AB20(char *, float *, char *, int), the two matrix helpers taking three pointers) and arg0 as char *, the func_0019AB20 argument setup now matches at both call sites. Remaining differences: register choice and placement of the int constants stored to 0x700038B0/0x700038B4 (the original builds -4.5 in v0 and 1.0 in v1 up front and the -20.5 word later in v0; this C uses a1 for -4.5) plus a one-slot shift of the second multiply. Float-constant stores, an int local for 1.0, temp declaration order and parameter order did not change it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Builds a transform into a 16-byte stack scratch (tmp), twice. First with
// scale.x = -4.5 (0xC0900000): writes the 0x700038A0 scratch block
// (A0 = [0x70003050] + 1.5*[0x70003060]; A8 = [0x70003058] + 1.5*[0x70003068];
// A4 = fparg0; AC = 1.0f; B0..BC = {-4.5,-20.5,0,0}), runs func_001026A0 then
// func_001028B8 to compose the matrices into tmp, then func_0019AB20(arg0, tmp,
// arg0+0x280, 6). If that returns nonzero, return 1. Otherwise it retries with
// scale.x = +4.5 (0x40900000) and returns whether the second attempt succeeded.
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028B8(void *a0, void *a1, void *a2);
extern int func_0019AB20(char *arg0, float *arg1, char *arg2, int arg3);

extern float D_70003070;
extern float D_700038A0;
extern float D_700038B0;

int func_00177B80(char *arg0, float fparg0)
{
    float tmp[4];
    float a, b;

    a = *(float *)0x70003050 + 1.5f * *(float *)0x70003060;
    b = *(float *)0x70003058 + 1.5f * *(float *)0x70003068;
    *(int *)0x700038B0 = 0xC0900000;
    *(int *)0x700038B4 = 0xC1A40000;
    *(int *)0x700038B8 = 0;
    *(int *)0x700038BC = 0;
    *(float *)0x700038A0 = a;
    *(float *)0x700038A8 = b;
    *(float *)0x700038A4 = fparg0;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(tmp, &D_70003070, &D_700038B0);
    func_001028B8(tmp, tmp, &D_700038A0);
    if (func_0019AB20(arg0, tmp, arg0 + 0x280, 6) != 0) {
        return 1;
    }
    *(int *)0x700038B0 = 0x40900000;
    *(int *)0x700038B4 = 0xC1A40000;
    *(int *)0x700038B8 = 0;
    *(int *)0x700038BC = 0;
    func_001026A0(tmp, &D_70003070, &D_700038B0);
    func_001028B8(tmp, tmp, &D_700038A0);
    return func_0019AB20(arg0, tmp, arg0 + 0x280, 6) ? 1 : 0;
}
