// COMPILER: eegcc
// EEGCC_AS: 2.96
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.68% NEARMISS).
// Fixes read from the original instructions:
//  - D_00241D4C (the current event/argument word) is volatile: the original reloads
//    it for the null test, for the handler argument and for the event-11 compare,
//    and never reuses one load across them.
//  - D_00241D48 is volatile, as the byte-matched sibling func_00112440 declares it.
//  - The "split symbol" note was stale: func_00111740 now has its own symbol, so this
//    unit is the thread body only (the creator is split out to its own file).
//  - Assembled with the 2.96 gas (call/return delay slots filled as the original).

//
// SEMANTICS: SDK service thread body.
//
// func_00111680 is an infinite loop that waits on its semaphore, exits (ExitThread)
// when D_00241D48 == 1, optionally traces, dispatches the registered handler
// D_00241D44(D_00241D4C), runs func_00111AE0 for event 11, and clears the "busy"
// flag D_00241D14 before looping. Its creator func_00111740 fills a ThreadParam-shaped
// block at D_00279B38 with this function as the entry point.
//

extern int D_00241CF8;
extern int D_00241D10;
extern volatile int D_00241D14;
extern void (*D_00241D44)();
extern volatile int D_00241D48;
extern volatile int D_00241D4C;
extern char D_0026BB08[];

extern void WaitSema();
extern void ExitThread();
extern void func_00122B58();
extern void func_00111AE0();

void func_00111680(void)
{
    for (;;) {
        WaitSema(D_00241D10);
        if (D_00241D48 == 1) {
            D_00241D14 = 0;
            ExitThread();
        }
        if (D_00241CF8 > 0) {
            func_00122B58(D_0026BB08, D_00241D44, D_00241D4C);
        }
        if (D_00241D44 != 0 && D_00241D4C != 0) {
            D_00241D44(D_00241D4C);
        }
        if (D_00241D4C == 11) {
            func_00111AE0();
        }
        D_00241D14 = 0;
    }
}
