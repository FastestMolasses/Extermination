// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 57.64% NEARMISS).
// Fixes read from the original instructions:
//  - The index is a 31-bit bitfield read of the +0x34 word (the 64-bit shift
//    pair the target uses is mwcc's bitfield extract), not a 64-bit local;
//    the entry is indexed as an array of 8-byte records.
//  - The +8 word is unsigned: that fixes the load order and add operand order
//    of the position sum (+8 + +0x60).
//  - The step==0 / mode-0 path calls func_001FD580 unconditionally; the old C
//    wrongly tested the flags byte first.
//  - Both D_008106F5 dispatches are switches with cases 0, 1, 2 in source order
//    ending in `break`; the per-entry frame is read before the flags byte.
//
// Frame/clip stepping loop. Each iteration: arg0+0x34 = arg0+8 + arg0+0x60,
// idx = its low 31 bits. If bit31 is set the entry table is D_00264DD0[0];
// otherwise it is D_00264DD0[D_00810700 + 1]. frame = entry.frame (+0),
// flags = entry.flags (+5). When bit31 is clear, D_008106F5 gates: mode 2
// returns 0; when arg0+0x60 is still 0 and the mode is 0, a successful
// func_001FD580(idx, arg0+0x70) (returns 1) also returns 0. Then, if both
// flags and frame are zero, arg0+0x60 is incremented and the loop repeats.
// Otherwise frame is committed to arg0+0x6C (arg0+0x68 is zeroed when
// arg0+0x60 is 0), flags goes to arg0+0x50 and entry byte +4 to arg0+0x51,
// and the function returns 1.

extern int func_001FD580(unsigned int idx, int *out);
extern int D_00264DD0[];
extern unsigned char D_008106F5;
extern unsigned char D_00810700;

/* The +0x34 position word: low 31 bits are the entry index, bit 31 selects
 * the shared table 0 instead of the current state's table. */
typedef struct {
    unsigned int idx : 31;
    unsigned int neg : 1;
} Slot;

/* 8-byte timeline entry (same layout func_001FD580 scans). */
typedef struct {
    unsigned short frame;   /* +0 */
    short id;               /* +2 */
    unsigned char f4;       /* +4: copied to arg0+0x51 */
    unsigned char flags;    /* +5 */
    unsigned char pad6[2];  /* +6 */
} Entry8;

int func_001FD790(int arg0) {
    unsigned short frame;
    unsigned char flags;
    unsigned int idx;
    Entry8 *base;
    int v;

    for (;;) {
        *(int *)(arg0 + 0x34) = *(unsigned int *)(arg0 + 8) + *(int *)(arg0 + 0x60);
        v = *(int *)(arg0 + 0x34);
        idx = ((Slot *)(arg0 + 0x34))->idx;
        if (v & 0x80000000) {
            base = (Entry8 *)D_00264DD0[0];
            frame = base[idx].frame;
            flags = base[idx].flags;
        } else {
            base = (Entry8 *)D_00264DD0[D_00810700 + 1];
            frame = base[idx].frame;
            flags = base[idx].flags;
            if (*(int *)(arg0 + 0x60) == 0) {
                switch (D_008106F5) {
                case 0:
                    if (func_001FD580(idx, (int *)(arg0 + 0x70)) == 1) {
                        return 0;
                    }
                    break;
                case 1:
                    break;
                case 2:
                    return 0;
                }
            } else {
                switch (D_008106F5) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    return 0;
                }
            }
        }
        if (flags == 0 && frame == 0) {
            *(int *)(arg0 + 0x60) = *(int *)(arg0 + 0x60) + 1;
        }
        if (flags != 0 || frame != 0) {
            break;
        }
    }
    if (*(int *)(arg0 + 0x60) == 0) {
        *(int *)(arg0 + 0x68) = 0;
    }
    *(int *)(arg0 + 0x6C) = frame;
    *(unsigned char *)(arg0 + 0x50) = flags;
    *(unsigned char *)(arg0 + 0x51) = base[idx].f4;
    return 1;
}
