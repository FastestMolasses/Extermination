// CFLAGS: -O4,p -sdatathreshold 0
extern int D_007A55FC;
extern int D_002DF7F0;
extern int D_002DF7F4;
extern char D_002DF740[];
extern char D_002DF800[];

extern void TerminateThread(int a0);
extern void DeleteThread(int a0);
extern void RemoveDmacHandler(int a0, int a1);
extern void RemoveIntcHandler(int a0, int a1);
extern void func_0010C360(int a0);
extern void func_0010C290(int a0);
extern void func_0010C2F8(int a0);
extern void func_00206B30(void *a0);
extern void func_002063A0(void *a0);
extern int func_00113CD0(void);

void func_00203C90(void) {
    int r;
    TerminateThread(D_007A55FC);
    DeleteThread(D_007A55FC);
    func_0010C360(2);
    RemoveDmacHandler(2, D_002DF7F0);
    func_0010C290(2);
    RemoveIntcHandler(2, D_002DF7F4);
    func_0010C2F8(2);
    func_00206B30(D_002DF740);
    func_002063A0(D_002DF800);
    do {
        r = func_00113CD0();
        asm { nop; nop; nop; nop; nop }
    } while (r == 0);
}
