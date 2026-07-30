// NEARMISS func_00200360  (vram 0x00200360, 0x3A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// The jr-table dispatch itself matches 100% (jtbl_002732B0 reloc included) and cases 1, 3, 4 and 6 are byte-identical. 25 residual instruction rows in three classes: (1) IDIOM-13 lui-in-branch-delay-slot, 3 sites, ~11 rows — the target leaves `nop` in the delay slots of `bne v0,v1,<else>` (case 1 @...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: 7-step state machine for a streaming/upload job.  The job context
// is the pointer parked in the scratchpad word at 0x70003B6C; ctx[9] is the
// step counter (the switch selector, dispatched through jtbl_002732B0),
// ctx[8] a completion code and ctx[0x0A] a sub-step counter.
//   0: if the header word at D_00289BC0 already equals D_00810700+4 the job is
//      already resident -> step = 2; otherwise step++ and kick off
//      func_00200780(D_0028A480, D_00289BC0, (D_00810700+4) << 11, 0x800).
//   1: poll func_00200730(); 1 -> step++, any other non-zero -> step--.
//   2: publish D_00289BC0 as the current record (D_00275C70).  If its word at
//      +0x18 is zero the set is empty: clear D_00810701 / D_00810704, set
//      ctx[8] = 0x63 (done) and reset step to 0.  Otherwise select sub-record
//      D_00810701 (stride 112, base +0x100), latch it in D_00810704, publish
//      it and step++.
//   3: two-phase func_001FF590(0xAC, phase) handshake driven by ctx[0x0A];
//      phase 0 then phase 1, each aborting the step while it returns 0.  On
//      completion ctx[0x0A] = 0 and step++.
//   4: step++, then compute the transfer window from the current record
//      (+4, +8, +0x14), advance the destination cursor
//      D_0028A744 = D_0028A740 + (rec+8 - rec+0x14) and issue
//      func_00200780(D_0028A488, D_0028A740, rec+4 + rec+0x14, size).
//   5: poll func_00200730(); 1 -> commit the cursor (D_0028A748 = D_0028A744)
//      and step++, any other non-zero -> step--.
//   6: fix-ups.  Walk the record's first table (count at +0x10, entries at
//      base+0x24 with stride 8) calling func_00200830 on each running offset,
//      then patch the second table (count at +0x1C, words at base+0x20):
//      each word's low 24 bits are relocated by D_0028A740 and stored into
//      D_0028A490 indexed by the word's top byte.  Finally ctx[8] = 0x63
//      (done) and step = 0.
//
// Modelling notes (do not change without re-measuring): the `extern` array
// sizes on the byte/word globals are over-declarations chosen so that
// -sdatathreshold 4 keeps them absolutely addressed while D_00275C70 stays
// %gp_rel; `volatile` on D_0028A740 / D_0028A744 reproduces the original's
// per-use reload of those two words.

extern int func_00200730();
extern int func_001FF590();
extern int func_00200780();
extern int func_00200830();

extern unsigned char D_00810700[8];
extern unsigned char D_00810701[8];
extern unsigned char D_00810704[8];
extern char D_00289BC0[0x1000];
extern char D_0028A480[8];
extern char D_0028A488[8];
extern int D_0028A490[256];
extern volatile int D_0028A740[2];
extern volatile int D_0028A744[2];
extern int D_0028A748[2];
extern char *D_00275C70;

void func_00200360(void) {
    char *m;
    unsigned char *p;
    unsigned char *st;
    int c;
    int d;
    int sum;
    int r;
    int base;
    int n;
    unsigned int i;
    int acc;
    int *w;
    int cnt;
    int t;
    unsigned int k;

    p = *(unsigned char **)0x70003B6C;
    st = p + 9;
    switch (p[9]) {
    case 0:
        c = D_00810700[0] + 4;
        if (*(int *)D_00289BC0 != c) {
            *st = *st + 1;
            func_00200780(D_0028A480, D_00289BC0, c << 11, 0x800);
        } else {
            *st = 2;
        }
        break;
    case 1:
        r = func_00200730();
        if (r != 0) {
            if (r == 1) {
                (*(unsigned char **)0x70003B6C)[9]++;
            } else {
                (*(unsigned char **)0x70003B6C)[9]--;
            }
        }
        break;
    case 2:
        m = D_00289BC0;
        D_00275C70 = m;
        if (*(int *)(m + 0x18) == 0) {
            D_00810701[0] = 0;
            D_00810704[0] = 0;
            p[8] = 0x63;
            (*(unsigned char **)0x70003B6C)[9] = 0;
        } else {
            c = D_00810701[0];
            D_00810704[0] = c;
            D_00275C70 = m + c * 112 + 0x100;
            *st = *st + 1;
        }
        break;
    case 3:
        switch (p[10]) {
        case 0:
            if (func_001FF590(0xAC, 0) == 0) {
                break;
            }
            (*(unsigned char **)0x70003B6C)[10]++;
            /* fall through */
        case 1:
            if (func_001FF590(0xAC, 1) == 0) {
                break;
            }
            (*(unsigned char **)0x70003B6C)[10] = 0;
            (*(unsigned char **)0x70003B6C)[9]++;
            break;
        }
        break;
    case 4:
        *st = *st + 1;
        m = D_00275C70;
        sum = *(int *)(m + 4) + *(int *)(m + 0x14);
        d = *(int *)(m + 8) - *(int *)(m + 0x14);
        D_0028A744[0] = D_0028A740[0] + d;
        func_00200780(D_0028A488, D_0028A740[0], sum, d);
        break;
    case 5:
        r = func_00200730();
        if (r != 0) {
            if (r == 1) {
                D_0028A748[0] = D_0028A744[0];
                (*(unsigned char **)0x70003B6C)[9]++;
            } else {
                (*(unsigned char **)0x70003B6C)[9]--;
            }
        }
        break;
    case 6:
        m = D_00275C70;
        acc = 0;
        base = *(unsigned short *)(m + 0xC) + *(unsigned short *)(m + 0xE);
        n = *(int *)(m + 0x10);
        if (n != 0) {
            for (i = 0; i < (unsigned int)n; i++) {
                func_00200830(D_0028A740[0] + acc);
                acc += *(int *)(D_00275C70 + ((base + (int)i) << 3) + 0x24);
            }
        }
        m = D_00275C70;
        cnt = *(int *)(m + 0x1C);
        if (cnt != 0) {
            w = (int *)(m + ((base + n) << 3) + 0x20);
            for (k = 0; k < (unsigned int)cnt; k++) {
                t = *w;
                D_0028A490[(unsigned int)t >> 24] = (t & 0xFFFFFF) + D_0028A740[0];
                w++;
            }
        }
        (*(unsigned char **)0x70003B6C)[8] = 0x63;
        (*(unsigned char **)0x70003B6C)[9] = 0;
        break;
    }
}
