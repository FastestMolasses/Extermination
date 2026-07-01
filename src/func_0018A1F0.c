// NEARMISS func_0018A1F0  (vram 0x0018A1F0, 0x4B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residual clusters: (1) copy_qw4 arg register swap (v0/v1 evaluation-order regalloc noise); (2) mwcc233 speculatively hoists a pure lui (address-hi computation, no side effect) into a branch delay slot the target leaves as nop -- confirmed not blockable via volatile. Neither is the clean-store...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void copy_qw4(void *dst, void *src);
extern int func_001026A0(void *dst, void *src, void *m);
extern int func_001028B8(void *a, void *b, void *c);
extern int func_00102948(void *dst, void *src);
extern int func_001031E0(void *a, void *b);
extern void func_00189D30(unsigned char *arg0);
extern int func_00189EC0(int a);
extern void func_00189FE0(unsigned char *arg0, void *a, void *b);
extern void func_0018A180(unsigned char *arg0);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_0019B2C0(void *a, void *b, int n);
extern void func_001AA840(unsigned char *arg0);
extern void func_001F00A0(int a, void *b, void *c, int d);

extern int **D_00275B40;
extern int D_008103F8[2];
extern float D_00810360[2];
extern int D_0024A440[];
extern int D_0024A450[];
extern int D_0024A4A0[];
extern float D_700031B0[2];
extern int D_700038A0[4];
extern int D_700038B0[4];
extern int D_700038C0[4];
extern int D_700038D0[4];

void func_0018A1F0(unsigned char *arg0) {
    int sp30[4];
    int sp40[4];
    int sp50[4];
    int i;
    int n;
    char *p;

    copy_qw4((char *)D_008103F8[0] + 0x90, (char *)*D_00275B40 + 0x90);

    if (*arg0 & 1) {
        func_001AA840(arg0);
        *(int *)0x700038A0 = 0x3E800000;
        *(int *)0x700038A4 = 0x3F800000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0(D_700038A0, (char *)*D_00275B40 + 0x90, D_700038A0);
        if (func_0019B2C0(D_00810360, D_700038A0, 6) != 0) {
            if (*(int *)0x700031D4 != 0) {
                if (func_00189EC0(*(int *)0x700031D4) == 0 && (*(short *)(*(int *)0x700031D0 + 0x1A) & 0xFF00) == 0x2000) {
                    func_001031E0(D_700038C0, D_700031B0);
                    p = *(char **)0x700031D0;
                    *(float *)0x700038B0 = *(float *)(p + 0x24);
                    *(float *)0x700038B4 = *(float *)(p + 0x28);
                    *(float *)0x700038B8 = *(float *)(p + 0x2C);
                    *(int *)0x700038BC = 0x3F800000;
                    func_00102948(D_700038A0, D_700031B0);
                    func_001F00A0(0x80000003, D_700038A0, D_700038B0, 0);
                    func_0018A180(arg0);
                }
            } else if ((*(short *)(*(int *)0x700031D0 + 0x1A) & 0xFF00) == 0x2000) {
                func_001031E0(D_700038C0, D_700031B0);
                p = *(char **)0x700031D0;
                *(float *)0x700038B0 = *(float *)(p + 0x24);
                *(float *)0x700038B4 = *(float *)(p + 0x28);
                *(float *)0x700038B8 = *(float *)(p + 0x2C);
                *(int *)0x700038BC = 0x3F800000;
                func_00102948(D_700038A0, D_700031B0);
                func_001F00A0(0x80000003, D_700038A0, D_700038B0, 0);
                func_0018A180(arg0);
            }
        }
    }

    if (*arg0 & 1) {
        func_001026A0(sp30, (char *)*D_00275B40 + 0x90, D_0024A440);
        func_001026A0(sp40, (char *)*D_00275B40 + 0x90, D_0024A450);
        n = func_0019A570(sp30, sp40, 7, 0x20);
        if (n != 0) {
            if ((unsigned int)(n - 1) < 2U) {
                func_00189FE0(arg0, sp40, sp30);
            }
        }
        i = 0;
        do {
            func_001026A0(sp50, (char *)*D_00275B40 + 0x90, (char *)D_0024A440 + (i + 2) * 0x10);
            func_001028B8(D_700038A0, sp50, sp30);
            func_001028B8(D_700038B0, sp50, sp40);
            n = func_0019A570(D_700038A0, D_700038B0, 7, 0x20);
            if (n != 0 && (unsigned int)(n - 1) < 2U) {
                func_00189FE0(arg0, D_700038B0, D_700038A0);
            }
            i += 1;
        } while (i < 2);
        i = 0;
        do {
            func_001026A0(sp50, (char *)*D_00275B40 + 0x90, (char *)D_0024A440 + (i + 4) * 0x10);
            func_001028B8(D_700038C0, sp50, sp30);
            func_001028B8(D_700038D0, sp50, sp40);
            n = func_0019A570(D_700038C0, D_700038D0, 7, 0x20);
            if (n != 0 && (unsigned int)(n - 1) < 2U) {
                func_00189FE0(arg0, D_700038D0, D_700038C0);
            }
            i += 1;
        } while (i < 2);
        func_001026A0(D_700038A0, (char *)*D_00275B40 + 0x90, D_0024A4A0);
        func_00102948(D_700038B0, D_00810360);
        *(float *)0x700038B4 = *(float *)0x700038A4;
        n = func_0019A570(D_700038B0, D_700038A0, 7, 0x20);
        if (n != 0 && (unsigned int)(n - 1) < 2U) {
            func_00189FE0(arg0, D_700038A0, D_700038B0);
        }
    }

    func_00189D30(arg0);
}
