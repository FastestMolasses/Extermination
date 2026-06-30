// NEARMISS func_00205050  (vram 0x00205050, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residual codegen artifacts after full logic recovery (88.9% mwcc233 vs 69.6% 991202): (1) the 28-bit DMA-address mask (((u64)&D_00821080<<0x24)>>0x24) compiles to an extra dsll32/dsrl32-by-0 64-bit normalization pair that the CW target elides (it shifts the 32-bit pointer directly); (2) the +...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Render-target / DMA pipeline init. Zeroes the gp-relative state words
// D_00275C7C..D_00275C8C, records the requested width/height (arg1/arg2) at
// +0x30/+0x34 and a packed tile count at +0x2C, then configures a GS context
// via func_001002E0 and the +0x14 / +0x11 register bit-fields, kicks the VU1
// cold-start, and builds a DMA chain (func_00205A00 .. func_00205E40) anchored
// at the 28-bit-masked address of D_00821080 OR'd with the 0x20000000 DMA tag.
// Finally programs the VIF1/GIF channel registers at 0x1000A000/10/20 and
// submits via dma_wait_and_submit.
//
// NEARMISS: 88.9% mwcc 2.3.3 (991202 = 69.6%). Logic fully recovered; residual
// is the 28-bit pointer-mask 64-bit normalization and the +0x14 computed-zero
// bit-field scheduling (mwcc codegen/scheduling artifacts).
extern int D_00275C7C;
extern int D_00275C80;
extern int D_00275C84;
extern int D_00275C88;
extern int D_00275C8C;
extern int D_00821080[2];

void func_001002E0(int *a, int b, int c, int d, int e, int f);
void func_00100550(long *a0);
void vu1_cold_start(void);
int *func_00205A00(int *dst, unsigned long long a, int b, int c, int d, int e, int f, int g);
int *func_00205E30(int *a0, int a1);
int *func_00205D00(int *a0, int b1, int b2, int b3, int b4);
int *func_00205D40(int *a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, long long a8);
int *func_00205DD0(int *a0, int b1, int b2, int b3, int b4);
int *func_00205E10(int *a0, int a1, int a2);
void func_00205E40(void *a0, unsigned int a1, unsigned int a2, unsigned int a3,
                   unsigned int a4, unsigned int a5, unsigned int a6);
void dma_wait_and_submit(int a, int b);

void func_00205050(int *arg0, int arg1, int arg2) {
    int *p;
    unsigned int dmaddr;

    D_00275C80 = 0;
    D_00275C7C = 0;
    D_00275C84 = 0;
    D_00275C88 = 0;
    D_00275C8C = 0;
    arg0[0xA] = 0;
    arg0[0xB] = ((((arg1 + 0x3F) >> 6) << 6) * (((arg2 + 0x1F) >> 5) << 5)) >> 0xB;
    arg0[0xC] = arg1;
    arg0[0xD] = arg2;
    func_001002E0(arg0, 0, 0x200, 0xE0, 0, 0);

    {
        int lo = 0 & 0x7FF;
        int hi = lo << 11;
        *(unsigned short *)((char *)arg0 + 0x14) =
            (*(unsigned short *)((char *)arg0 + 0x14) & ~0x7FF) | lo;
        *(int *)((char *)arg0 + 0x14) =
            (*(int *)((char *)arg0 + 0x14) & 0xFFC007FF) | hi;
        *(unsigned char *)((char *)arg0 + 0x11) =
            (*(unsigned char *)((char *)arg0 + 0x11) & ~0x7E) | 0x10;
    }
    func_00100550((long *)arg0);

    vu1_cold_start();

    dmaddr = (unsigned int)(((unsigned long long)&D_00821080 << 0x24) >> 0x24);
    p = func_00205A00((int *)(dmaddr | 0x20000000), 0xE, 1, 0, 0, 0, 1, 6);
    p = func_00205E30(p, 1);
    p = func_00205D00(p, 0, 8, 0, 0);
    p = func_00205D40(p, 0, 0, 0, 0, 0, 0, 0, 0);
    p = func_00205DD0(p, 0, arg0[0xC] - 1, 0, (((arg0[0xD] + 0x1F) >> 5) * 3) << 6);
    p = func_00205E10(p, 0, 0);
    func_00205E40(p, 0, 1, 0, 0, 0, 0);

    *(int *)0x1000A010 = (int)dmaddr;
    *(int *)0x1000A020 = 7;
    *(int *)0x1000A000 = 0x101;
    dma_wait_and_submit(0, 0);
}
