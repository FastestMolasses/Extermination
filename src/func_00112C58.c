// COMPILER: eegcc
// EEGCC_AS: 2.96
// CFLAGS: -O2

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.67% NEARMISS).
// The C was already right: the original fills a call delay slot that only the
// 2.96 gas fills (// EEGCC_AS: 2.96); found by the 2.96 sweep over eegcc NEARMISS files.
//

extern char D_00279E00[];
extern volatile int D_00241D08;
extern volatile int D_00241D48;
extern volatile int D_00241D14;
extern int func_00112088(void);
extern int func_00111F18(int);
extern int func_001115D0();
extern int func_0010E8A8(void *, int, int, int, int, int, int, void *, void *);
extern int SignalSema(int);

int func_00112C58(void)
{
    if (func_00112088() == 6) {
        return 0;
    }
    if (func_00111F18(0xC) == 0) {
        return 0;
    }
    D_00241D48 = 7;
    D_00241D14 = 1;
    if (func_0010E8A8(D_00279E00, 8, 1, 0, 0, 0, 0, func_001115D0,
                      (void *)&D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        SignalSema(D_00241D08);
        return 0;
    }
    SignalSema(D_00241D08);
    return 1;
}
