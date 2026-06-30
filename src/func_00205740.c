// NEARMISS func_00205740  (vram 0x00205740, 0xCC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling near-miss (94.9%, mwcc233): logic fully recovered, identical instruction multiset. Sole residual is the `sw local[3], 0x3C(sp)` store positioned one slot later than the target (target emits it interleaved among the call-argument paddub moves) -- a pure EE list-scheduler ord...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 94.9% (mwcc233). DMA/GS packet dispatch: masks D_008211C0 to 28 bits,
// builds a 4-int local descriptor (local[0..3]), calls func_00205A00 to build the
// DMA source record then func_002058D0(ret, local, arg1), writes the three D1
// channel HW registers (0x1000A000/A010/A020), and submits via dma_wait_and_submit.
// Body/structure correct; sole residual is one store (local[3]->0x3C(sp)) scheduled
// one slot late vs target -- EE list-scheduler artifact, permuter territory.
extern int *func_00205A00(int *dst, unsigned long long a, int b, int c, int d, int e, int f, int g);
extern void func_002058D0(int a0, int *a1, unsigned int a2);
extern void dma_wait_and_submit(int a0, int a1);
extern char D_008211C0[];

void func_00205740(char *arg0, unsigned int arg1) {
    int local[4];
    int addr;

    addr = (int)D_008211C0 & 0x0FFFFFFF;
    local[0] = 0;
    local[1] = 0;
    local[2] = *(int *)(arg0 + 0x30) << 4;
    local[3] = (((*(int *)(arg0 + 0x34) + 0x1F) >> 5) * 3) << 10;

    func_002058D0((int)func_00205A00((int *)(addr | 0x20000000), 0xE, 1, 0, 0, 0, 1, 4),
                  local, arg1);

    *(int *)0x1000A010 = addr;
    *(int *)0x1000A020 = 5;
    *(int *)0x1000A000 = 0x101;
    dma_wait_and_submit(0, 0);
}
