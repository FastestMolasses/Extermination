// NEARMISS func_00112150  (vram 0x00112150, 0x2F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.85% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler + switch-tree + GPR-coloring wall (0x2EC vs 0x2F0, one insn short). Logic/structure fully recovered; three residuals. (1) switch decision tree: the original emits `beq m,1 / slti m,2 -> default / beql m,2` (7 insns, the 0x800 default set in the bnez delay slot); ee-gcc emits ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241CF8;
extern int D_00241D08;
extern int D_00241D14;
extern int D_00241D48;
extern int D_00279E00;
extern unsigned int D_00279F80[];
extern int D_0027A2C0;
extern char D_0026BC00[];
extern char D_0026BC18[];
extern char D_0026BC30[];
extern char D_0026BC60[];
extern char D_0026BC78[];

extern int func_00112088(void);
extern int func_00111F18(int a0);
extern void func_00122B58(const char *fmt, ...);
extern void func_0010DFD8(void *a0, int a1);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, void *a7, void *a8);
extern int CreateSema(int sema);
extern void func_001115D0(void *arg);

int func_00112150(unsigned int *list, unsigned char *mode)
{
    int i;
    int n;
    int blk;
    int m;
    unsigned int v;

    if (func_00112088() == 6)
        return 0;

    if (func_00111F18(3) == 0)
        return 0;

    if (D_00241CF8 > 0)
        func_00122B58(D_0026BC00);

    i = 0;
    v = *list;
    if (v != 0xFFFFFFFFu) {
    copy_next:
        D_00279F80[i] = v;
        i++;
        if (i < 0xC0) {
            list++;
            v = *list;
            if (v != 0xFFFFFFFFu)
                goto copy_next;
        }
    }

    n = (i / 3) * 3;
    D_00279F80[n] = 0xFFFFFFFFu;
    D_00279F80[n + 1] = 0xFFFFFFFFu;
    D_00279F80[n + 2] = 0xFFFFFFFFu;

    ((char *)D_00279F80)[0x30C] = mode[0];
    ((char *)D_00279F80)[0x30D] = mode[1];
    ((char *)D_00279F80)[0x30E] = mode[2];
    *(int **)((char *)D_00279F80 + 0x310) = &D_0027A2C0;

    m = mode[2];
    switch (m) {
    case 1:
        blk = 0x918;
        break;
    case 2:
        blk = 0x924;
        break;
    default:
        blk = 0x800;
        break;
    }

    D_0027A2C0 = 0;
    if (D_00241CF8 > 0)
        func_00122B58(D_0026BC18);

    for (i = 1; i < n; i += 3) {
        v = D_00279F80[i + 1];
        if ((v & 1) == 0) {
            if (D_00241CF8 > 0)
                func_00122B58(D_0026BC30, v, D_00279F80[i] * blk);
            func_0010DFD8((void *)D_00279F80[i + 1], D_00279F80[i] * blk);
        }
    }

    func_0010DFD8(D_00279F80, 0x18);
    func_0010DFD8(&D_0027A2C0, 4);

    if (D_00241CF8 > 0)
        func_00122B58(D_0026BC60);

    D_00241D48 = 1;
    D_00241D14 = 1;
    if (func_0010E8A8(&D_00279E00, 0xF, 1, D_00279F80, 0x314, 0, 0, func_001115D0,
                      &D_00241D48) < 0) {
        D_00241D48 = 0;
        D_00241D14 = 0;
        CreateSema(D_00241D08);
        return 0;
    }

    if (D_00241CF8 > 0)
        func_00122B58(D_0026BC78);
    CreateSema(D_00241D08);
    return 1;
}
