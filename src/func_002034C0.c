// NEARMISS func_002034C0  (vram 0x002034C0, 0x220 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling permutation (body/logic 100% correct): (1) the two func_00108DB0 calls materialize their a3 function-pointer arg before a0 in the target, mwcc emits a0 first; (2) the target sinks the D_007A5590=0 store into the _iSetAlarm jal delay slot, mwcc hoists it above the D_007A557x...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Boot/system init for a subsystem. Sets two EE hardware registers at
// 0x1000E000 (D_CTRL: OR 3) / 0x1000E010, runs func_002039A0 and func_001095F0,
// then registers two big memory pools via func_00206A00 / func_00206210, hooks
// two callbacks (func_00203D30, func_00203E60) through func_00108DB0, and stages
// an _iSetAlarm parameter block at D_007A557x: handler func_00206D10, buffer
// D_0079E540, size 0x4000, priority 5, gp base &D_0027D370, count 0. Arms the
// alarm, CreateThreads the returned id (stashed in D_007A55FC), then spins on
// func_00113C68 over the per-slot table at D_00821010 (8-byte stride, indexed by
// the gp-rel state byte D_00275C78) until it returns nonzero. Finally swaps the
// INTC handler at channel 2 (Add then Remove func_00206030/func_00206170, with
// func_0010C2F8 / func_0010C3C8 around it) and returns the table's +4 field
// (D_00821014) for the current slot.
//
// NEARMISS 96.1% (mwcc 2.3.3; 991202 = 84.8%). Body/logic fully recovered.
// -sdatathreshold 8 makes the gp-base scalars D_0027D370 / D_00275C78 gp-relative
// (CW's addiu/lbu gp-rel form); the far .data globals are declared as incomplete
// arrays so mwcc keeps them in absolute lui/%lo. The two residuals are pure
// instruction-scheduling permutations: (1) the func_00108DB0 calls materialize
// their a3 (function-pointer) arg before a0 in the target, mwcc does a0 first;
// (2) the target sinks the D_007A5590 = 0 store into the _iSetAlarm jal delay
// slot while mwcc hoists it above the D_007A557x staging block. Neither is the
// clean-store nop, so 2.3.3 does not close them -- permuter/scheduling territory.
extern int func_002039A0(void *p, int a);
extern void func_001095F0(void);
extern int func_00206A00(void *a, void *b, int c, void *d, void *e, int f, void *g, int h);
extern int func_00206210(void *a, void *b, int c, int d);
extern int func_00108DB0(void *a, int b, int c, void *d, void *e);
extern int func_00203B20(void *a, int b, void *c, int d);
extern int _iSetAlarm(void *p);
extern int CreateThread(int a, void *b);
extern int func_00113C68(int a, char *p);
extern int AddIntcHandler(int chan, void *handler, int arg);
extern void func_0010C2F8(int chan);
extern int RemoveIntcHandler(int chan, void *handler, int arg);
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
extern int D_007A5570[];
extern void *D_007A5574[];
extern void *D_007A5578[];
extern int D_007A557C[];
extern void *D_007A5580[];
extern int D_007A5584[];
extern int D_007A5590[];
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
    func_002039A0(D_0028F700, v);
    func_001095F0();
    func_00206A00(D_002DF740, D_005ABD40, 0x152000, D_0071E540, D_005AAD00, 0x100, D_007A2540, 0x200);
    func_00206210(D_002DF800, D_006FE540, 0x20000, 0x6000);
    func_00108DB0(D_002DF740, 0, 0, func_00203D30, D_0028F700);
    func_00108DB0(D_002DF740, 2, 0, func_00203E60, D_0028F700);
    func_00203B20(D_007A55A0, ((int)D_002E0080 & 0x0FFFFFFF) | 0x20000000, D_00538080, 2);
    D_007A5574[0] = func_00206D10;
    D_007A5578[0] = D_0079E540;
    D_007A557C[0] = 0x4000;
    D_007A5584[0] = 5;
    D_007A5580[0] = &D_0027D370;
    D_007A5590[0] = 0;
    D_007A55FC[0] = _iSetAlarm(D_007A5570);
    CreateThread(D_007A55FC[0], D_002DF740);
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    do {
    } while (func_00113C68(D_00821010[D_00275C78 * 2], buf) == 0);
    D_002DF7F4[0] = AddIntcHandler(2, func_00206030, 0);
    func_0010C2F8(2);
    D_002DF7F0[0] = RemoveIntcHandler(2, func_00206170, 0);
    func_0010C3C8(2);
    return D_00821014[D_00275C78 * 2];
}
