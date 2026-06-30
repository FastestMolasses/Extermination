// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Seeds the scratch matrix block at 0x700038A0 (identity-ish: col1 = -1.0,
// col3 = 1.0), then builds it from the table referenced through the global
// at D_00275B40 (row at +0x14, offset +0x90) via func_001026A0. Picks one of
// two 4-int parameter sets written to the scratch block at 0x700038B0
// depending on the byte flag at p+0xE1, then hands both scratch blocks to
// func_001F4A00.
extern void func_001026A0(void *dst, int n, void *src);
extern void func_001F4A00(void *a, void *b);
extern int D_00275B40;
extern char D_700038A0[];
extern char D_700038B0[];

void func_001288D0(int arg0, char *p) {
    int hi;

    hi = 0x80;
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0xBFC00000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038A0, *(int *)(D_00275B40 + 0x14) + 0x90, D_700038A0);
    if (*(unsigned char *)(p + 0xE1) == 0) {
        *(int *)0x700038B0 = 0x20;
        *(int *)0x700038B4 = 0x70;
        *(int *)0x700038B8 = hi;
        *(int *)0x700038BC = hi;
    } else {
        *(int *)0x700038B0 = hi;
        *(int *)0x700038B4 = 0x50;
        *(int *)0x700038B8 = 0x30;
        *(int *)0x700038BC = hi;
    }
    func_001F4A00(D_700038A0, D_700038B0);
}
