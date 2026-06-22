// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Thread-spawn SDK setup. Releases any pending wait-thread, lowers the
// dispatch priority of the returned thread to 5, fills a CreateThread/
// _iSetAlarm parameter block (func_001FB0C0 entry, D_0081FEC0 stack, 0x1000
// stack size, priority 2, gp base &D_0027D370 in arg+0x10), arms the alarm,
// then CreateThreads the resulting thread id and stashes it in D_00282184.
//
// Matched on mwcc 2.3.3 (mwcps2-2.3.3-000906): under 991202 the lone residual
// is the clean-store delay-slot wall (89.19%). 2.3.3 leaves CW's nop and is
// byte-identical. -sdatathreshold 8 makes the gp-base symbol D_0027D370
// gp-relative (CW's addiu v0, gp, %gp_rel form); the high D_0081xxxx globals
// are declared as incomplete arrays so mwcc keeps them out of small data
// (absolute lui/lo), each as its own relocation. Verified objdiff 100% vs
// build/expected/func_001F9780.o.
extern int CreateThread(int, int);
extern int DisableDispatchThread(int, int);
extern int ReleaseWaitThread(void);
extern int _iSetAlarm(void *);
extern int D_0027D370;
extern int D_00282184[];
extern int D_0081FE70[];
extern void *D_0081FE74[];
extern void *D_0081FE78[];
extern int D_0081FE7C[];
extern void *D_0081FE80[];
extern int D_0081FE84[];
extern int D_0081FEC0[];
extern int func_001FB0C0[];

void func_001F9780(void) {
    int temp_v0;

    DisableDispatchThread(ReleaseWaitThread(), 5);
    D_0081FE74[0] = func_001FB0C0;
    D_0081FE78[0] = D_0081FEC0;
    D_0081FE7C[0] = 0x1000;
    D_0081FE84[0] = 2;
    D_0081FE80[0] = &D_0027D370;
    temp_v0 = _iSetAlarm(D_0081FE70);
    CreateThread(temp_v0, 0);
    D_00282184[0] = temp_v0;
}
