// NEARMISS func_0016EBA0  (vram 0x0016EBA0, 0x3AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-ORDER wall: target loads the state byte directly into a0 (reusing arg0's register) and lazily materializes the arg0->s0 callee-save copy after the first case-30 compare fails; mwcc233 eagerly copies arg0->s0 at entry and keeps the state byte in a1, shifting every subsequent co...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001B0B50(unsigned char);
extern void func_001749A0(char *, int, int, float);
extern void anim_eval_skeleton(char *);
extern void func_001026A0(char *, char *, char *);
extern void build_trs_matrix(char *, char *, char *, char *);
extern int func_0019AD00(char *, char *, int);
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern int func_00188550(char *);
extern void func_00179880(char *);
extern int func_00175900(char *, int);
extern void func_00182870(char *, int);
extern short D_0028A9A0;
extern char D_700038A0[];
extern char D_700038B0[];
extern float D_700031B0;
extern float D_700031B8;
extern char *D_700031D0;
extern float D_70003A20;

void func_0016EBA0(char *arg0) {
    unsigned char st;
    char *v;
    short t;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        func_001B0B50(st);
        *(int *)(arg0 + 0x38) = 0;
        if (*(unsigned char *)(arg0 + 0xD) == 0) {
            *(unsigned char *)(arg0 + 6) = 0xA;
            func_001749A0(arg0, 0x153, 0, 1.0f);
            anim_eval_skeleton(arg0);
            *(volatile float *)0x700038A0 = 0.0f;
            *(volatile float *)0x700038A4 = 0.0f;
            *(volatile float *)0x700038A8 = 5.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(arg0 + 0xB0, arg0 + 0xD0, D_700038A0);
            return;
        }
        if (*(unsigned char *)(arg0 + 0xD) == 1) {
            *(volatile float *)0x700038A0 = 0.0f;
            *(volatile float *)0x700038A4 = 0.0f;
            *(volatile float *)0x700038A8 = 5.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(arg0 + 0xB0, arg0 + 0xD0, D_700038A0);
            build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, arg0 + 0xC0, arg0 + 0x60);
            func_001749A0(arg0, 0x72, 0, 1.0f);
            *(unsigned char *)(arg0 + 6) = 0x14;
            *(int *)(arg0 + 0x2EC) = 0;
            *(short *)(arg0 + 0x28) = 8;
            return;
        }
        *(volatile float *)0x700038A0 = 0.0f;
        *(volatile float *)0x700038A4 = -3.0f;
        *(volatile float *)0x700038A8 = -5.0f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
        if (func_0019AD00(arg0, D_700038B0, 7) != 0) {
            v = (char *)*(volatile int *)0x700031D0;
            *(float *)(arg0 + 0xB0) = *(volatile float *)0x700031B0 + (1.5f * *(float *)(v + 0x24));
            v = (char *)*(volatile int *)0x700031D0;
            *(float *)(arg0 + 0xB8) = *(volatile float *)0x700031B8 + (1.5f * *(float *)(v + 0x2C));
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 20.5f;
            v = (char *)*(volatile int *)0x700031D0;
            *(volatile float *)0x70003A20 = func_0011E620(-*(float *)(v + 0x2C), *(float *)(v + 0x24));
            *(float *)(arg0 + 0xC4) = func_001B1470(4.712389f + *(volatile float *)0x70003A20);
            *(unsigned char *)(arg0 + 6) = 0x1E;
            func_001749A0(arg0, func_00188550(arg0), 0, 0.0f);
            return;
        }
        return;
    case 10:
        if (D_0028A9A0 == 0 && (*(int *)(arg0 + 0x200) & 0x1000)) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0;
            return;
        }
        break;
    case 20:
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            return;
        }
        break;
    case 21:
        func_00179880(arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_00182870(arg0, 1);
            func_001749A0(arg0, 0x6D, 0, 8.0f);
            return;
        }
        break;
    case 22:
        if (D_0028A9A0 == 0 && (*(int *)(arg0 + 0x200) & 0x1000)) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0;
            return;
        }
        break;
    case 30:
        if (D_0028A9A0 == 0) {
            *(char *)(arg0 + 5) = 0x18;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x2C;
            *(char *)(arg0 + 0x1F1) = 0;
            *(unsigned char *)(arg0 + 0xD) = 2;
        }
        break;
    }
}
