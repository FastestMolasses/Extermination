// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.10% NEARMISS).
// Fixes read from the original instructions:
//  - func_002039A0 takes one argument (its definition in src/); the second
//    argument passed before was spurious.
//  - func_00108DB0's 4th parameter is an int, so the two callbacks are passed
//    as (int) casts; this restores the target's argument-setup order.
//  - The block at 0x007A5570 is one ThreadParam struct global; writing every
//    field (including gp_reg at +0x10 and option=0 at +0x20) through that one
//    struct keeps the original store order, with the option store in the
//    CreateThread call's delay slot.
//
// Boot/system init for a subsystem. Sets two EE hardware registers at
// 0x1000E000 (D_CTRL: OR 3) / 0x1000E010, runs func_002039A0 (one argument) and func_001095F0,
// then registers two big memory pools via func_00206A00 / func_00206210, hooks
// two callbacks (func_00203D30, func_00203E60) through func_00108DB0, and fills
// the CreateThread ThreadParam block D_007A5570: entry func_00206D10, stack
// D_0079E540, size 0x4000, priority 5, gp base &D_0027D370, count 0. Creates
// the thread, StartThreads the returned id (stashed in D_007A55FC), then spins on
// func_00113C68 over the per-slot table at D_00821010 (8-byte stride, indexed by
// the gp-rel state byte D_00275C78) until it returns nonzero. Finally installs
// func_00206030 as the INTC cause-2 handler (AddIntcHandler, enabled via
// func_0010C2F8) and func_00206170 as the DMAC channel-2 handler (AddDmacHandler,
// enabled via func_0010C3C8) and returns the table's +4 field
// (D_00821014) for the current slot.
//
extern void func_002039A0(void *p);
extern void func_001095F0(void);
extern int func_00206A00(void *a, void *b, int c, void *d, void *e, int f, void *g, int h);
extern int func_00206210(void *a, void *b, int c, int d);
extern int func_00108DB0(void *a, int b, int c, int d, void *e);
extern int func_00203B20(void *a, int b, void *c, int d);
extern int CreateThread(void *p);
extern int StartThread(int a, void *b);
extern int func_00113C68(int a, char *p);
extern int AddIntcHandler(int chan, void *handler, int arg);
extern void func_0010C2F8(int chan);
extern int AddDmacHandler(int chan, void *handler, int arg);
extern void func_0010C3C8(int chan);

extern unsigned char D_00275C78;
extern int D_0027D370;
extern char D_0028F700[];
extern char D_002DF740[];
extern int D_002DF7F0[];
extern int D_002DF7F4[];
extern char D_002DF800[];
extern char D_002E0080[];
extern char D_00538080[];
extern char D_005AAD00[];
extern char D_005ABD40[];
extern char D_006FE540[];
extern char D_0071E540[];
extern char D_0079E540[];
extern char D_007A2540[];
typedef struct {
    int status;
    void *entry;
    void *stack;
    int stackSize;
    void *gpReg;
    int initPriority;
    int currentPriority;
    int attr;
    int option;
} ThreadParam;
extern ThreadParam D_007A5570;
extern char D_007A55A0[];
extern int D_007A55FC[];
extern int D_00821010[];
extern int D_00821014[];
extern void func_00203D30(void);
extern void func_00203E60(void);
extern void func_00206030(void);
extern void func_00206170(void);
extern void func_00206D10(void);

int func_002034C0(void) {
    char buf[3];
    int v;

    v = *(int *)0x1000E000;
    *(int *)0x1000E000 = v | 3;
    *(int *)0x1000E010 = 4;
    func_002039A0(D_0028F700);
    func_001095F0();
    func_00206A00(D_002DF740, D_005ABD40, 0x152000, D_0071E540, D_005AAD00, 0x100, D_007A2540, 0x200);
    func_00206210(D_002DF800, D_006FE540, 0x20000, 0x6000);
    func_00108DB0(D_002DF740, 0, 0, (int)func_00203D30, D_0028F700);
    func_00108DB0(D_002DF740, 2, 0, (int)func_00203E60, D_0028F700);
    func_00203B20(D_007A55A0, ((int)D_002E0080 & 0x0FFFFFFF) | 0x20000000, D_00538080, 2);
    D_007A5570.entry = func_00206D10;
    D_007A5570.stack = D_0079E540;
    D_007A5570.stackSize = 0x4000;
    D_007A5570.initPriority = 5;
    D_007A5570.gpReg = &D_0027D370;
    D_007A5570.option = 0;
    D_007A55FC[0] = CreateThread(&D_007A5570);
    StartThread(D_007A55FC[0], D_002DF740);
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    do {
    } while (func_00113C68(D_00821010[D_00275C78 * 2], buf) == 0);
    D_002DF7F4[0] = AddIntcHandler(2, func_00206030, 0);
    func_0010C2F8(2);
    D_002DF7F0[0] = AddDmacHandler(2, func_00206170, 0);
    func_0010C3C8(2);
    return D_00821014[D_00275C78 * 2];
}
