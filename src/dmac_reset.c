// COMPILER: eegcc
// EEGCC_AS: 2.96
// (2.96 gas: it fills call/return delay slots as the original does)
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.27% NEARMISS).
// Fixes read from the original instructions:
//  - The per-channel register block is hardware: the channel base table holds
//    pointers to volatile ints, so the six clearing stores keep source order
//    (a plain pointer let the scheduler hoist the last store to the front).
//  - Plain indexed loop over the two tables (ee-gcc strength-reduces it into the
//    pointer walk with a 9..0 counter itself); the old hand-written pointer walk
//    gave the wrong register colouring.
//  - The callee prototypes follow their definitions: func_00101B80 (byte clear)
//    returns void and takes a byte pointer; func_00101CD0 takes a byte pointer.
//  - Compound volatile read-modify-writes on the DMAC control registers.
//
// Resets the DMA controller: remembers whether DMAC_CTRL's enable bit was set,
// clears CHCR/MADR/TADR/ASR0/ASR1/SADR of every channel present in
// D_00241090, writes the STAT clear mask and keeps only the STAT mask bits,
// builds a zeroed 20-byte setup block and applies it via func_00101CD0, then
// re-enables the DMAC if arg0 == 1. Returns the previous enable bit.

extern void func_00101B80(unsigned char *, int);
extern int func_00101CD0(unsigned char *);
extern int D_00241090[];
extern volatile int *D_00241050[];

int dmac_reset(int arg0) {
    unsigned char buf[32];
    int ret;
    int i;

    ret = *(volatile int *)0x1000E000 & 1;
    for (i = 0; i < 10; i++) {
        if (D_00241090[i] != 0) {
            volatile int *e = D_00241050[i];
            e[0x80 / 4] = 0;
            e[0] = 0;
            e[0x30 / 4] = 0;
            e[0x10 / 4] = 0;
            e[0x50 / 4] = 0;
            e[0x40 / 4] = 0;
        }
    }
    *(volatile int *)0x1000E010 = 0xFF1F;
    *(volatile int *)0x1000E010 &= 0xFF1F0000;
    func_00101B80(buf, 20);
    func_00101CD0(buf);
    if (arg0 == 1) {
        *(volatile int *)0x1000E000 |= 1;
    }
    return ret;
}
