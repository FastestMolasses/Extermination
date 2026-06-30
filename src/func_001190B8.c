// NEARMISS func_001190B8  (vram 0x001190B8, 0x138 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.26% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc global-address %hi-caching / coloring wall. Body fully decoded and correct (init: func_001191F0(0x65,0,&local,&local,0x40,0x40); seven func_00121A28(region,0,size) clears of D_0027CCC0/C6C0/E0C0/F740/F7C0/817C0/819C0; loop i=0x2F..0 writing 0xFFFF and 0x78 shorts at D_0027CCC0+0x4E with 0x6A stride; finalize D...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void func_001191F0(int a, int b, void *c, void *d, int e, int f);
extern void func_00121A28(void *dst, int val, int size);

extern unsigned char D_0027F740[];
extern unsigned char D_0027CCC0[];
extern unsigned char D_0027C6C0[];
extern unsigned char D_0027E0C0[];
extern unsigned char D_0027F7C0[];
extern unsigned char D_002817C0[];
extern unsigned char D_002819C0[];

void func_001190B8(int arg0)
{
    int local[16];
    int i;
    unsigned char *p;
    unsigned char *f740;

    local[0] = 0x1E;
    local[1] = arg0;
    local[4] = 0;
    func_001191F0(0x65, 0, local, local, 0x40, 0x40);

    func_00121A28(D_0027CCC0, 0, 0x13E0);
    func_00121A28(D_0027C6C0, 0, 0x600);
    func_00121A28(D_0027E0C0, 0, 0x1680);
    f740 = D_0027F740;
    func_00121A28(f740, 0, 0x50);
    func_00121A28(D_0027F7C0, 0, 0x2000);
    func_00121A28(D_002817C0, 0, 0x200);
    func_00121A28(D_002819C0, 0, 0x100);

    *(short *)(f740 + 0x3A) = 0x3C;

    p = D_0027CCC0 + 0x4E;
    for (i = 0x2F; i >= 0; i--) {
        *(short *)(p - 0x48) = (short)0xFFFF;
        *(short *)(p) = 0x78;
        p += 0x6A;
    }

    *(int *)(f740 + 0x48) = 0;
    *(int *)(f740 + 0x44) = 1;
    *(int *)(f740 + 0x40) = 0;
}
