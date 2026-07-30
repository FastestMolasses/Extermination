// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Movie/streaming per-frame pump (companion to func_00206170).  Latches the
// GS CSR FIELD bit (0x12001000 >> 13) into D_00275C88, and while the stream
// gate D_00275C80 is set: bump the frame counter, kick the DMA submit, and
// on success advance the ring buffer returned by func_00205F90 -- on the
// even field, state 2 -> 1 (submit the +0x1CB40 half); on the odd field,
// state 1 -> 0 (submit the +0x40 half) and raise D_00275C84.  Always ends
// with sync + ei and returns 0.
//
// The volatile qualifiers are load-bearing: `D_00275C7C++` on a volatile
// reproduces the target's dead-then-live double load, and a volatile
// D_00275C80 keeps mwcc from sinking the D_00275C88 store into the beqz
// delay slot.  991202 fills that slot anyway (92.36%); 2.3.3 is 100.0.
extern volatile int D_00275C7C;
extern volatile int D_00275C80;
extern int D_00275C84;
extern volatile int D_00275C88;
extern int D_00275C8C;
extern char D_007A55A0[8];
extern char D_007A55C0[8];
extern int D_007A55F8[2];
extern int dma_wait_and_submit(int, int);
extern char *func_00205F90(char *);
extern void func_00205700(char *, int);
extern void func_00205F50(char *);

int func_00206030(void) {
    char *p;

    D_00275C88 = (int)((*(volatile unsigned long *)0x12001000 >> 13) & 1);
    if (D_00275C80) {
        D_00275C7C++;
        D_00275C8C = dma_wait_and_submit(1, 0);
        if (D_00275C8C == 0) {
            p = func_00205F90(D_007A55A0);
            if (p == 0) {
                D_007A55F8[0] = D_007A55F8[0] + 1;
                asm { sync; ei; }
                return 0;
            }
            if (D_00275C88 == 0 && *(int *)p == 2) {
                func_00205700(D_007A55C0, 0);
                func_00205F50(p + 0x1CB40);
                *(int *)p = 1;
            } else if (D_00275C88 != 0 && *(int *)p == 1) {
                func_00205700(D_007A55C0, 1);
                func_00205F50(p + 0x40);
                *(int *)p = 0;
                D_00275C84 = 1;
            }
        }
    }
    asm { sync; ei; }
    return 0;
}
