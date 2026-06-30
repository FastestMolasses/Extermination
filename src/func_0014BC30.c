// NEARMISS func_0014BC30  (vram 0x0014BC30, 0x1E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.60% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP even/odd register-coloring permutation + constant-CSE difference (permuter-class). Body and all control flow fully recovered (o233 87.6%). The scratch-fill block (load D_00810350..5C constant table, store object fields arg0+0xB0..0xBC and the const table into the D_700038A0/B0 VU0 scratchpad, ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_00122BB8(void);
extern int func_001B13F0(float *a, float *b, float v);
extern int func_001B1560(char *a, float *b, float v);
extern int func_0021BE40(char *a, char *b);
extern char D_008102B0;
extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern float D_0081035C;
extern unsigned char D_008104E4;
extern float D_700038A0;
extern float D_700038B0;

int func_0014BC30(char *arg0, char *arg1) {
    float c0, c1, c2, c3;

    if (*(int *)(arg1 + 0x38) == 0) {
        return 0;
    }
    if (!((func_00122BB8() >> 0x13) & 1)) {
        return 0;
    }
    if (func_0021BE40(&D_008102B0, arg0) != 0) {
        return 0;
    }
    c0 = D_00810350;
    c1 = D_00810354;
    c2 = D_00810358;
    c3 = D_0081035C;
    *(volatile float *)0x700038A0 = *(float *)(arg0 + 0xB0);
    *(volatile float *)0x700038A4 = *(float *)(arg0 + 0xB4);
    *(volatile float *)0x700038A8 = *(float *)(arg0 + 0xB8);
    *(volatile float *)0x700038AC = *(float *)(arg0 + 0xBC);
    *(volatile float *)0x700038B0 = c0;
    *(volatile float *)0x700038B4 = c1;
    *(volatile float *)0x700038B8 = c2;
    *(volatile float *)0x700038BC = c3;
    *(float *)0x700038A4 = *(float *)0x700038A4 + 15.0f;
    *(float *)0x700038B4 = *(float *)0x700038B4 + 15.0f;
    if (D_008104E4 != 1) {
        if (func_001B13F0(&D_700038B0, &D_700038A0, 30.0f) != 0) {
            return 0;
        }
    }
    if (func_001B13F0(&D_700038B0, &D_700038A0, 100.0f) == 0) {
        return 0;
    }
    if (func_001B1560(arg0, &D_700038B0, 0.3926991f) == 0) {
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
