// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Initializes an actor/object: sets state byte +0x23F = 2 and clears the
// +0x24C word, then runs setup func_001662D0(obj). If the object's type/kind
// byte +0x04 == 1, clears the global flag at 0x70003B8D. Returns void.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the 991202 build mis-lowers the
// `bne +0x4 != 1` guard, leaving a one-instruction residual; 2.3.3 matches
// byte-identical. Verified objdiff 100% vs build/expected/func_001833F0.o.
extern void func_001662D0(char *);

void func_001833F0(char *arg0) {
    *(char *)(arg0 + 0x23F) = 2;
    *(int *)(arg0 + 0x24C) = 0;
    func_001662D0(arg0);
    if (*(unsigned char *)(arg0 + 4) == 1) {
        *(char *)0x70003B8D = 0;
    }
}
