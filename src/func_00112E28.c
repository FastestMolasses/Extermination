// NEARMISS func_00112E28  (vram 0x00112E28, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 51.49% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Structurally identical sibling of func_00111F18 (semaphore re-init, different data symbols D_00241D0C/D_00241CFC/D_0027AF60/sema-id 0x80000593). Same two confirmed-s84 walls: eegcc forward-branch-likely wall (bgezl on func_0010E6F8(...) >= 0) and the 4-nop inline-asm delay-loop idiom unreproducible from plain C. Sam...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D0C;
extern int D_00241CF8;
extern int D_00241CFC;
extern int D_00241D2C;
extern int D_00279B00;
extern int D_00279B08;
extern int D_0027AF60;
extern char D_0026BCC0;
extern char D_0026BCE8;

extern void func_001118B8(void);
extern int iSignalSema(int sema);
extern void func_00122B58(const char *fmt, int a1, int a2);
extern void iReleaseWaitThread(int a, int b);
extern int func_00112DC0(int a0);
extern int CreateSema(int param);
extern void func_0010E088(void);
extern int func_0010E6F8(void *a0, unsigned int a1, int a2);

int func_00112E28(int a0) {
    int v0;
    int *p;

    func_001118B8();
    v0 = iSignalSema(D_00241D0C);
    if (D_00241D0C != v0) {
        if (D_00241CF8 > 0) {
            func_00122B58((const char *)&D_0026BCC0, D_00241CFC, a0);
        }
        return 0;
    }
    D_00241CFC = a0;
    iReleaseWaitThread(D_00279B00, D_00279B08);
    if (func_00112DC0(1) != 0) {
        CreateSema(D_00241D0C);
        return 0;
    }
    func_0010E088();
    if (D_00241D2C >= 0) {
        return 1;
    }
    p = &D_0027AF60;
    for (;;) {
        v0 = func_0010E6F8(p, 0x80000593, 0);
        if (v0 >= 0) {
            break;
        }
        if (D_00241CF8 > 0) {
            func_00122B58((const char *)&D_0026BCE8, 0, 0);
        }
        { int d = 0x100000; do { d--; } while (d != -1); }
    }
    if (*((int *)p + 9) == 0) {
        D_00241D2C = 0;
    }
    return 1;
}
