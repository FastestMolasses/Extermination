// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-frame subsystem update: init via func_001FABB0, then a 0x30-slot
// loop calling func_0011A470(i) for every slot whose func_00119890(0,i)
// returns 4 (branch-likely bnel guard). Then walks 3 records of stride
// 0x60 in D_00281FD0, each holding a 0x30-bit mask at field +0x8; for
// every set bit calls func_0011A5C8(bit) (beqzl guard), shifting the mask
// right per bit. Finalizes via func_0011A4D0. Local-decl order (b, mask,
// p, g) is the regalloc tie-break: it pins the pointer to s2 and the outer
// group counter to s3 to match the target coloring; only mwcc 2.3.3 lands
// it 100%.
extern void func_001FABB0(void);
extern int func_00119890(int a0, int a1);
extern void func_0011A470(int a0);
extern void func_0011A5C8(int a0);
extern void func_0011A4D0(void);
extern char D_00281FD0[];

void func_001F9B20(void) {
    int i;
    int b;
    int mask;
    char *p;
    int g;
    func_001FABB0();
    for (i = 0; i < 0x30; i++) {
        if (func_00119890(0, i) == 4) {
            func_0011A470(i);
        }
    }
    p = D_00281FD0;
    for (g = 0; g < 3; g++) {
        mask = *(int *)(p + 0x8);
        for (b = 0; b < 0x30; b++) {
            if (mask & 1) {
                func_0011A5C8(b);
            }
            mask >>= 1;
        }
        p += 0x60;
    }
    func_0011A4D0();
}
