// NEARMISS func_0014BC30  (vram 0x0014BC30, 0x1E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-28 (was 87.60%): every scratchpad word is a relocated volatile extern
// (idiom-32, // SPAD:), the D_00810350.. table is copied straight into 0x700038B0..BC, the two
// +15.0f height offsets read both words before writing either, and func_001B13F0 / func_001B1560
// are declared with the float first (as the byte-matched func_0012F6C0 does), which puts the
// float constant ahead of the address arguments like the original.
// Remaining differences: the two height additions put the 15.0f register first; the original
// has it as the second operand (two operand fields, everything else identical). A local for
// 15.0f fixes the operand order but swaps the FP registers of the constant and the first word.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B0 0x700038B4 0x700038B8 0x700038BC

extern int func_00122BB8(void);
extern int func_001B13F0(float r, volatile void *a, volatile void *b);
extern int func_001B1560(float angle, char *a, volatile void *b);
extern int func_0021BE40(char *a, char *b);
extern char D_008102B0;
extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern float D_0081035C;
extern unsigned char D_008104E4;
extern volatile float D_700038A0[];
extern volatile float D_700038A4[];
extern volatile float D_700038A8[];
extern volatile float D_700038AC[];
extern volatile float D_700038B0[];
extern volatile float D_700038B4[];
extern volatile float D_700038B8[];
extern volatile float D_700038BC[];

int func_0014BC30(char *arg0, char *arg1) {

    if (*(int *)(arg1 + 0x38) == 0) {
        return 0;
    }
    if (!((func_00122BB8() >> 0x13) & 1)) {
        return 0;
    }
    if (func_0021BE40(&D_008102B0, arg0) != 0) {
        return 0;
    }
    D_700038A0[0] = *(float *)(arg0 + 0xB0);
    D_700038A4[0] = *(float *)(arg0 + 0xB4);
    D_700038A8[0] = *(float *)(arg0 + 0xB8);
    D_700038AC[0] = *(float *)(arg0 + 0xBC);
    D_700038B0[0] = D_00810350;
    D_700038B4[0] = D_00810354;
    D_700038B8[0] = D_00810358;
    D_700038BC[0] = D_0081035C;
    {
        float y0 = D_700038A4[0] + 15.0f;
        float y1 = D_700038B4[0] + 15.0f;
        D_700038A4[0] = y0;
        D_700038B4[0] = y1;
    }
    if (D_008104E4 != 1) {
        if (func_001B13F0(30.0f, D_700038B0, D_700038A0) != 0) {
            return 0;
        }
    }
    if (func_001B13F0(100.0f, D_700038B0, D_700038A0) == 0) {
        return 0;
    }
    if (func_001B1560(0.3926991f, arg0, D_700038B0) == 0) {
        return 0;
    }
    *(char *)(arg0 + 5) = 6;
    *(char *)(arg0 + 6) = 0;
    if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
        if (((func_00122BB8() >> 0x12) & 7) < 3) {
            *(char *)(arg0 + 5) = 5;
        }
    }
    return 1;
}
