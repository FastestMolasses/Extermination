// NEARMISS func_00111C28  (vram 0x00111C28, 0x2EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.26% via ee-gcc 2.9-991111-01 (-O2 -fno-strength-reduce). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall (+2-insn address-form residual). Instruction count is EXACT (0x2EC == 0x2EC) and every instruction matches modulo register numbering. Expected allocates hi(D_00241D20)->s1 and shifts everything else up (s2=arg1, s3=arg0, s4=hi CF8, s5=hi C80, s6=hi D08, s7=hi E28); ee-gcc ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2 -fno-strength-reduce

extern volatile int D_00241D08;
extern int D_00241CF8;
extern int D_00241D00;
extern int D_00241D20;
extern int D_00279B00;
extern int D_00279B08;
extern char D_00279C80[];
extern unsigned char D_00279DC0[];
extern unsigned char D_00279E28[];
extern char D_0026BB50[];
extern char D_0026BB70[];
extern char D_0026BB88[];
extern char D_0026BB98[];
extern char D_0026BBA8[];

extern void func_001118B8(void);
extern int iSignalSema(int sema);
extern void iReleaseWaitThread(int a, void *b);
extern int func_00112D18(int a0);
extern void func_0010E088(int a0);
extern int func_0010E6F8(void *a0, unsigned int a1, int a2);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, void *a7, void *a8);
extern void func_0010DFD8(void *a0, int a1);
extern void func_00122B58(const char *fmt, ...);
extern int CreateSema(int sema);
extern void *memcpy(void *dst, const void *src, unsigned int n);

int func_00111C28(char *out, const char *name)
{
    unsigned char *cd;
    char *req;
    int i;
    int d;
    int ret;

    func_001118B8();
    if (D_00241D08 != iSignalSema(D_00241D08)) {
        return 0;
    }

    D_00241D00 = 1;
    iReleaseWaitThread(D_00279B00, &D_00279B08);
    if (func_00112D18(1) != 0) {
        CreateSema(D_00241D08);
        return 0;
    }

    func_0010E088(0);

    if (D_00241D20 < 0) {
        for (;;) {
            cd = D_00279E28;
            if (func_0010E6F8(cd, 0x80000597u, 0) < 0) {
                if (D_00241CF8 > 0)
                    func_00122B58(D_0026BB50);
                d = 0x100000;
                do {
                    d--;
                    __asm__("nop");
                    __asm__("nop");
                    __asm__("nop");
                    __asm__("nop");
                } while (d != -1);
                continue;
            }
            if (*(int *)(cd + 0x24) != 0)
                break;
            d = 0x100000;
            do {
                d--;
                __asm__("nop");
                __asm__("nop");
                __asm__("nop");
                __asm__("nop");
            } while (d != -1);
        }
        D_00241D20 = 0;
    }

    i = 0;
    while (i < 0x100) {
        if ((D_00279C80[0x20 + i] = name[i]) == 0)
            break;
        i++;
    }
    if (i == 0x100)
        D_00279C80[0x11F] = 0;

    req = D_00279C80;
    *(char **)(req + 0x120) = req;

    if (D_00241CF8 > 0)
        func_00122B58(D_0026BB70, req + 0x20);

    func_0010DFD8(req, 0x124);

    if (func_0010E8A8(D_00279E28, 0, 0, req, 0x124, D_00279DC0, 4, 0, 0) < 0) {
        CreateSema(D_00241D08);
        return 0;
    }

    memcpy(out, (char *)((unsigned int)req | 0x20000000), 32);

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BB88, out + 8);
        if (D_00241CF8 > 0) {
            func_00122B58(D_0026BB98, *(int *)(out + 4));
            if (D_00241CF8 > 0)
                func_00122B58(D_0026BBA8, *(int *)out);
        }
    }

    ret = *(int *)((unsigned int)D_00279DC0 | 0x20000000);
    CreateSema(D_00241D08);
    return ret;
}
