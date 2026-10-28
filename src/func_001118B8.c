// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.70% NEARMISS).
// Fixes read from the original instructions:
//  - the four subsystem state words are volatile (shared with other threads /
//    callers), which keeps each id store right after its CreateSema call and
//    frees the delay slots the original fills;
//  - init_count is set before max_count;
//  - assembled with the 2.96 gas, which fills the call delay slots like the
//    original.

// Lazily create the three global semaphores used by this subsystem.
// CreateSema (0x0010B820, EE syscall 64) takes an ee_sema_t * and returns
// the semaphore id.
typedef struct {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    int attr;
    int option;
} ee_sema_t;

extern volatile int D_00241D08;
extern volatile int D_00241D0C;
extern volatile int D_00241D10;
extern volatile int D_00241D14;

extern int CreateSema(ee_sema_t *param);

void func_001118B8(void) {
    ee_sema_t sema;

    if (D_00241D08 != -1 && D_00241D0C != -1) {
        return;
    }
    sema.option = 0;
    sema.init_count = 1;
    sema.max_count = 1;
    D_00241D08 = CreateSema(&sema);
    D_00241D0C = CreateSema(&sema);
    sema.init_count = 0;
    D_00241D10 = CreateSema(&sema);
    D_00241D14 = 0;
}
