// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// VBLANK interrupt handler (installed by gs_readback_queue_run through
// func_00101548; taken only by address). Moved here unchanged from
// gs_readback_queue_run.c when the symbol split was fixed (2026-10-27): waits for
// interrupts to be disabled, bumps the vblank counters, latches the field bit
// from the GS CSR, calls func_0010C710 and re-enables interrupts.
//

extern volatile short D_00810E88;
extern volatile int D_00810E90;
extern volatile int D_00810E98;
extern volatile int D_00282184;
extern void func_0010C710(int);

void func_001AB140(void) {
    asm {
        lui $v1, 1
    wait_di:
        nop
        di
        sync.p
        mfc0 $v0, $12
        and $v0, $v0, $v1
        bnez $v0, wait_di
    }
    D_00810E98++;
    D_00810E90++;
    D_00810E88 = (short)((*(volatile unsigned long *)0x12001000 >> 13) & 1);
    func_0010C710(D_00282184);
    asm { sync; ei; }
}
