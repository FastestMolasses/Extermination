// COMPILER: eegcc
// CFLAGS: -O2

//
// libpad bring-up: publishes the "pad system active" flag, then opens the two
// controller ports through the SIF helper func_0010E6F8, spinning on a
// 0x10000-iteration nop delay loop until each port's ready word (D_00279700[9]
// for port 0, [19] for port 1) goes non-zero. Once both are up it queries the
// libpad module version (func_001114B8): the high byte must be 3, otherwise it
// prints the two diagnostic strings and bails with 0. On success it tail-calls
// func_001107C8 with the caller's argument.

extern int func_0010E6F8(int a0, int a1, int a2);
extern int func_001114B8(void);
extern void func_00122B58(char *fmt, ...);
extern int func_001107C8(int a0);

extern int D_00241CB0;
extern int D_00279700[];
extern char D_0026BA28[];
extern char D_0026BA50[];

int sub_PsIIlibpad_2000(int a0)
{
    int i;
    int r;

    D_00241CB0 = 1;

    for (;;) {
        func_0010E6F8((int)&D_00279700[0], 0x80000100, 0);
        if (D_00279700[9] != 0) {
            break;
        }
        i = 0x10000;
        do {
            i--;
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        } while (i != -1);
    }

    for (;;) {
        func_0010E6F8((int)&D_00279700[10], 0x80000101, 0);
        if (D_00279700[19] != 0) {
            break;
        }
        i = 0x10000;
        do {
            i--;
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        } while (i != -1);
    }

    r = func_001114B8();
    if ((r >> 8) != 3) {
        func_00122B58(D_0026BA28);
        func_00122B58(D_0026BA50, 3, 0, r >> 8, r & 0xFF);
        return 0;
    }
    return func_001107C8(a0);
}
