// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Actor teardown/transition step. If the actor has a live linked record at
// +0x20 (arg0), mark that record's state byte (+0x04) = 3 (freed/pending) and
// clear the link (+0x20 = 0). If the per-context flag at arg1+0x83 is set,
// clear it, run the area/context reset func_0021C040(&D_008102B0, arg0), and
// if the global mode byte D_008106BD == 1 reset it to 0. Finally tick the
// actor: func_001B1190(actor->byte+0x9A) and func_001AFC10(actor).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 is wall #13 (it fills the `bne`/store delay slot and
// reorders `sb zero,0x83(a1)` where CW leaves the nop). The 2.3.3 build leaves
// that nop, so this readable C is byte-identical. Verified objdiff 100.0% vs
// build/expected/func_0013B9A0.o.
extern void func_0021C040(char *a, char *b);
extern void func_001B1190(int x);
extern void func_001AFC10(char *p);
extern char D_008102B0;
extern unsigned char D_008106BD;

void func_0013B9A0(char *arg0, char *arg1) {
    int *v1;

    v1 = *(int **)(arg0 + 0x20);
    if (v1) {
        *(char *)((char *)v1 + 4) = 3;
        *(int *)(arg0 + 0x20) = 0;
    }
    if (*(char *)(arg1 + 0x83)) {
        *(char *)(arg1 + 0x83) = 0;
        func_0021C040(&D_008102B0, arg0);
        if (D_008106BD == 1) {
            D_008106BD = 0;
        }
    }
    func_001B1190(*(unsigned char *)(arg0 + 0x9A));
    func_001AFC10(arg0);
}
