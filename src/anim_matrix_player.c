// NEARMISS anim_matrix_player  (vram 0x0017B660, 0x2A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS: player-character bone-matrix-transition writer (documented in docs/FINDINGS.md ~line 2723 as the writer of D_00287F40/D_00288D40, driven by D_00275B40, the per-bone source-pointer table published by func_001CB5B0). Fully recovered logic: state-driven branch on arg0+0x1F1 selecting a sin...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern void func_00102958(void *, void *);
extern void func_001749F0(char *, int, int, float);
extern void func_00179D20(char *);
extern void func_00179FF0(char *);
extern int func_0017B490(int, unsigned char, int);
extern float func_001C61D0(int, short);
extern void func_001C9D50(void *, void *, void *, float);
extern char **D_00275B40;
extern char D_00287F40;
extern char D_00288D40;

void anim_matrix_player(char *arg0) {
    char *s0 = arg0;
    int i;
    char *p;
    short a1;
    short s1;
    short s5;
    float f20, f21, f22, f0;

    if (*(unsigned char *)(s0 + 0x1F1) == 0) {
        s1 = (short) func_0017B490(1, *(unsigned char *)(s0 + 0x235), *(unsigned char *)(s0 + 0x25C));
        a1 = *(short *)(s0 + 0x20C);
        if (s1 != a1) {
            f20 = func_001C61D0(*(int *)(s0 + 0x40), a1);
            f0 = func_001C61D0(*(int *)(s0 + 0x40), s1);
            func_001749F0(s0, s1, 0, f0 * ((f20 - *(float *)(s0 + 0x3C)) / f20));
        }
        return;
    }
    if (*(unsigned char *)(s0 + 0x1F1) == 1) {
        s5 = (short) func_0017B490(1, *(unsigned char *)(s0 + 0x235), *(unsigned char *)(s0 + 0x25C) + 1);
    } else {
        s5 = (short) func_0017B490(1, *(unsigned char *)(s0 + 0x235), *(unsigned char *)(s0 + 0x25C) - 1);
    }
    a1 = *(short *)(s0 + 0x20C);
    f20 = *(float *)(s0 + 0x3C);
    s1 = a1;
    f21 = func_001C61D0(*(int *)(s0 + 0x40), a1);
    f22 = func_001C61D0(*(int *)(s0 + 0x40), s5);
    func_00179D20(s0);

    p = &D_00288D40;
    for (i = 0; i < *(unsigned char *)(s0 + 0xC); i++) {
        func_00102958(p, D_00275B40[i] + 0x90);
        p += 0x40;
    }

    func_001749F0(s0, s5, 0, f22 * ((f21 - f20) / f21));
    func_00179D20(s0);

    p = &D_00287F40;
    for (i = 0; i < *(unsigned char *)(s0 + 0xC); i++) {
        func_00102958(p, D_00275B40[i] + 0x90);
        p += 0x40;
    }

    {
        char *p2 = &D_00287F40;
        char *p3 = &D_00288D40;
        for (i = 0; i < *(unsigned char *)(s0 + 0xC); i++) {
            func_001C9D50(D_00275B40[i] + 0x90, p3, p2, *(float *)(s0 + 0x208));
            p2 += 0x40;
            p3 += 0x40;
        }
    }

    func_00179FF0(s0);
    if (*(float *)(s0 + 0x208) < 1.0f) {
        func_001749F0(s0, a1, 0, f21 - f20);
    }
}
