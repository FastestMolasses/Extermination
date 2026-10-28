// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// SDK (SIF RPC): the RPC server loop (sceSifRpcLoop shape). Forever: take
// each pending request from the queue with func_0010ECC8 and execute it with
// func_0010ED18; when the queue is empty, SleepThread until woken. Never returns.

extern int func_0010ECC8(int a0);
extern void func_0010ED18(int a0);
extern int SleepThread(void);

void func_0010EEF8(int a0) {
    int r;
    while (1) {
        while ((r = func_0010ECC8(a0)) != 0) {
            func_0010ED18(r);
        }
        SleepThread();
    }
}
