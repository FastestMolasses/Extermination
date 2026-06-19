// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): scheduler service loop — signal sema, dispatch by state.
extern int D_00276C98;
extern int D_0026B7B0;
extern void SignalSema(int sema);
extern void iReferThreadStatus(int thid);
extern void ChangeThreadPriority(int thid);
extern void func_0010D890(int *a0);

void func_0010C5C8(int *a0) {
    for (;;) {
        SignalSema(D_00276C98);
        switch (a0[0]) {
        case 0:
            iReferThreadStatus(a0[1]);
            break;
        case 1:
            ChangeThreadPriority(a0[1]);
            break;
        default:
            func_0010D890(&D_0026B7B0);
            break;
        }
    }
}
