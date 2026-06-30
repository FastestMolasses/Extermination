// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-row processing: runs func_00180420 setup, then two func_001031E0
// transforms of arg0+0x290 into scratch blocks D_700038A0/D_700038B0,
// nudges D_700038A4 by -3.0f and D_700038B4 by -4.5f, tests via
// func_0019AB20 (returns 2 on hit), else normalizes func_00180300 result.
extern void func_00180420(int);
extern void func_001031E0(float *, float *);
extern int func_0019AB20(char *, char *, char *, int);
extern int func_00180300(char *, char *, unsigned char);
extern char D_700038A0[0x100];
extern char D_700038B0[0x100];

int func_00180530(char *arg0) {
    func_00180420((int)arg0);
    func_001031E0((float *)D_700038A0, (float *)(arg0 + 0x290));
    *(float *)0x700038A4 = *(float *)0x700038A4 - 3.0f;
    func_001031E0((float *)D_700038B0, (float *)(arg0 + 0x290));
    *(float *)0x700038B4 = *(float *)0x700038B4 - 4.5f;
    if (func_0019AB20(arg0, D_700038B0, arg0 + 0x280, 6) != 0) {
        return 2;
    }
    return func_00180300(arg0, D_700038A0, *(unsigned char *)(arg0 + 0xD)) ? 1 : 0;
}
