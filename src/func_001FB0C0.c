// CFLAGS: -O4,p -sdatathreshold 0
// Worker/poll loop: refresh thread status, then spin calling func_001192D0()
// until it returns 0, then run func_001152B0() and repeat forever. The five
// nops after the poll call are hardware/pipeline padding emitted by the
// original; reproduced with an inline-asm nop block (plain C control flow
// otherwise). Byte-identical under both 991202 and 2.3.3; 991202 chosen.
extern void ReferThreadStatus(void);
extern int func_001192D0(void);
extern void func_001152B0(void);

void func_001FB0C0(void) {
    int r;
    for (;;) {
        ReferThreadStatus();
        do {
            r = func_001192D0();
            asm { nop; nop; nop; nop; nop; }
        } while (r != 0);
        func_001152B0();
    }
}
