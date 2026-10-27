// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.67% NEARMISS).
// Fixes read from the original instructions:
//  - relocation loops (states 7 and 11): the masked 24-bit offset is the first
//    operand and the cursor is added as an unsigned integer, then cast back.
//  - state 9: the read offset is accumulated into n1 (n1 += n2) and n1 is passed,
//    which puts n1 straight into the argument register.
//  - state 11's relocation loop uses its own counter/bound/cursor/word locals,
//    declared word, index, count, pointer (local order decides their registers).
//  - the callee externs stay unprototyped: the byte-matched callee prototypes
//    (func_00200780 takes 4, func_002009E0 2, func_00200830 1, func_001FF590 2)
//    are honoured by every call's argument count, and full prototypes reorder
//    the argument setup in states 0/1.

// SEMANTICS: Two-stage streaming loader / relocator state machine. The context
// lives in scratchpad at 0x70003B6C: byte 8 is a status/result code, byte 9 the
// main state (dispatched through jtbl_00273280, 12 entries) and byte 0xA a
// sub-state used by the two file-open phases. D_00810700 is the current request
// slot, D_00810701 / D_00810703 / D_00810704 track chained sub-file indices,
// D_00275C70 points at the current header block and D_0028A73C / D_0028A740 /
// D_0028A744 / D_0028A748 are the running load/allocation cursors.
//   0  - advance and issue func_00200780 for descriptor D_0028A3C0[slot*8] into
//        D_00275304 (size -1 = whole file).
//   1  - poll func_00200730: 1 -> advance and func_002009E0(D_00275304,
//        D_0028A3C4[slot*8]); any other nonzero result resets state 9 to 0.
//   2  - advance and issue the header read: func_00200780(D_0028A480, D_00289BC0,
//        (slot+4) << 11, 0x800).
//   3  - poll: 1 -> latch D_00810703 = D_00810700 and advance; other nonzero -> back up one state.
//   4  - open phase A on sub-state 0xA: 0 sets D_00275C70 = D_00289BC0 and bumps
//        the sub-state, then 1 waits for func_001FF590(0xAB, 0) and 2 waits for
//        func_001FF590(0xAB, 1); once both pass the sub-state is cleared and the
//        main state advances (0 and 1 fall through to the next sub-case).
//   5  - advance and stream the first block: length = hdr[8]-hdr[0x14], bump the
//        write cursor D_0028A740 = D_0028A73C + length, then
//        func_00200780(D_0028A488, D_0028A73C, hdr[4]+hdr[0x14], length).
//   6  - poll: 1 -> advance, other nonzero -> back up one state.
//   7  - fix-up pass for the first buffer. total = sum over the hdr[0x10] section
//        records at hdr + (hdr[0xE]+hdr[0xC] + i)*8 + 0x24, calling
//        func_00200830(D_0028A73C + total) per section; then apply the hdr[0x1C]
//        relocation words at hdr + (base+count)*8 + 0x20, each writing
//        D_0028A490[w >> 24] = D_0028A73C + (w & 0xFFFFFF). func_00200890() then
//        commits. If hdr[0x18] == 0 the chain is finished: clear D_00810701 /
//        D_00810704, point D_0028A744 and D_0028A748 at D_0028A740, set the ctx
//        status byte 8 to 0x63 and reset state to 0. Otherwise latch
//        D_00810704 = D_00810701, retarget D_00275C70 at
//        D_00289BC0 + D_00810701*0x70 + 0x100 and advance.
//   8  - open phase B on sub-state 0xA, same shape as state 4 but with tag 0xAC
//        and no D_00275C70 assignment.
//   9  - advance and stream the second block into D_0028A740, bumping D_0028A744.
//   10 - poll: 1 -> latch D_0028A748 = D_0028A744 and advance; other nonzero -> back up.
//   11 - fix-up pass for the second buffer (same two loops, cursor D_0028A740),
//        then status byte 8 = 0x63 and state reset to 0.
// A main state >= 12 falls straight through and does nothing.
//
// NOTE: the scratchpad state bytes are reached through `volatile` pointers and
// the four load cursors are `volatile` so the loads/stores keep source order.

extern unsigned char *D_70003B6C[16];               /* PS2 scratchpad @ 0x70003B6C */

extern int func_001FF590();
extern int func_00200730();
extern int func_00200780();
extern int func_00200830();
extern int func_00200890();
extern int func_002009E0();

extern void *D_00275304[];
extern unsigned char *D_00275C70;
extern unsigned char D_00289BC0[];
extern unsigned char D_0028A3C0[];
extern unsigned char D_0028A3C4[];
extern unsigned char D_0028A480[];
extern unsigned char D_0028A488[];
extern unsigned char *D_0028A490[];
extern unsigned char *volatile D_0028A73C[];
extern unsigned char *volatile D_0028A740[];
extern unsigned char *volatile D_0028A744[];
extern unsigned char *volatile D_0028A748[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char D_00810703[];
extern unsigned char D_00810704[];

void func_001FFCD0(void) {
    unsigned char *p = D_70003B6C[0];
    volatile unsigned char *st = p + 9;
    volatile unsigned char *sub;
    unsigned char *q;
    unsigned char *h;
    unsigned int *t;
    unsigned char *v;
    unsigned int base;
    unsigned int n;
    unsigned int i;
    int total;
    unsigned int cnt;
    unsigned int w;
    unsigned int j;
    int n1;
    int n2;
    int n3;
    int res;
    unsigned int w2;
    unsigned int j2;
    unsigned int cnt2;
    unsigned int *t2;

    switch (*st) {
    case 0:
        *st = *st + 1;
        func_00200780(&D_0028A3C0[D_00810700[0] * 8], D_00275304[0], 0, -1);
        break;
    case 1:
        res = func_00200730();
        if (res != 0) {
            if (res == 1) {
                q = D_70003B6C[0];
                q[9] = q[9] + 1;
                func_002009E0(D_00275304[0], *(int *)&D_0028A3C4[D_00810700[0] * 8]);
            } else {
                q = D_70003B6C[0];
                q[9] = 0;
            }
        }
        break;
    case 2:
        *st = *st + 1;
        func_00200780(D_0028A480, (int)D_00289BC0, (D_00810700[0] + 4) << 11, 0x800); /* matching device: argument weight; the parameter is really void *buf */
        break;
    case 3:
        res = func_00200730();
        if (res != 0) {
            if (res == 1) {
                D_00810703[0] = D_00810700[0];
                q = D_70003B6C[0];
                q[9] = q[9] + 1;
            } else {
                q = D_70003B6C[0];
                q[9] = q[9] - 1;
            }
        }
        break;
    case 4:
        sub = p + 0xA;
        switch (*sub) {
        case 0:
            D_00275C70 = D_00289BC0;
            *sub = *sub + 1;
            /* fallthrough */
        case 1:
            if (func_001FF590(0xAB, 0) == 0) {
                break;
            }
            q = D_70003B6C[0];
            q[0xA] = q[0xA] + 1;
            /* fallthrough */
        case 2:
            if (func_001FF590(0xAB, 1) == 0) {
                break;
            }
            q = D_70003B6C[0];
            q[0xA] = 0;
            q = D_70003B6C[0];
            q[9] = q[9] + 1;
            break;
        }
        break;
    case 5:
        *st = *st + 1;
        h = D_00275C70;
        n1 = *(int *)(h + 4);
        n2 = *(int *)(h + 0x14);
        n3 = *(int *)(h + 8);
        D_0028A740[0] = D_0028A73C[0] + (n3 - n2);
        func_00200780(D_0028A488, D_0028A73C[0], n1 + n2, n3 - n2);
        break;
    case 6:
        res = func_00200730();
        if (res != 0) {
            if (res == 1) {
                q = D_70003B6C[0];
                q[9] = q[9] + 1;
            } else {
                q = D_70003B6C[0];
                q[9] = q[9] - 1;
            }
        }
        break;
    case 7:
        h = D_00275C70;
        total = 0;
        base = *(unsigned short *)(h + 0xE) + *(unsigned short *)(h + 0xC);
        n = *(unsigned int *)(h + 0x10);
        if (n != 0) {
            for (i = 0; i < n; i++) {
                func_00200830(D_0028A73C[0] + total);
                total += *(int *)(D_00275C70 + (base + i) * 8 + 0x24);
            }
        }
        cnt = *(unsigned int *)(D_00275C70 + 0x1C);
        if (cnt != 0) {
            t = (unsigned int *)(D_00275C70 + (base + n) * 8 + 0x20);
            for (j = 0; j < cnt; j++) {
                w = *t;
                D_0028A490[w >> 24] = (unsigned char *)((w & 0xFFFFFF) + (unsigned int)D_0028A73C[0]);
                t++;
            }
        }
        func_00200890();
        if (*(int *)(D_00275C70 + 0x18) == 0) {
            D_00810701[0] = 0;
            D_00810704[0] = 0;
            v = D_0028A740[0];
            q = D_70003B6C[0];
            D_0028A748[0] = v;
            D_0028A744[0] = v;
            q[8] = 0x63;
            q = D_70003B6C[0];
            q[9] = 0;
        } else {
            D_00810704[0] = D_00810701[0];
            q = D_70003B6C[0];
            D_00275C70 = &D_00289BC0[D_00810701[0] * 0x70] + 0x100;
            q[9] = q[9] + 1;
        }
        break;
    case 8:
        switch (p[0xA]) {
        case 0:
            if (func_001FF590(0xAC, 0) == 0) {
                break;
            }
            q = D_70003B6C[0];
            q[0xA] = q[0xA] + 1;
            /* fallthrough */
        case 1:
            if (func_001FF590(0xAC, 1) == 0) {
                break;
            }
            q = D_70003B6C[0];
            q[0xA] = 0;
            q = D_70003B6C[0];
            q[9] = q[9] + 1;
            break;
        }
        break;
    case 9:
        *st = *st + 1;
        h = D_00275C70;
        n1 = *(int *)(h + 4);
        n2 = *(int *)(h + 0x14);
        n3 = *(int *)(h + 8);
        n1 += n2;
        D_0028A744[0] = D_0028A740[0] + (n3 - n2);
        func_00200780(D_0028A488, D_0028A740[0], n1, n3 - n2);
        break;
    case 10:
        res = func_00200730();
        if (res != 0) {
            if (res == 1) {
                D_0028A748[0] = D_0028A744[0];
                q = D_70003B6C[0];
                q[9] = q[9] + 1;
            } else {
                q = D_70003B6C[0];
                q[9] = q[9] - 1;
            }
        }
        break;
    case 11:
        h = D_00275C70;
        total = 0;
        base = *(unsigned short *)(h + 0xC) + *(unsigned short *)(h + 0xE);
        n = *(unsigned int *)(h + 0x10);
        if (n != 0) {
            for (i = 0; i < n; i++) {
                func_00200830(D_0028A740[0] + total);
                total += *(int *)(D_00275C70 + (base + i) * 8 + 0x24);
            }
        }
        cnt2 = *(unsigned int *)(D_00275C70 + 0x1C);
        if (cnt2 != 0) {
            t2 = (unsigned int *)(D_00275C70 + (base + n) * 8 + 0x20);
            for (j2 = 0; j2 < cnt2; j2++) {
                w2 = *t2;
                D_0028A490[w2 >> 24] = (unsigned char *)((w2 & 0xFFFFFF) + (unsigned int)D_0028A740[0]);
                t2++;
            }
        }
        q = D_70003B6C[0];
        q[8] = 0x63;
        q = D_70003B6C[0];
        q[9] = 0;
        break;
    }
}
