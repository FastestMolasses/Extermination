// NEARMISS func_00117428  (vram 0x00117428, 0x2B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.05% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc global-address-form wall (target uses `la sym` + `lw 0x30(reg)`; ee-gcc folds the offset into `lui %hi(sym+48)` / `lw %lo(sym+48)`) + the resulting forward branch-likely (annul) loss on two early-exits + GPR coloring. Pointer-base and -mno-split-addresses variants tested and rejected (see h...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// Sound-channel allocator. Scans the 48-entry channel table three times, round-robin from a
// shared cursor:
//   pass 1 (only when arg0 != 0): find a channel already playing this exact sound (state 2,
//                                 f20 == arg0, f22 == arg2) so it can be retriggered;
//   pass 2:                       find a completely free slot (f00 == 0, state != 3);
//   pass 3:                       score the busy slots and steal the cheapest one.
// arg1 is the caller's priority, compared against each entry's f1E.
//

extern unsigned char D_0027CCC0[];
extern unsigned char D_0027F740[];

int func_00117428(int arg0, int arg1, int arg2)
{
    int i;
    int idx;
    int cand1 = -1;
    int cand2 = -1;
    int cand3 = -1;
    int best1 = -1;
    int best2 = -1;
    int best3 = -1;

    if (arg0 != 0) {
        for (i = 0; i < 0x30; i++) {
            idx = *(unsigned int *)(D_0027F740 + 0x30) % 0x30;
            if (*(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x1A) == 2 &&
                *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x20) == arg0 &&
                *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x22) == arg2)
                return idx;
            *(unsigned int *)(D_0027F740 + 0x30) = *(unsigned int *)(D_0027F740 + 0x30) + 1;
        }
    }

    for (i = 0; i < 0x30; i++) {
        idx = *(unsigned int *)(D_0027F740 + 0x30) % 0x30;
        if (*(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x0) == 0 &&
            *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x1A) != 3)
            return idx;
        *(unsigned int *)(D_0027F740 + 0x30) = *(unsigned int *)(D_0027F740 + 0x30) + 1;
    }

    for (i = 0; i < 0x30; i++) {
        idx = *(unsigned int *)(D_0027F740 + 0x30) % 0x30;
        if (*(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x8) == 1) {
            if (*(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x1A) == 1)
                return idx;
            if (*(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x1A) == 2 &&
                arg1 >= *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x1E) &&
                *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0xA) < best2) {
                cand2 = idx;
                best2 = *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0xA);
            }
        } else if (*(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x1A) == 1) {
            if (*(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0xA) < best1) {
                cand1 = idx;
                best1 = *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0xA);
            }
        } else if (*(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x1A) == 2) {
            if (arg1 >= *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0x1E) &&
                *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0xA) < best3) {
                cand3 = idx;
                best3 = *(unsigned short *)(D_0027CCC0 + idx * 0x6A + 0xA);
            }
        }
        *(unsigned int *)(D_0027F740 + 0x30) = *(unsigned int *)(D_0027F740 + 0x30) + 1;
    }

    if (cand1 != -1)
        return cand1;
    if (cand2 != -1)
        return cand2;
    if (cand3 != -1)
        return cand3;
    return -1;
}
