// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 51.49% NEARMISS).
// Fixes read from the original instructions:
//  - The RPC bind loop was wrong: a failed bind prints and delays, then retries;
//    a successful bind with a zero server word delays and retries too; only a
//    nonzero server word clears the bind latch (D_00241D24 = 0, inside the loop
//    just before the break). Same shape as the byte-matched func_00113280 and func_00112E28.
//  - The first debug print passes the argument first and D_00241D00 second.
//  - ReferThreadStatus takes the status buffer's address; func_0010E088 takes 0.
//  - The semaphore word is volatile (it is re-read after PollSema).
//  - Not walls: ee-gcc emits the success test as branch-likely itself, and the
//    delay loops need no inline nop (the r5900 assembler pads short loops).

// SIF RPC client init (sibling of func_00112E28): takes the module semaphore
// with PollSema (prints and returns 0 if it is busy), records the argument,
// refreshes the thread status, then binds RPC server 0x80000595 once, retrying
// with a busy-wait, and returns 1.

extern volatile int D_00241D08;
extern int D_00241CF8;
extern int D_00241D00;
extern int D_00241D24;
extern int D_00279B00;
extern int D_00279B08[];
extern unsigned char D_00279E00[];
extern char D_0026BBC0[];
extern char D_0026BBE8[];

extern void func_001118B8(void);
extern int PollSema(int semid);
extern void func_00122B58(char *fmt, ...);
extern void ReferThreadStatus(int tid, void *info);
extern int func_00112D18(int arg);
extern void SignalSema(int semid);
extern void func_0010E088(int arg);
extern int func_0010E6F8(void *bd, unsigned int sid, unsigned int mode);

int func_00111F18(int a0)
{
    int i;
    int r;

    func_001118B8();
    r = PollSema(D_00241D08);
    if (D_00241D08 != r) {
        if (D_00241CF8 > 0) {
            func_00122B58(D_0026BBC0, a0, D_00241D00);
        }
        return 0;
    }
    D_00241D00 = a0;
    ReferThreadStatus(D_00279B00, D_00279B08);
    if (func_00112D18(1)) {
        SignalSema(D_00241D08);
        return 0;
    }
    func_0010E088(0);

    if (D_00241D24 < 0) {
        for (;;) {
            if (func_0010E6F8(D_00279E00, 0x80000595, 0) < 0) {
                if (D_00241CF8 > 0) {
                    func_00122B58(D_0026BBE8);
                }
                i = 0x100000;
                do {
                    i--;
                } while (i != -1);
                continue;
            }
            if (*(int *)(D_00279E00 + 0x24) != 0) {
                D_00241D24 = 0;
                break;
            }
            i = 0x100000;
            do {
                i--;
            } while (i != -1);
        }
    }
    return 1;
}
