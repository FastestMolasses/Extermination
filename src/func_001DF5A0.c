// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// The original never writes f12 before calling func_001DF180, so its caller's
// f12 (func_001E7310 / func_001E7440 pass 0.2 * self+0x60) reaches
// func_001DF180 as that function's float argument: declared and passed here
// as `amp` (corrected lane DFIX 2026-09-28; was declared without it).
// The advanced cursor (+0x1C += 0x10) is stored before func_001CB760 runs
// (the original's store sits in the call's delay slot); the old C stored it
// after the call. func_001CB760 receives the cursor read before the advance.
// With both corrections mwcc 2.3.3 matches (objdiff 56.92% -> 100%).
extern char *D_00275670;
extern char D_007635C0[8];
extern int func_001DF180(int, float);
extern void func_001CB760(char *, int, int, char *);

void func_001DF5A0(float amp) {
    char *t1;
    char *a3;
    char *v0;
    int a2;
    int a1;
    int t0;

    a2 = func_001DF180(3, amp);
    t1 = D_00275670;
    t0 = 0x60;
    a3 = *(char **)(t1 + 0x1c);
    a1 = 0x00fff000;
    a3[3] = t0;
    v0 = *(char **)(t1 + 0x1c);
    *(int *)(v0 + 4) = 0;
    v0 = *(char **)(t1 + 0x1c);
    *(short *)v0 = 0;
    v0 = *(char **)(t1 + 0x1c);
    v0 = v0 + 0x10;
    *(char **)(t1 + 0x1c) = v0;
    func_001CB760(D_007635C0, a1, a2, a3);
}
