// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Subsystem reset: zeros a run of hardware/scratch regs at 0x70003B8x-9x and
// the 0x70003258 word, memsets the 0x48-byte table D_008106B0 to 0, then
// calls func_001FC9B0. KEY STEERING (s84): place the 0x70003258 store BEFORE
// the func_00121A28 call -- mwcc then schedules it into the jal delay slot and
// emits `sb 0x3B92` adjacent to the call setup, with `lui v0,%hi(D_008106B0)`
// interleaved before `sb 0x3B92`, matching CW exactly. This removed the need
// for the old asm-void .word form. 991202 stalls at 90.77%; mwcc 2.3.3
// byte-matches.
extern char D_008106B0[0x80];
extern void func_00121A28(char *, int, int);
extern void func_001FC9B0(void);

void func_001AFCF0(void) {
    *(volatile short *)0x70003B84 = 0;
    *(volatile char *)0x70003B93 = 0;
    *(volatile char *)0x70003B8C = 0;
    *(volatile char *)0x70003B8D = 0;
    *(volatile char *)0x70003B8E = 0;
    *(volatile char *)0x70003B8F = 0;
    *(volatile char *)0x70003B91 = 0;
    *(volatile char *)0x70003B92 = 0;
    *(volatile int *)0x70003258 = 0;
    func_00121A28(D_008106B0, 0, 0x48);
    func_001FC9B0();
}
