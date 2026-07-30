// NEARMISS func_00113280  (vram 0x00113280, 0x1F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.77% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc callee-saved GPR coloring permutation (confirmed s84 fast-park class). Instruction count, control flow, scheduling and every delay slot match one-for-one; .text size 0x1F8 == expected. Expected colors D_00241CF8->s6, mode->s2, D_00241D30->s1, D_0027AFB0->s5, D_0027B060->s4, D_0027AB40->s7;...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// libcdvd: sceCdInit(mode) — binds the cdvd "S" RPC server (sid 0x8000059A),
// retrying with a busy-wait, then issues fno 0 with the init mode.

extern int D_00241CF8;
extern volatile int D_00241D0C;
extern int D_00241D30;
extern char D_0026BD30[];
extern char D_0026BD40[];
extern char D_0026BD60[];
extern unsigned char D_0027AFB0[];
extern unsigned char D_0027AB40[];
extern int D_0027B060;

extern void func_00122B58(char *fmt);
extern void func_001118B8(void);
extern int iSignalSema(int semid);
extern void CreateSema(int semid);
extern int func_00112DC0(int arg);
extern void func_0010E088(int arg);
extern int func_0010E6F8(void *bd, unsigned int sid, unsigned int mode);
extern void func_0010DFD8(void *addr, int size);
extern int func_0010E8A8(void *bd, int fno, int mode, void *send, int ssize,
                         void *recv, int rsize, void *end_func, void *end_para);

int func_00113280(int mode)
{
    int i;
    int r;

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BD30);
    }
    func_001118B8();
    r = iSignalSema(D_00241D0C);
    if (D_00241D0C != r) {
        return 6;
    }

    if (func_00112DC0(1) != 0) {
        goto error;
    }
    func_0010E088(0);

    if (D_00241D30 < 0) {
        for (;;) {
            if (func_0010E6F8(D_0027AFB0, 0x8000059A, 0) < 0) {
                if (D_00241CF8 > 0) {
                    func_00122B58(D_0026BD40);
                }
                i = 0x100000;
                do {
                    i--;
                    __asm__ __volatile__("nop");
                } while (i != -1);
                continue;
            }
            if (*(int *)(D_0027AFB0 + 0x24) != 0) {
                break;
            }
            i = 0x100000;
            do {
                i--;
                __asm__ __volatile__("nop");
            } while (i != -1);
        }
        D_00241D30 = 0;
    }

    D_0027B060 = mode;
    func_0010DFD8(&D_0027B060, 4);
    if (func_0010E8A8(D_0027AFB0, 0, 0, &D_0027B060, 4, D_0027AB40, 4, 0, 0) < 0) {
error:
        CreateSema(D_00241D0C);
        return (mode != 8) ? 6 : -1;
    }

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BD60);
    }
    CreateSema(D_00241D0C);
    return *(volatile int *)((unsigned int)D_0027AB40 | 0x20000000);
}
