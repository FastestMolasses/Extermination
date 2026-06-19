// CFLAGS: -O4,p -sdatathreshold 0
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
