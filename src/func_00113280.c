// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.93% NEARMISS).
// Fixes read from the original instructions:
//  - The "GPR coloring" residual was ee-gcc's allocation priority (references
//    weighted by loop depth over live length), so the source shape decides it:
//  - the bind-done latch reset (D_00241D30 = 0) sits INSIDE the retry loop,
//    just before the break, not after the loop;
//  - the func_00112DC0 failure path is its own SignalSema + return block (the
//    compiler cross-jumps it into the shared error tail), not a goto into the
//    RPC-failure branch.
//  - The delay loops need no inline nop: the r5900 assembler pads short loops.

// libcdvd: sceCdInit(mode) - binds the cdvd "S" RPC server (sid 0x8000059A),
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
extern int PollSema(int semid);
extern void SignalSema(int semid);
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
    r = PollSema(D_00241D0C);
    if (D_00241D0C != r) {
        return 6;
    }

    if (func_00112DC0(1)) {
        SignalSema(D_00241D0C);
        return (mode != 8) ? 6 : -1;
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
                } while (i != -1);
                continue;
            }
            if (*(int *)(D_0027AFB0 + 0x24) != 0) {
                D_00241D30 = 0;
                break;
            }
            i = 0x100000;
            do {
                i--;
            } while (i != -1);
        }
    }

    D_0027B060 = mode;
    func_0010DFD8(&D_0027B060, 4);
    if (func_0010E8A8(D_0027AFB0, 0, 0, &D_0027B060, 4, D_0027AB40, 4, 0, 0) < 0) {
        SignalSema(D_00241D0C);
        return (mode != 8) ? 6 : -1;
    }

    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BD60);
    }
    SignalSema(D_00241D0C);
    return *(volatile int *)((unsigned int)D_0027AB40 | 0x20000000);
}
