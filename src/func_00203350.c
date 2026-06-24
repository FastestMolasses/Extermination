// COMPILER: mwcc233
// CFLAGS: -O4,p
//
// Game-loop/scene driver. Inits subsystems (func_00113280(0), func_001FBC50,
// func_001F9B20), spins on func_001192D0()==1, then twice func_001009C8(0).
// Sets up a buffer D_007A55C0 via func_00205050(buf,0x200,0xE0,0x280,0xF0)
// [note the 5th arg 0xF0 -> $t0/$a4] and func_00205740(buf,0x80000000).
// Polls func_00113C38(0x50,5,D_00275B50)==0 then func_00203460()!=1, tears down
// (func_00205740, func_001CCCC0, func_00200970(0), 2x func_001009C8(0),
// func_001F9BF0), and clears the byte flag D_00821058.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the 991202 residual was the
// idiom-13 clean-store delay-slot nop; 2.3.3 matches byte-identical.
// DUAL ADDRESSING is load-bearing: D_00275B50 is gp-rel (small int), while
// D_007A55C0 and D_00821058 are absolute (lui/%hi). To get both right under one
// flag, DROP -sdatathreshold 0 (use the default threshold so the small int goes
// gp-rel) and declare the two buffers as LARGE arrays ([0x4000]) so they exceed
// the threshold and stay absolute. objdiff 100% vs build/expected/func_00203350.o.
extern void func_001009C8(int);
extern void func_00113280(int);
extern int func_00113C38(int, int, int);
extern int func_001192D0(void);
extern void func_001CCCC0(void);
extern void func_001F9B20(void);
extern void func_001F9BF0(void);
extern void func_001FBC50(void);
extern void func_00200970(int);
extern int func_00203460(void);
extern void func_00205050(void *, int, int, int, int);
extern void func_00205740(void *, int);
extern int D_00275B50;
extern char D_007A55C0[0x4000];
extern signed char D_00821058[0x4000];

void func_00203350(void) {
    func_00113280(0);
    func_001FBC50();
    func_001F9B20();
    do {
    } while (func_001192D0() == 1);
    func_001009C8(0);
    func_001009C8(0);
    func_00205050(D_007A55C0, 0x200, 0xE0, 0x280, 0xF0);
    func_00205740(D_007A55C0, 0x80000000);
    do {
    } while (func_00113C38(0x50, 5, D_00275B50) == 0);
    do {
    } while (func_00203460() != 1);
    func_00205740(D_007A55C0, 0x80000000);
    func_001CCCC0();
    func_00200970(0);
    func_001009C8(0);
    func_001009C8(0);
    func_001F9BF0();
    D_00821058[0] = 0;
}
