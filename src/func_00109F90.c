// COMPILER: eegcc
// CFLAGS: -Os -fno-schedule-insns
// Tiny SDK dispatch shim: when the mode word D_002414AC has reached state 3 run
// func_00109E68, otherwise run func_00109FD0. Neither call is a tail call in the
// original (the frame is set up and $ra saved/restored around both), so the
// trailing `asm volatile("")` barriers are needed to stop ee-gcc turning them
// into `j` sibling calls.
//
// CFLAGS note: `-Os -fno-schedule-insns` is load-bearing, not cosmetic. At the
// usual -O2 ee-gcc colours the compare constant into $a0 (`li a0,3`); the
// original reuses the now-dead %hi address register, `addiu $v0,$zero,3`. -Os
// alone colours the load into $v0 instead, and -O2 -fno-schedule-insns colours
// the load into $a0 — only the pair reproduces the original's
// lui $v0 / lw $v1 / addiu $v0,$zero,3 assignment.
extern int D_002414AC;

extern void func_00109FD0(void);
extern void func_00109E68(void);

void func_00109F90(void)
{
    if (D_002414AC != 3) {
        func_00109FD0();
        asm volatile("");
        return;
    }
    func_00109E68();
    asm volatile("");
}
