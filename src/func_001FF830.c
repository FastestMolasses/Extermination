// func_001FF830 -- byte-matched from C (objdiff 100%). Jump-table dispatcher: the
// compiled local .rodata table is pinned at its original address
// (tools/decomp/rodata_pin.py). Promoted from NEARMISS in the jr-table lane
// (2026-09-23): scratchpad externs (idiom-32); `(char *)D_00289BC0` in the
// func_00200780 call (argument weight); the first loop indexes
// `((h + (int)i) << 3)`; the second loop has its own counter `k`, and the
// case-7 locals are declared h, n, i, base, off, q, cnt, e, k (idiom-35).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003B90

// SEMANTICS: one step of the resource-bank streamer, driven by the slot record
// whose pointer lives in EE scratchpad 0x70003B6C and dispatched on the slot's
// state byte at +9 through jtbl_00273260 (8 states).  `arg0` is the bank id.
//   0: pick the destination buffer for this bank id and publish it in the
//      gp-rel pointer D_00275C74, recording a kind code in slot+0xF:
//      ids 2/3 -> D_0028A738, kind 0; ids 1/0x27/0x28/0x29/0x37 -> the fixed
//      address 0x01800000, kind 1; id 0x1D -> D_0028A5A0, kind 2;
//      ids 0x32..0x35 -> D_0028A744, kind 3; id 0x36 -> D_0028A748, kind 2;
//      ids 0x2A/0x2B -> 0x01800000 or D_0028A748 depending on the flag byte at
//      0x70003B90, kind 1; anything else -> D_0028A748, kind 1.  Then advance
//      the state and kick the header DMA
//      func_00200780(D_0028A480, D_00289BC0, arg0 << 11, 0x800).
//   1: poll func_00200730(): 0 = keep waiting, 1 = advance the state,
//      anything else = restart at state 0.
//   2: run the sub-streamer func_001FF3F0(); advance the state when it reports
//      completion.
//   3: advance the state, then issue the payload DMA
//      func_00200780(D_0028A488, D_00275C74, desc[4] + desc[0x14],
//      desc[8] - desc[0x14]) where desc is the current bank header D_00275C70.
//   4: poll func_00200730(): 0 = wait, 1 = advance, otherwise step back one
//      state.
//   5: force the state to 7, then commit the cursor using the header span
//      delta (desc[8] - desc[0x14]) added to D_00275C74, selected by the kind
//      code in slot+0xF: 0 -> D_0028A73C, 2 -> D_0028A744 and mirror it into
//      D_0028A748, 3 -> jump to state 6 instead.
//   6: func_001FB370(D_00275C74); on success publish the result in D_0028A748
//      and advance the state.
//   7: walk the header: for each of the desc[0x10] chunks run
//      func_00200830(base + off), advancing off by field 0x24 of slot
//      (desc[0xE] + i); then relocate the desc[0x1C] pointer table that starts
//      at slot (desc[0xE] + count) + 0x20 -- for each entry e,
//      D_0028A490[e >> 24] = (e & 0xFFFFFF) + base.  Finally mark the slot done
//      (slot[8] = 0x63, slot[9] = 0).

extern char *volatile D_70003B6C[16];               /* PS2 scratchpad @ 0x70003B6C */
extern unsigned char D_70003B90[16];                /* PS2 scratchpad @ 0x70003B90 */

extern void func_00200780(void *file, void *buf, int offset, int size);
extern int func_00200730(void);
extern void func_00200830(char *a);
extern int func_001FF3F0(void);
extern char *func_001FB370(char *a);

extern char *D_00275C70;
extern char *D_00275C74;
extern char D_00289BC0[0x800];
extern char D_0028A480[16];
extern char D_0028A488[16];
extern char *D_0028A490[64];
extern char *D_0028A5A0[16];
extern char *D_0028A738[16];
extern char *D_0028A73C[16];
extern char *D_0028A744[16];
extern char *D_0028A748[16];

void func_001FF830(int arg0) {
    char *slot;
    unsigned char *st;
    char *p;
    char *d;
    int r;

    slot = D_70003B6C[0];
    st = (unsigned char *)(slot + 9);
    switch (*(unsigned char *)(slot + 9)) {
    case 0:
        switch (arg0) {
        case 2:
        case 3:
            D_00275C74 = D_0028A738[0];
            slot[0xF] = 0;
            break;
        case 1:
        case 0x27:
        case 0x37:
        case 0x28:
        case 0x29:
            D_00275C74 = (char *)0x01800000;
            slot[0xF] = 1;
            break;
        case 0x1D:
            D_00275C74 = D_0028A5A0[0];
            slot[0xF] = 2;
            break;
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
            D_00275C74 = D_0028A744[0];
            slot[0xF] = 3;
            break;
        case 0x36:
            D_00275C74 = D_0028A748[0];
            slot[0xF] = 2;
            break;
        case 0x2A:
        case 0x2B:
            if (D_70003B90[0] == 0) {
                D_00275C74 = (char *)0x01800000;
            } else {
                D_00275C74 = D_0028A748[0];
            }
            slot[0xF] = 1;
            break;
        default:
            D_00275C74 = D_0028A748[0];
            slot[0xF] = 1;
            break;
        }
        p = D_70003B6C[0];
        *(unsigned char *)(p + 9) += 1;
        func_00200780(D_0028A480, (char *)D_00289BC0, arg0 << 0xB, 0x800); /* no-op cast = argument weight (matching device) */
        break;
    case 1:
        r = func_00200730();
        if (r != 0) {
            if (r == 1) {
                p = D_70003B6C[0];
                *(unsigned char *)(p + 9) += 1;
            } else {
                p = D_70003B6C[0];
                p[9] = 0;
            }
        }
        break;
    case 2:
        if (func_001FF3F0() != 0) {
            p = D_70003B6C[0];
            *(unsigned char *)(p + 9) += 1;
        }
        break;
    case 3:
        {
            int s;
            *st += 1;
            d = D_00275C70;
            s = *(int *)(d + 0x14);
            func_00200780(D_0028A488, D_00275C74, *(int *)(d + 4) + s, *(int *)(d + 8) - s);
        }
        break;
    case 4:
        r = func_00200730();
        if (r != 0) {
            if (r == 1) {
                p = D_70003B6C[0];
                *(unsigned char *)(p + 9) += 1;
            } else {
                p = D_70003B6C[0];
                *(unsigned char *)(p + 9) -= 1;
            }
        }
        break;
    case 5:
        {
            char *cur;
            int delta;
            int f;
            *st = 7;
            d = D_00275C70;
            p = D_70003B6C[0];
            cur = D_00275C74;
            delta = *(int *)(d + 8) - *(int *)(d + 0x14);
            f = *(unsigned char *)(p + 0xF);
            if (f == 0) {
                D_0028A73C[0] = cur + delta;
            } else if (f == 2) {
                D_0028A744[0] = cur + delta;
                D_0028A748[0] = D_0028A744[0];
            } else if (f == 3) {
                p[9] = 6;
            }
        }
        break;
    case 6:
        {
            char *res;
            res = func_001FB370(D_00275C74);
            if (res != 0) {
                D_0028A748[0] = res;
                p = D_70003B6C[0];
                *(unsigned char *)(p + 9) += 1;
            }
        }
        break;
    case 7:
        {
            int h;
            int n;
            unsigned int i;
            char *base;
            int off;
            int *q;
            int cnt;
            unsigned int e;
            unsigned int k;

            d = D_00275C70;
            base = D_00275C74;
            n = *(int *)(d + 0x10);
            h = *(unsigned short *)(d + 0xE);
            off = 0;
            if (n != 0) {
                for (i = 0; i < (unsigned int)n; i++) {
                    func_00200830(base + off);
                    off += *(int *)(D_00275C70 + ((h + (int)i) << 3) + 0x24);
                }
            }
            d = D_00275C70;
            cnt = *(int *)(d + 0x1C);
            if (cnt != 0) {
                q = (int *)(d + (h + n) * 8 + 0x20);
                for (k = 0; k < (unsigned int)cnt; k++) {
                    e = *(unsigned int *)q;
                    D_0028A490[e >> 0x18] = (char *)((e & 0xFFFFFF) + (int)base);
                    q += 1;
                }
            }
            (D_70003B6C[0])[8] = 0x63;
            (D_70003B6C[0])[9] = 0;
        }
        break;
    }
}
