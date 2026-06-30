// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Builds a DMA chain / scatter list. base points into the big global table
// D_0028F700 at slot D_00810E80 (stride 0x70000) + 0x1F3EC0 + (arg1 << 6).
// The first word is set to 0x20000000 (a DMA tag). It then scans up to 0x1000
// words at arg0: for each non-zero word it records the value (masked to its low
// 28 bits, & 0x0FFFFFFF) into the +4 field of the current chain entry, advances
// the chain pointer to *(arg0 + 0x4000) + 0x10, and clears the source word.
// On exit it writes the masked physical addresses of the next entry (base+0x20)
// and of base out through arg2/arg3 (and the trailing +4 field), i.e. the EE
// "truncate 64-bit address to 28-bit physical" idiom (dsll32 4 / dsrl32 4).
//
// Two keys to the match: (1) the address truncation is written as
// `(long long)ptr & 0xFFFFFFF` (a 64-bit AND, which mwcc lowers to the
// dsll32/dsrl32 pair WITHOUT a spurious 32-bit sign-extension); (2) the base
// address is built in two statements so the `(arg1 << 6)` term is added LAST,
// reproducing the target's addu ordering. Matched with mwcc 2.3.3
// (mwcps2-2.3.3-000906); pinned 991202 reaches 94.5%.
extern char D_0028F700;
extern short D_00810E80;

void func_001CB800(int *arg0, int arg1, int *arg2, int *arg3) {
    char *base;
    int *cur;
    long long v;
    int count;

    base = &D_0028F700 + (int)D_00810E80 * 0x70000 + 0x1F3EC0;
    base = base + (arg1 << 6);
    count = 0;
    *(int *)base = 0x20000000;
    cur = (int *)base;
    do {
        v = *arg0;
        if (v != 0) {
            cur[1] = v & 0xFFFFFFF;
            cur = (int *)(*(int *)((char *)arg0 + 0x4000) + 0x10);
            *arg0 = 0;
        }
        count += 1;
        arg0 += 1;
    } while (count < 0x1000);
    cur[1] = (long long)(base + 0x20) & 0xFFFFFFF;
    *arg2 = (long long)base & 0xFFFFFFF;
    *arg3 = (long long)(base + 0x20) & 0xFFFFFFF;
}
