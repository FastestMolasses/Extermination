// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Reset/teardown on the actor at `arg0`: clear the state byte at +0x234 and
// the global flag D_00810707, run func_0015C1F0(arg0), then if the linked
// record pointer at +0x1C is non-null mark that record's +0x4 = 3 (a
// detach/free state) and null out the +0x1C link; finally run the cleanup
// func_0015C7C0(arg0). Returns nothing.
//
// 233-only: under 991202 the lone residual is the clean-store delay-slot nop
// of the beqz/sw branch idiom (wall #13); mwcc 2.3.3 leaves the nop where CW
// does. Verified objdiff 100.0 vs build/expected/func_0015C750.o.
extern void func_0015C1F0(char *);
extern void func_0015C7C0(char *);
extern unsigned char D_00810707;

void func_0015C750(char *arg0) {
    int *p;

    *(char *)(arg0 + 0x234) = 0;
    D_00810707 = 0;
    func_0015C1F0(arg0);
    p = *(int **)(arg0 + 0x1C);
    if (p != 0) {
        *(char *)((char *)p + 4) = 3;
        *(int *)(arg0 + 0x1C) = 0;
    }
    func_0015C7C0(arg0);
}
