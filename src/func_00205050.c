// NEARMISS func_00205050  (vram 0x00205050, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.39% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) (was 88.91%).
// Matching wave 2026-10-28 fixes read from the original instructions:
//  - FIDELITY: the last chain call is func_00205E40(p, 1, 1, 0, 0, 0, 0); the old C
//    passed 0 as the first field.
//  - the +0x14 / +0x11 masks are bit-field stores to a GS DISPFB register at +0x10
//    (DBX, DBY, then FBW = 8), not hand-written masks.
//  - the 28-bit DMA address is `(unsigned int)&D_00821080 & 0x0FFFFFFF` (mwcc emits
//    the mask as a 64-bit shift pair itself).
// Remaining differences: the tile-count multiply is scheduled one slot later (after
// the third saved-register spill instead of before it); every other instruction matches.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Render-target / DMA pipeline init. Zeroes the gp-relative state words
// D_00275C7C..D_00275C8C, records the requested width/height (arg1/arg2) at
// +0x30/+0x34 and a packed tile count at +0x2C, then configures a GS context
// via func_001002E0, then sets the DISPFB register at +0x10 (DBX = DBY = 0,
// FBW = 8, i.e. 512 pixels), kicks the VU1
// cold-start, and builds a DMA chain (func_00205A00 .. func_00205E40) anchored
// at the 28-bit-masked address of D_00821080 OR'd with the 0x20000000 DMA tag.
// Finally programs the VIF1/GIF channel registers at 0x1000A000/10/20 and
// submits via dma_wait_and_submit.
typedef unsigned long long u64;
typedef struct {
    u64 FBP : 9;
    u64 FBW : 6;
    u64 PSM : 5;
    u64 pad0 : 12;
    unsigned int DBX : 11;
    unsigned int DBY : 11;
    unsigned int pad1 : 10;
} DispFb;

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
extern int dma_wait_and_submit(int query, int timeout);

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

    ((DispFb *)(arg0 + 4))->DBX = 0;
    ((DispFb *)(arg0 + 4))->DBY = 0;
    ((DispFb *)(arg0 + 4))->FBW = 8;
    func_00100550((long *)arg0);

    vu1_cold_start();

    dmaddr = (unsigned int)&D_00821080 & 0x0FFFFFFF;
    p = func_00205A00((int *)(dmaddr | 0x20000000), 0xE, 1, 0, 0, 0, 1, 6);
    p = func_00205E30(p, 1);
    p = func_00205D00(p, 0, 8, 0, 0);
    p = func_00205D40(p, 0, 0, 0, 0, 0, 0, 0, 0);
    p = func_00205DD0(p, 0, arg0[0xC] - 1, 0, (((arg0[0xD] + 0x1F) >> 5) * 3) << 6);
    p = func_00205E10(p, 0, 0);
    func_00205E40(p, 1, 1, 0, 0, 0, 0);

    *(int *)0x1000A010 = (int)dmaddr;
    *(int *)0x1000A020 = 7;
    *(int *)0x1000A000 = 0x101;
    dma_wait_and_submit(0, 0);
}
