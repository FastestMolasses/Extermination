// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.49% NEARMISS).
// Fixes read from the original instructions:
// - sub_BASCUS_97112_DS00_00_EX_ico takes the task record: the original leaves p in
//   $a0 for that call (its definition in src/ is `int (int *e)`), which is also why
//   the state byte is loaded into $a1. The old `(void)` extern was the "coloring wall".
// - Steps 0-3 end in `return`, not `break`: that restores the filled dispatch and
//   guard delay slots under 2.3.3 (no 991202 needed).
// - The descriptor/buffer globals are arrays (`D_x[]`, used without `&`), which
//   puts the slot-descriptor index computation where the original has it.
// - The slot-scan end test is `>= 5` (not `> 4`), keeping the compare out of $at.
// - The task record is a struct with a 12-byte SaveRec array at +0x74; the
//   record's int store only gets the original's index-first address add as a
//   struct member access.

// SEMANTICS: save-data / memory-card boot sequence, step-driven on the state byte
// p[4] of the task record p (a 5-pass loop driven by the counter p[0x68]).
//   p[3]     = outer task state (set to 3 when the whole sequence finishes)
//   p[4]     = this state machine's step (0..4)
//   p[5]     = cleared on completion
//   p[0xA]   = index of the "current" save slot
//   p[0x48]  = port, p[0x4C] = slot -- forwarded to every async step
//   p[0x64]  = per-step async sub-state, reset to 0 on each step advance
//   p[0x68]  = save-slot iterator, 0..4
//   p[0x6C+i]      = "slot i present" flag byte
//   p[0x74 + i*12] = 12-byte per-slot header record (6 bytes copied from the
//                    file header D_00810700..05, plus a word from D_00810750)
// step 0: func_001FEDD0() opens the card with descriptor D_002738E0; on success -> 1.
// step 1: func_00123168() builds the icon/param block at D_00821980 from template
//         D_002738F8, forces D_008219A0 = 2.0f, then func_001FE6B0() writes the 0x48-byte
//         header via descriptor D_00273910; on completion (== 1) -> 2.
// step 2: sub_BASCUS_97112_DS00_00_EX_ico(p) writes the icon file; on 1 -> 3.
// step 3: func_001FE6B0() writes the 0x3C4-byte block from D_00266B30 via D_00273940;
//         on 1 -> 4 and start the slot scan (p[0x68] = 0).
// step 4: for each slot p[0x68]: if it is the ACTIVE slot (== p[0xA]) recompute the
//         checksum over the live block D_00810700 and read from there, otherwise pull
//         the block back from scratchpad 0x70000000 into the staging buffer D_008219D0
//         (func_001AF220 refreshes it first). func_001FE6B0() then reads the 0x640-byte
//         save through the per-slot descriptor D_00267060[slot * 0x40]. On success (1),
//         if this was the active slot, publish its presence flag and cache its 12-byte
//         header record; then reset the sub-state, advance the slot, and when all 5
//         slots are done reset p[3]/p[4]/p[5] and finish. Results 0 and -1 just retry.
extern int func_001FEDD0(int, int, void *);
extern int func_001FE6B0(int *, int, int, void *, void *, int);
extern void func_00123168(void *, void *);
extern int sub_BASCUS_97112_DS00_00_EX_ico(int *);
extern void func_002284F0(unsigned char *);
extern void func_001AF220(void);
extern void block_copy(unsigned char *, unsigned char *, int);

extern unsigned char D_00266B30[];
extern unsigned char D_00267060[];
extern unsigned char D_002738E0[];
extern unsigned char D_002738F8[];
extern unsigned char D_00273910[];
extern unsigned char D_00273940[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810703;
extern unsigned char D_00810704;
extern unsigned char D_00810705;
extern int D_00810750;
extern unsigned char D_00821980[];
extern int D_008219A0;
extern unsigned char D_008219D0[];

typedef struct {
    unsigned char hdr[8];
    int w;
} SaveRec;

typedef struct {
    unsigned char pad0[3];
    unsigned char state;      /* 0x03 */
    unsigned char step;       /* 0x04 */
    unsigned char flag5;      /* 0x05 */
    unsigned char pad6[4];
    unsigned char cur;        /* 0x0A */
    unsigned char padB[0x3D];
    int port;                 /* 0x48 */
    int slot;                 /* 0x4C */
    unsigned char pad50[0x14];
    int sub;                  /* 0x64 */
    int iter;                 /* 0x68 */
    unsigned char present[8]; /* 0x6C */
    SaveRec rec[5];           /* 0x74 */
} SaveTask;

void sub_BASCUS_97112_DS00_00_EX_DATA_00(SaveTask *p) {
    unsigned char *src;
    unsigned char *dst;
    int len;
    void *tbl;
    unsigned char n;
    int r;

    switch (p->step) {
    case 0:
        if (func_001FEDD0(p->port, p->slot, D_002738E0) == 0) {
            p->step = 1;
            p->sub = 0;
        }
        return;
    case 1:
        func_00123168(D_00821980, D_002738F8);
        D_008219A0 = 0x40000000;
        tbl = D_00273910;
        if (func_001FE6B0(&p->sub, p->port, p->slot, tbl, D_00821980, 0x48) == 1) {
            p->step = 2;
            p->sub = 0;
        }
        return;
    case 2:
        if (sub_BASCUS_97112_DS00_00_EX_ico((int *)p) == 1) {
            p->step = 3;
            p->sub = 0;
        }
        return;
    case 3:
        tbl = D_00273940;
        if (func_001FE6B0(&p->sub, p->port, p->slot, tbl, D_00266B30, 0x3C4) == 1) {
            p->step = 4;
            p->iter = 0;
            p->sub = 0;
        }
        return;
    case 4:
        if (p->iter == p->cur) {
            func_002284F0(&D_00810700);
            src = &D_00810700;
        } else {
            func_001AF220();
            dst = D_008219D0;
            len = 0x640;
            block_copy(dst, (unsigned char *)0x70000000, len);
            src = D_008219D0;
        }
        tbl = &D_00267060[p->iter * 0x40];
        r = func_001FE6B0(&p->sub, p->port, p->slot, tbl, src, 0x640);
        switch (r) {
        case -1:
            break;
        case 0:
            break;
        case 1:
            if (p->iter == (n = p->cur)) {
                p->present[n] = 1;
                p->rec[p->cur].hdr[0] = D_00810700;
                p->rec[p->cur].hdr[1] = D_00810701;
                p->rec[p->cur].hdr[2] = D_00810702;
                p->rec[p->cur].hdr[3] = D_00810703;
                p->rec[p->cur].hdr[4] = D_00810704;
                p->rec[p->cur].hdr[5] = D_00810705;
                p->rec[p->cur].w = D_00810750;
            }
            p->sub = 0;
            p->iter = p->iter + 1;
            if (p->iter >= 5) {
                p->state = 3;
                p->step = 0;
                p->flag5 = 0;
                p->sub = 0;
            }
            break;
        }
        break;
    }
}
