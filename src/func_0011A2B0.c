// NEARMISS func_0011A2B0  (vram 0x0011A2B0, 0x1C0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 53.72% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc frame-stride + coloring wall. Expected frame is 0x50 (4 callee-saved: keeps the 0xFFFFFFFF best_val sentinel in t2 materialized once at top AND best_idx in s3 across the search loop); my clean C compiles to 0x40 (3 saved regs), recomputing/relocating the sentinel into a non-saved temp. Also the entry switch (a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027CCC0[];
extern unsigned char D_0027F740[];

extern void func_001157F0(int a0, int a1, int a2, int a3);
extern void func_00121A28(void *p, int a1, int a2);

int func_0011A2B0(int arg0) {
    int start, limit;
    int i;
    long mask;
    unsigned int best_val;
    int best_idx;

    best_idx = -1;

    if (arg0 == 1) {
        start = 0;
        limit = 0x18;
    } else if (arg0 < 2) {
        if (arg0 != 0) {
            return -1;
        }
        start = 0;
        limit = 0x30;
    } else {
        if (arg0 != 2) {
            return -1;
        }
        start = 0x18;
        limit = 0x30;
    }

    best_val = 0xFFFFFFFF;
    mask = 1;
    i = start;
    while (i < limit) {
        unsigned char *entry = D_0027CCC0 + i * 0x6A;
        if (*(unsigned short *)entry == 0) {
            if (*(unsigned short *)(entry + 0x1A) != 3) {
                *(unsigned short *)entry = 1;
                *(unsigned short *)(entry + 0x1A) = 3;
                return i;
            }
        }
        {
            unsigned int v = *(unsigned short *)(entry + 0xA);
            if (v < best_val && *(unsigned short *)(entry + 0x1A) != 3) {
                best_val = v;
                best_idx = i;
            }
        }
        i++;
        mask <<= 1;
    }

    if (best_idx == -1) {
        return -1;
    }

    func_001157F0(3, 0, 0, 0);
    {
        unsigned char *entry = D_0027CCC0 + best_idx * 0x6A;
        *(long *)(D_0027F740 + 0x28) |= mask;
        func_00121A28(entry, 0, 0x6A);
        *(unsigned short *)(entry + 6) = 0xFFFF;
        *(unsigned short *)(entry + 0x26) = 0xFFFF;
        *(unsigned short *)(entry + 0x24) = 0xFFFF;
        *(unsigned short *)(entry + 0x22) = 0xFFFF;
        *(unsigned short *)(entry + 0x4E) = 0x78;
        *(unsigned short *)entry = 1;
        *(unsigned short *)(entry + 0x1A) = 3;
        return best_idx;
    }
}
