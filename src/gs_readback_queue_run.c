// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was an 82.25% NEARMISS).
// The code was always byte-identical; splat gave this function and the VBLANK
// handler after it one 0x39C-byte symbol. config/symbol_addrs.txt now names the
// handler func_001AB140, so each is its own unit (src/func_001AB140.c).
//

//
// CORRECTED (decomp lane round 5, 2026-10-20): the sync wait calls
// dma_wait_and_submit(0, 0), the 0x00100A60 entry the original calls (the old C
// used its stale pre-rename alias sub_D2_TADR_08x).

// Boot/main loop: init the IOP modules and subsystems, install the VBLANK
// handler, then spin forever: run one frame of game logic, wait for the
// handler's vblank counter, flip the double-buffered display/draw contexts
// (D_00810E80 selects the buffer, D_00810E88 is the field/interlace bit).
extern volatile short D_00810E80;
extern volatile short D_00810E88;
extern volatile int D_00810E90;
extern volatile int D_00810E98;
extern char D_00810EA0[];
extern char D_00810F00[];
extern char D_00810F80[];
extern char D_00811070[];
extern char D_008110F0[];
extern unsigned char D_00821058;
extern volatile int D_00282184;

extern int sub_cdrom0_IRX_SNDN2DRV_IRX_1(void);
extern void func_001FEE60(void);
extern void func_001AB370(void);
extern void func_001CCCC0(void);
extern void func_001CCBD0(int, int, int);
extern void func_001AB430(void);
extern void func_001FB210(void);
extern void func_001F9820(void);
extern void func_001F9780(void);
extern void func_00101548(void *);
extern void func_001FF1E0(int);
extern void sub_EXTERMINATION(void);
extern void func_001CCB10(void);
extern void func_001B5790(void);
extern void func_00225CC0(void);
extern void func_001AB650(void);
extern void func_001AB740(int, void *);
extern void func_001AB7E0(void);
extern void func_001AED80(int);
extern void func_001D1AE0(int);
extern void func_001B57E0(void);
extern void func_001AEBE0(void);
extern void func_001AB6A0(void);
extern void func_001FCA10(void);
extern void func_001AEE70(void);
extern void func_001FB100(void);
extern void func_001B5B70(void);
extern int dma_wait_and_submit(int, int);
extern void func_0011B910(void);
extern void func_0011B5E0(void);
extern void func_0011B328(void);
extern void func_0011AE88(void);
extern void func_0011A9D8(void);
extern void gs_readback_queue_drain(void);
extern void func_001AB590(void);
extern void func_00203350(void);
extern void func_001D1C10(int);
extern void func_001AB4E0(int, int);
extern void func_001015A8(char *, int, int, int);
extern void func_00101810(char *, int, int, int);
extern void FlushCache(int);
extern void func_00100550(char *);
extern void func_001D2300(void);
extern void func_001D2580(int);
extern void func_0010C710(int);
extern void func_001AB140(void);

void gs_readback_queue_run(void) {
    char *p;
    char *q;
    short flip;
    short vsync;

    if (sub_cdrom0_IRX_SNDN2DRV_IRX_1() != 0) {
        for (;;) {
        }
    }
    func_001FEE60();
    *(volatile int *)0x10000010 = 0x83;
    func_001AB370();
    func_001CCCC0();
    func_001CCBD0(0, 0x3FFF, 0);
    func_001AB430();
    func_001FB210();
    func_001F9820();
    func_001F9780();
    func_00101548(func_001AB140);
    func_001FF1E0(0);
    sub_EXTERMINATION();
    func_001CCB10();
    func_001B5790();
    func_00225CC0();
    func_001AB650();
    func_001AB740(0, func_001AB7E0);
    while (D_00810E88 == 0) {
    }
    *(volatile int *)0x10000000 = 0;
    func_001AED80(0);
    for (;;) {
        D_00810E98 = 0;
        func_001D1AE0(D_00810E80);
        func_001B57E0();
        func_001AEBE0();
        func_001AB6A0();
        func_001FCA10();
        func_001AEE70();
        func_001FB100();
        func_001B5B70();
        if (dma_wait_and_submit(0, 0) != 0) {
            func_0011B910();
            func_0011B5E0();
            func_0011B328();
            func_0011AE88();
            func_0011A9D8();
        }
        gs_readback_queue_drain();
        func_001AB590();
        if (D_00821058 == 1) {
            func_00203350();
            func_001D1C10(D_00810E80);
            func_001AEE70();
        }
        while (D_00810E98 == 0) {
        }
        *(volatile int *)0x10000000 = 0;
        func_001AB4E0(*(volatile short *)0x70003B94, *(volatile short *)0x70003B96);
        if (D_00810E80) {
            p = D_00811070;
        } else {
            p = D_00810F00;
        }
        flip = 1 - D_00810E88;
        func_001015A8(p, *(volatile short *)0x70003B70, *(volatile short *)0x70003B72, flip);
        if (D_00810E80) {
            q = D_008110F0;
        } else {
            q = D_00810F80;
        }
        flip = 1 - D_00810E88;
        func_00101810(q, *(volatile short *)0x70003B70, *(volatile short *)0x70003B72, flip);
        FlushCache(0);
        func_00100550(&D_00810EA0[D_00810E80 * 40]);
        func_001D2300();
        flip = 1 - D_00810E80;
        vsync = D_00810E88;
        D_00810E80 = flip;
        func_001D2580(vsync);
        *(volatile int *)0x70003B64 = *(volatile int *)0x70003B64 + 1;
    }
}
