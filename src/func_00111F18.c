// NEARMISS func_00111F18  (vram 0x00111F18, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 51.49% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Two confirmed-s84 ee-gcc walls. (1) eegcc forward-branch-likely wall: expected emits bgezl $v0 (annulling) on the func_0010E6F8(...) >= 0 syscall-success check; our ee-gcc 2.9-991111-01 emits non-annulling bgez/bltz (only emits branch-likely on loop back-edges). (2) 4-nop inline-asm delay-loop idiom: the SDK busy-wa...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D08;
extern int D_00241CF8;
extern int D_00241D00;
extern int D_00241D24;
extern int D_00279B00;
extern int D_00279B08;
extern int D_00279E00;
extern int D_FFFFF;
extern char D_0026BBC0;
extern char D_0026BBE8;

extern void func_001118B8(void);
extern int iSignalSema(int sema);
extern void func_00122B58(const char *fmt, int a1, int a2);
extern void iReleaseWaitThread(int a, int b);
extern int func_00112D18(int a0);
extern int CreateSema(int param);
extern void func_0010E088(void);
extern int func_0010E6F8(void *a0, unsigned int a1, int a2);

int func_00111F18(int a0) {
    int v0;
    int *p;

    func_001118B8();
    v0 = iSignalSema(D_00241D08);
    if (D_00241D08 != v0) {
        if (D_00241CF8 > 0) {
            func_00122B58((const char *)&D_0026BBC0, D_00241D00, a0);
        }
        return 0;
    }
    D_00241D00 = a0;
    iReleaseWaitThread(D_00279B00, D_00279B08);
    if (func_00112D18(1) != 0) {
        CreateSema(D_00241D08);
        return 0;
    }
    func_0010E088();
    if (D_00241D24 >= 0) {
        return 1;
    }
    p = &D_00279E00;
    for (;;) {
        v0 = func_0010E6F8(p, 0x80000595, 0);
        if (v0 >= 0) {
            break;
        }
        if (D_00241CF8 > 0) {
            func_00122B58((const char *)&D_0026BBE8, 0, 0);
        }
        { int d = 0x100000; do { d--; } while (d != -1); }
    }
    if (*((int *)p + 9) == 0) {
        D_00241D24 = 0;
    }
    return 1;
}
