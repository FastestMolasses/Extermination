// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (new file, split out of func_00111680).
// Fixes read from the original instructions:
//  - The old candidate's body had the wrong shape: the original keeps the result in a
//    local (1, or 0 on the "already running" path) with the create path in the if-arm
//    and the priority change in the else-arm, and returns once at the end.
//  - GetThreadId takes no arguments; all SDK callees now have full prototypes and the
//    thread block is a ThreadParam-shaped struct.
//  - The ThreadParam fields are filled in the order stack size, gp, entry, stack,
//    priority; that source order decides the saved-register assignment of the three
//    parameters. The default 2.9 gas is used (EEGCC_AS: 2.96 is not needed).
//
// SEMANTICS: SDK service thread creator.
//
// func_00111740(prio, stack, stack_size): if the service thread (id in D_00241D04)
// does not exist yet, records the caller's thread id in D_00279B00, reads its status
// into D_00279B08, fills the ThreadParam block D_00279B38 (entry func_00111680, the
// given stack and size, gp = the program's gp base, initial priority prio), creates
// and starts the thread, stores its id and returns 1. Otherwise it only changes the
// existing thread's priority to prio and returns 0.

struct ThreadParam {
    int status;
    void *func;
    void *stack;
    int stackSize;
    void *gpReg;
    int initPriority;
    int currentPriority;
    unsigned int attr;
    unsigned int option;
    int waitType;
    int waitId;
    int wakeupCount;
};

extern int D_00241D04;
extern int D_00279B00;
extern struct ThreadParam D_00279B08;
extern struct ThreadParam D_00279B38;
extern int D_0027D370;

extern int GetThreadId(void);
extern int ReferThreadStatus(int tid, struct ThreadParam *info);
extern int CreateThread(struct ThreadParam *param);
extern int StartThread(int tid, void *arg);
extern int ChangeThreadPriority(int tid, int prio);
extern void func_00111680(void);

int func_00111740(int prio, void *stack, int stack_size)
{
    int ret = 1;

    if (D_00241D04 == 0) {
        D_00279B00 = GetThreadId();
        ReferThreadStatus(D_00279B00, &D_00279B08);
        D_00279B38.stackSize = stack_size;
        D_00279B38.gpReg = &D_0027D370;
        D_00279B38.func = func_00111680;
        D_00279B38.stack = stack;
        D_00279B38.initPriority = prio;
        D_00241D04 = CreateThread(&D_00279B38);
        StartThread(D_00241D04, 0);
    } else {
        ChangeThreadPriority(D_00241D04, prio);
        ret = 0;
    }
    return ret;
}
