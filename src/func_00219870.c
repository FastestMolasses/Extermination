// NEARMISS func_00219870  (vram 0x00219870, 0x6D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered and byte-identical up to a register-allocation-order permutation: mwcc233 colors the state-byte load into a0 (my object emits it into a1 like the target expects the OPPOSITE arg live at that point) and a handful of downstream a0/a1/v0/v1 uses follow suit; this casca...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int func_001026A0(float *a, void *b, float *c);
extern float func_00102738(float *a, float *b);
extern void func_00102760(float *a, float *b);
extern void func_001028B8(float *a, void *b, float *c);
extern void func_001028D0(float *a, void *b, void *c);
extern void func_00102900(float *a, float *b, float c);
extern float func_0011E748(float x);
extern int func_0019AA80(void *a, char *b, int c);
extern void func_001AFC10(unsigned char a);
extern int func_001B0FD0(unsigned char a);
extern void func_001B1190(unsigned char a);
extern int func_001B11E0(int a);
extern unsigned char func_001B1630(float a, float b, float c);
extern void func_001B1B70(char *arg0);
extern char *func_001C5570(char *arg0, void *arg1, int arg2, int arg3);
extern void func_001C6380(char *arg0);
extern void func_001EFD20(int a, void *b);
extern void func_001FBD50(char *arg0, int arg1, int arg2, float arg3);
extern void func_00219F50(char *arg0);
extern void func_0021A180(char *arg0);
extern void func_0021A440(char *arg0, void *arg1);
extern int D_00275B40;
extern unsigned char *D_008102C8;
extern float D_700038A0[4];
extern float D_700038B0[4];
extern char D_00810360[8];

void func_00219870(char *arg0) {
    unsigned char state;
    char *sub = arg0 + 0x1F0;
    state = *(unsigned char *)(arg0 + 4);

    switch (state) {
    case 0:
        if (func_001B0FD0(state) == 0) {
            *(short *)(arg0 + 0x34) = 1;
            *(unsigned char *)(arg0 + 0) = 1;
            *(int *)(arg0 + 0x2D8) = 0;
            func_001C6380(arg0);
            func_00219F50(arg0);
            *(int *)(arg0 + 0x2C4) = 0x14;
            *(float *)0x700038A0 = 1.0f;
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = 0.0f;
            *(float *)0x700038AC = 0.25f;
            *(char **)(arg0 + 0x2CC) = func_001C5570(arg0, D_700038A0, 0x7C, 1);
            if (func_001B11E0((*(unsigned char *)(arg0 + 0x9A) + 1) & 0xFF) != 0) {
                *(short *)(arg0 + 0x28) = 0;
                *(unsigned char *)(arg0 + 5) = 2;
                func_0021A440(arg0, sub);
                return;
            }
        }
        return;

    case 1: {
        unsigned char sub_state;
        *(short *)(arg0 + 0x2A) = *(short *)(arg0 + 0x2A) + 1;
        sub_state = *(unsigned char *)(arg0 + 5);
        switch (sub_state) {
        case 0:
            if ((*D_008102C8) != 1 && func_0019AA80(sub, sub + 0x10, 0x40) != 0) {
                func_001FBD50(arg0, 0x429, 0, 300.0f);
                *(unsigned char *)(arg0 + 0) = 2;
                *(short *)(arg0 + 0x28) = 0;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_0021A440(arg0, sub);
                *(float *)(arg0 + 0x2E8) = 0.0f;
                *(int *)(arg0 + 0x2D8) = 1;
                *(float *)0x700038AC = 0.0f;
                *(float *)0x700038A4 = 0.0f;
                *(float *)0x700038A0 = 0.0f;
                *(float *)0x700038A8 = 0.2f;
                func_001026A0(D_700038A0, (void *)(*(int *)(D_00275B40 + 0) + 0x90), D_700038A0);
                *(float *)(arg0 + 0x2E4) = *(float *)0x700038A0;
                *(float *)(arg0 + 0x2E0) = *(float *)0x700038A8;
            }
            if (*(short *)(arg0 + 0x36) != 0) {
                func_001B1190((*(unsigned char *)(arg0 + 0x9A) + 1) & 0xFF);
                func_001FBD50(arg0, 0x42A, 0, 300.0f);
                *(short *)(arg0 + 0x28) = 0;
                *(unsigned char *)(arg0 + 5) = 2;
                func_001028D0(D_700038A0, sub + 0x10, sub);
                *(int *)0x700038AC = 0;
                func_00102760(D_700038A0, D_700038A0);
                func_001028D0(D_700038B0, D_00810360, sub);
                *(int *)0x700038BC = 0;
                *(float *)0x70003A20 = func_0011E748(func_00102738(D_700038B0, D_700038B0));
                func_00102760(D_700038B0, D_700038B0);
                *(float *)0x70003A24 = *(float *)0x70003A20 * func_00102738(D_700038A0, D_700038B0);
                func_00102900(D_700038A0, D_700038A0, *(float *)0x70003A24);
                func_001028B8(D_700038A0, sub, D_700038A0);
                func_0021A440(arg0, D_700038A0);
            }
            {
                short v = *(short *)(arg0 + 0x2A) & 0x7F;
                *(short *)0x70003B88 = v;
                if (v >= 0x40) {
                    *(short *)0x70003B88 = 0x7F - v;
                }
            }
            {
                float f = (*(short *)0x70003B88 * 2) / 128.0f;
                *(float *)0x70003A20 = f;
                *(float *)(*(char **)(arg0 + 0x2CC) + 0xA0) = f;
            }
            *(int *)(*(char **)(arg0 + 0x2CC) + 0xA4) = 0;
            *(int *)(*(char **)(arg0 + 0x2CC) + 0xA8) = 0;
        block_25:
            *(float *)(*(char **)(arg0 + 0x2CC) + 0xAC) = 0.25f;
            break;
        case 1:
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) + 1;
            if (*(short *)(arg0 + 0x28) >= (*(int *)(arg0 + 0x2C4) + 0xA) >> 2) {
                func_001FBD50(arg0, 0x42B, 0, 300.0f);
                if (*(unsigned char *)(arg0 + 0) == 2) {
                    *(short *)(arg0 + 0x2A) = 0;
                    *(short *)(arg0 + 0x28) = 0;
                    {
                        char *q = sub + 0xD4;
                        *(int *)q = *(int *)q >> 2;
                    }
                    *(unsigned char *)(arg0 + 4) = 2;
                }
            }
            if (*(short *)(arg0 + 0x2A) & 8) {
                *(float *)(*(char **)(arg0 + 0x2CC) + 0xA0) = 1.0f;
            } else {
                *(float *)(*(char **)(arg0 + 0x2CC) + 0xA0) = 0.0f;
            }
            *(int *)(*(char **)(arg0 + 0x2CC) + 0xA4) = 0;
            *(int *)(*(char **)(arg0 + 0x2CC) + 0xA8) = 0;
            goto block_25;
        }
        if (*(int *)(arg0 + 0x2D8) != 0) {
            char *p;
            *(float *)(arg0 + 0x2E8) = *(float *)(arg0 + 0x2E8) + -0.015f;
            p = *(char **)(D_00275B40 + 4);
            *(float *)(p + 0xC0) = *(float *)(arg0 + 0x2E4) + *(float *)(p + 0xC0);
            p = *(char **)(D_00275B40 + 4);
            *(float *)(p + 0xC4) = *(float *)(arg0 + 0x2E8) + *(float *)(p + 0xC4);
            p = *(char **)(D_00275B40 + 4);
            *(float *)(p + 0xC8) = *(float *)(arg0 + 0x2E0) + *(float *)(p + 0xC8);
        }
        *(unsigned char *)(arg0 + 1) = func_001B1630(*(float *)(arg0 + 0xB0), *(float *)(arg0 + 0xB4), *(float *)(arg0 + 0xB8));
        if (*(unsigned char *)(arg0 + 1) != 0) {
            if ((int)*(unsigned char *)(arg0 + 5) < 2 && (*D_008102C8) == 1) {
                func_001B1B70(arg0);
            }
            (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        }
        func_0021A180(arg0);
        return;
    }

    case 2: {
        short cnt;
        *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) + 1;
        cnt = *(short *)(arg0 + 0x28);
        if (cnt >= 4) {
            if (cnt == 4) {
                func_001FBD50(arg0, 0x42C, 0, 300.0f);
            }
            *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
            *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
            *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038A0 = *(float *)(arg0 + 0xB0) + *(float *)(arg0 + 0x2D4) * (float) *(short *)(arg0 + 0x2A);
            *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
            *(float *)0x700038A8 = *(float *)(arg0 + 0xB8) + *(float *)(arg0 + 0x2D0) * (float) *(short *)(arg0 + 0x2A);
            *(float *)0x700038AC = 1.0f;
            func_001EFD20(0x8000006D, D_700038A0);
            *(short *)(arg0 + 0x2A) = *(short *)(arg0 + 0x2A) + 1;
        }
        cnt = *(short *)(arg0 + 0x2A);
        if (*(float *)(arg0 + 0x2C8) < 6.0f * (float) cnt || cnt == 8) {
            *(unsigned char *)(arg0 + 4) = 3;
            if (*(char **)(arg0 + 0x2CC) != 0) {
                func_001B1190(*(unsigned char *)(arg0 + 0x9A));
                *(char *)(*(char **)(sub + 0xDC) + 4) = 3;
                return;
            }
        }
        break;
    }

    case 3:
        func_001AFC10(state);
        break;
    }
}
