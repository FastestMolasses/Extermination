// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.90% NEARMISS).
// Fixes read from the original instructions:
//  - The 28-bit DMA address of D_008211C0 is computed AFTER the four descriptor
//    words are filled, not before; that statement order puts the local[3] store
//    among the call-argument setup exactly as the original does.
//
// DMA/GS packet dispatch: masks D_008211C0 to 28 bits, builds a 4-int local
// descriptor (local[0..3]), calls func_00205A00 to build the DMA source record,
// then func_002058D0(ret, local, arg1), writes the three D1 channel HW registers
// (0x1000A000/A010/A020), and submits via dma_wait_and_submit.
extern int *func_00205A00(int *dst, unsigned long long a, int b, int c, int d, int e, int f, int g);
extern void func_002058D0(int a0, int *a1, unsigned int a2);
extern void dma_wait_and_submit(int a0, int a1);
extern char D_008211C0[];

void func_00205740(char *arg0, unsigned int arg1) {
    int local[4];
    int addr;

    local[0] = 0;
    local[1] = 0;
    local[2] = *(int *)(arg0 + 0x30) << 4;
    local[3] = (((*(int *)(arg0 + 0x34) + 0x1F) >> 5) * 3) << 10;
    addr = (int)D_008211C0 & 0x0FFFFFFF;

    func_002058D0((int)func_00205A00((int *)(addr | 0x20000000), 0xE, 1, 0, 0, 0, 1, 4),
                  local, arg1);

    *(int *)0x1000A010 = addr;
    *(int *)0x1000A020 = 5;
    *(int *)0x1000A000 = 0x101;
    dma_wait_and_submit(0, 0);
}
