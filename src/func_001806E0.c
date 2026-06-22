// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_00180420(int);
extern int func_00180300(char *, char *, unsigned char);
extern char D_700038A0[0x100];
extern int D_00275B40;

int func_001806E0(char *arg0) {
    func_00180420((int)arg0);
    *(float *)0x700038A0 = *(float *)(arg0 + 0x290);
    *(float *)0x700038A8 = *(float *)(arg0 + 0x298);
    *(float *)0x700038A4 = 18.0f + *(float *)(arg0 + 0xB4);
    if (func_00180300(arg0, D_700038A0, *(unsigned char *)(arg0 + 0xD)) == 2) {
        return 2;
    }
    *(float *)0x700038A4 = 6.0f + *(float *)(*(int *)(D_00275B40 + 4) + 0xC4);
    return func_00180300(arg0, D_700038A0, *(unsigned char *)(arg0 + 0xD));
}
