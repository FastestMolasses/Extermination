// NEARMISS sub_Invalid_modion_type_ignored_d  (vram 0x001033E0, 0x334 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.02% via ee-gcc 2.9-991111-01 (-O2) (matching wave 2026-10-27; was 88.94%).
// Fixes read from the original instructions: the decoder-slot table D_002418F8 is an
// array of two 0x140-byte slot records plus the current-slot index at 0x280 (struct
// access instead of one reused char pointer, which had pinned the base in one register);
// the DMA tag destination is (D_002412EC & 0x0FFFFFFF) | 0x20000000 (uncached), the tag
// pairs written at +0 and +0x10; each framebuffer-select arm has its own pointer local.
// Remaining differences: in the mode-3 arm the original schedules the high half of
// D_00241544's address one slot before the current-slot reload (two instructions swapped);
// temps, inline helper, struct/array/volatile table types, operand orders and
// scheduling flags did not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241408;
extern int D_00241428;
extern int D_002412EC;
extern int D_002414AC;
extern char D_0026B2E0[];
extern int *D_00241544;
extern int *D_00241548;
extern int *D_0024154C;

extern void func_0010A378(char *msg, int arg);
extern void sub_c_invalid_motion_type_d_2(int x, int y, int flags);

struct Ent {
    int dma;            /* 0x000 */
    int pad4;
    int a[4];           /* 0x008 */
    int b[68];          /* 0x018 */
    int base;           /* 0x128 */
    int count;          /* 0x12C */
    int first;          /* 0x130 */
    int field;          /* 0x134 */
    int busy;           /* 0x138 */
    int pad13c;
};
struct Ctx {
    struct Ent e[2];
    int cur;            /* 0x280 */
};
extern struct Ctx D_002418F8;
#define CUR (D_002418F8.e[D_002418F8.cur])

int sub_Invalid_modion_type_ignored_d(int mba, int field, int flags, int mtype)
{
    int x = mba % D_00241428;
    int y = mba / D_00241428;
    int px = x * 16;
    int py = y * 16;
    int first = flags & 1;

    if (first) {
        while ((*(volatile unsigned int *)0x1000D400 >> 8) & 1)
            ;
        CUR.busy = 0;
    } else {
        int i;
        int n;
        unsigned long *dst;

        if ((unsigned int)(mtype - 1) >= 3) {
            func_0010A378(D_0026B2E0, mtype);
            D_00241408 = 1;
            return 0;
        }
        sub_c_invalid_motion_type_d_2(px, py, flags);

        while ((*(volatile unsigned int *)0x1000D400 >> 8) & 1)
            ;

        dst = (unsigned long *)((D_002412EC & 0x0FFFFFFF) | 0x20000000);
        n = CUR.count;
        for (i = 0; i < n; i++) {
            int id = (i == n - 1) ? 0 : 3;
            dst[0] = ((unsigned long)(CUR.a[i] & 0x0FFFFFFF) << 32) | 0x30000030;
            dst[2] = ((unsigned long)(CUR.b[i] & 0x0FFFFFFF) << 32)
                   | ((unsigned long)id << 28) | 0x30;
            dst += 4;
        }
        __asm__("sync");
        *(volatile int *)0x1000D480 = CUR.dma;
        *(volatile int *)0x1000D430 = D_002412EC;
        *(volatile int *)0x1000D420 = 0;
        *(volatile int *)0x1000D400 = 0x105;
        CUR.busy = 1;
    }

    if (field == 1 && (flags & 2)) {
        CUR.field = field;
    } else {
        CUR.field = 0;
    }

    CUR.first = first;

    if (D_002414AC == 3) {
        int *fb = D_00241544;
        CUR.base = fb[0] + (x * fb[4] + y) * 0x180;
    } else {
        int *fb = (D_002414AC == 2) ? D_0024154C : D_00241548;
        CUR.base = fb[0] + (x * fb[4] + y) * 0x180;
    }
    return 1;
}
