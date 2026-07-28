// NEARMISS func_001EA240  (vram 0x001EA240, 0x908 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.98% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP constant-materialization SCHEDULING order at ONE call site (4 instructions): in the state-1 light-group pair, the target emits the second func_0021B9A0 call's args as f13(20.0f) before f12(1.0f); mwcc 2.3.3 emits f12-first. Body otherwise byte-identical. Tried 12+ shapes: (z=20.0f) assign-in-a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Rumble/shake effect driver (func_0021B9A0 channel driver), dispatched on the
// effect state byte e[4]; e[0xD] is the effect type. D_00275C30/D_00275C34 are set every call
// to the current effect object / its work block e+0x1F0. State 0 (init):
// seeds the work block (accumulator +0x54, per-type step from the
// D_00255430 table, duration limit +0xC, RNG seed from func_00122BB8) and
// preconditions the direction/matrix block e+0xB0..0x108 per type-group
// (func_001029C0/func_00102C58/func_00102BB0 matrix setup, jitter via
// func_00122BB8, func_001B1470 angle wrap), then falls through to state 1
// (run): kicks the big/small motors (func_0021B9A0 channels 2/3, 20.0f for
// the light group / 100.0f for the heavy group), invokes the per-type update
// handler from the D_00255434 function-pointer table, advances the
// accumulator by the step and ends the effect (e[4]=3) when the limit is
// exceeded (limit 0.0f = endless with 2.0f decay clamp), then re-arms
// channel 1 with (0,0). States 2/3: func_001AFC10 teardown.
//
// NEARMISS residual (mwcc 2.3.3, 99.98%): at the state-1 light-group pair,
// the target materializes the second call's float args f13(20.0f) BEFORE
// f12(1.0f); mwcc emits f12-first. Pure constant-materialization scheduling
// artifact — 12+ source shapes tried (assign-in-arg temp, decl-init vars,
// operand swaps, comma exprs); the order flips in reduced reproductions but
// never in the full function (IR-numbering sensitive). Permuter territory.
//
extern void func_001AFC10(unsigned char *e);
extern int func_00122BB8(void);
extern void func_00102760(void *a, void *b);
extern void func_001CD390(void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern void func_001029C0(void *a);
extern void func_00102C58(void *a, void *b, void *c);
extern void func_00102BB0(void *a, void *b, float f);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001F0460(int a, void *b);
extern float func_001B1470(float a);
extern int func_001CCF70(void *a);
extern void func_0021B9A0(int ch, float a, float b);
extern float D_00255430[];
extern int D_00255434[];
extern int D_00275C30;
extern int D_00275C34;
extern float D_008102E8[];
extern float D_700038A0[];

void func_001EA240(unsigned char *e)
{
    unsigned char st;
    int n;
    char *w;
    float lim;

    D_00275C34 = (int)(e + 0x1F0);
    D_00275C30 = (int)e;
    st = e[4];
    switch (st) {
    case 0:
        switch (e[0xD]) {
        case 0x13:
            *(float *)(D_00275C34 + 0x54) = 0.0f;
            *(float *)(D_00275C34 + 0xC) = 6.0f;
            break;
        case 0x14: case 0x15: case 4: case 0x16: case 0x17: case 0x1C:
        case 0x1D: case 0x28: case 0x19: case 0x1A: case 0x29: case 0xE:
        case 0x1F: case 0x21: case 0x22: case 0x1B: case 9: case 0x24:
        case 0x2A:
            *(float *)(D_00275C34 + 0x54) = 0.2f;
            *(float *)(D_00275C34 + 0xC) = 1.5f;
            break;
        case 0x20:
            *(float *)(D_00275C34 + 0x54) = 0.0f;
            *(float *)(D_00275C34 + 0xC) = 2.0f;
            break;
        default:
            *(float *)(D_00275C34 + 0x54) = 0.0f;
            *(float *)(D_00275C34 + 0xC) = 1.5f;
            break;
        }
        *(float *)(D_00275C34 + 8) = *(float *)((char *)D_00255430 + e[0xD] * 8);
        *(int *)(D_00275C34 + 0) = func_00122BB8();
        *(float *)(D_00275C34 + 0x5C) = (float)func_00122BB8() / 2147483648.0f;
        e[0xC] = 0;
        e[9] = 0;
        e[4] = 1;
        switch (e[0xD]) {
        case 0x19: case 0x1A: case 0x13: case 0x1E:
            break;
        case 0: case 0x23:
            func_00102760(e + 0xC0, e + 0xC0);
            func_001CD390(e + 0xD0, e + 0xC0);
            func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
            if (*(int *)(e + 0x38) != 0) {
                func_001F0460(0, e + 0xD0);
            }
            break;
        case 0x18: case 0x1B: case 0x26:
            func_00102760(e + 0xC0, e + 0xC0);
            func_001CD390(e + 0xD0, e + 0xC0);
            func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
            break;
        case 9: case 0x24: case 0xE:
            func_001029C0(e + 0xD0);
            func_00102C58(e + 0xD0, e + 0xD0, e + 0xC0);
            func_00102BB0(e + 0xD0, e + 0xD0, 3.1415927f);
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = 0.0f;
            *(float *)0x700038A8 = -3.5f * (D_008102E8[0] / 0.8f);
            *(float *)0x700038AC = 1.0f;
            func_001026A0(D_700038A0, e + 0xD0, D_700038A0);
            func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
            *(float *)(e + 0x100) += *(float *)0x700038A0;
            *(float *)(e + 0x104) += *(float *)0x700038A4;
            *(float *)(e + 0x108) += *(float *)0x700038A8;
            break;
        case 0xA: case 0xB: case 5: case 1: case 0x25:
            func_001029C0(e + 0xD0);
            func_00102C58(e + 0xD0, e + 0xD0, e + 0xC0);
            func_00102BB0(e + 0xD0, e + 0xD0, 3.1415927f);
            func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
            *(float *)(e + 0x104) += 0.5f;
            break;
        case 4: case 0x16: case 0x17: case 0x1C: case 0x1D: case 0x28:
        case 0x29:
            if (*(int *)(e + 0x38) != 0) {
                *(float *)(e + 0xC0) += (3.1415927f * (120.0f * ((float)func_00122BB8() / 2147483648.0f) - 60.0f)) / 180.0f;
                *(float *)(e + 0xC4) += (3.1415927f * (120.0f * ((float)func_00122BB8() / 2147483648.0f) - 60.0f)) / 180.0f;
                *(float *)(e + 0xC0) = func_001B1470(*(float *)(e + 0xC0));
                *(float *)(e + 0xC4) = func_001B1470(*(float *)(e + 0xC4));
                func_001029C0(e + 0xD0);
                func_00102C58(e + 0xD0, e + 0xD0, e + 0xC0);
                func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
            }
            break;
        default:
            if (*(int *)(e + 0x38) != 0) {
                func_001029C0(e + 0xD0);
                func_00102C58(e + 0xD0, e + 0xD0, e + 0xC0);
                func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
            }
            break;
        }
        /* fall through */
    case 1:
        switch (e[0xD]) {
        case 3: case 0x14: case 0x15: case 4: case 0x16: case 0x17:
        case 0x19: case 0x1A: case 0x29: case 6:
            func_0021B9A0(2, 1.0f, 20.0f);
            func_0021B9A0(3, 1.0f, 20.0f);
            break;
        case 0x1C: case 0x1D: case 0x28:
            func_0021B9A0(2, 1.0f, 100.0f);
            func_0021B9A0(3, 1.0f, 100.0f);
            break;
        }
        n = func_001CCF70(e + 0x100);
        w = (char *)D_00275C34;
        *(int *)(w + 4) = *(int *)(w + 0);
        (*(void (**)(void *, int, void *))((char *)D_00255434 + e[0xD] * 8))(e + 0xD0, n, w);
        *(float *)(D_00275C34 + 0x54) += *(float *)(D_00275C34 + 8);
        lim = *(float *)(D_00275C34 + 0xC);
        if (0.0f == lim) {
            if (!(*(float *)(D_00275C34 + 0x54) <= 2.0f)) {
                *(float *)(D_00275C34 + 0x54) -= 1.0f;
            }
        } else if (!(*(float *)(D_00275C34 + 0x54) <= lim)) {
            e[4] = 3;
        }
        switch (e[0xD]) {
        case 3: case 0x14: case 0x15: case 4: case 0x16: case 0x17:
        case 0x1C: case 0x1D: case 0x28: case 0x19: case 0x1A: case 0x29:
        case 6:
            func_0021B9A0(1, 0.0f, 0.0f);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(e);
        break;
    }
}
