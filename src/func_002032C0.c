// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Fills 9 entries of an 8-byte-stride output table at D_00821010 from a
// parallel int table of selectors at D_00264FB0. For each of the 9 entries:
// spins calling func_00113280(0) until func_00111C28(buf, selector) returns
// nonzero (buf is a stack scratch the callee writes), then copies buf[0]/buf[1]
// into the two words of the current output entry. buf is sized [8] to reproduce
// the original 0x60-byte stack frame (the callee writes a 0x20-byte region at
// sp+0x40; only the first two words are consumed here).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906). Verified objdiff 100% vs
// build/expected/func_002032C0.o. (991202 = 90.1%.) Counter init (i=0) before
// the pointer inits is load-bearing for the saved-register assignment.
extern void func_00113280(int);
extern int func_00111C28(int *, int);
extern int D_00264FB0;
extern int D_00821010;

void func_002032C0(void) {
    int buf[8];
    int *src;
    int *dst;
    int i;

    i = 0;
    src = &D_00264FB0;
    dst = &D_00821010;
    do {
        func_00113280(0);
        while (func_00111C28(buf, *src) == 0) {
            func_00113280(0);
        }
        dst[0] = buf[0];
        dst[1] = buf[1];
        i += 1;
        src += 1;
        dst += 2;
    } while (i < 9);
}
