// NEARMISS sub_BASCUS_97112_DS00_00_EX_DATA_00  (vram 0xASCUS_97112_DS00_00_EX_DATA_00, 0x2F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.49% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + downstream scheduling (NOT the clean-store idiom-13 nop; 2.3.3 is WORSE here — it leaves the dispatch slots empty while the target and 991202 fill them). Root cause is one coloring decision: the target loads the p[4] state byte into $a1, keeping $a0 live so the `paddub s0,a0,z...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
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
// step 2: sub_BASCUS_97112_DS00_00_EX_ico() writes the icon files; on 1 -> 3.
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
extern int sub_BASCUS_97112_DS00_00_EX_ico(void);
extern void func_002284F0(unsigned char *);
extern void func_001AF220(void);
extern void block_copy(unsigned char *, unsigned char *, int);

extern unsigned char D_00266B30;
extern unsigned char D_00267060[];
extern unsigned char D_002738E0;
extern unsigned char D_002738F8;
extern unsigned char D_00273910;
extern unsigned char D_00273940;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810703;
extern unsigned char D_00810704;
extern unsigned char D_00810705;
extern int D_00810750;
extern unsigned char D_00821980;
extern int D_008219A0;
extern unsigned char D_008219D0;

void sub_BASCUS_97112_DS00_00_EX_DATA_00(unsigned char *p) {
    unsigned char *src;
    unsigned char *dst;
    int len;
    void *tbl;
    unsigned char n;
    int r;

    switch (p[4]) {
    case 0:
        if (func_001FEDD0(*(int *)(p + 0x48), *(int *)(p + 0x4C), &D_002738E0) == 0) {
            p[4] = 1;
            *(int *)(p + 0x64) = 0;
        }
        break;
    case 1:
        func_00123168(&D_00821980, &D_002738F8);
        D_008219A0 = 0x40000000;
        tbl = &D_00273910;
        if (func_001FE6B0((int *)(p + 0x64), *(int *)(p + 0x48), *(int *)(p + 0x4C),
                          tbl, &D_00821980, 0x48) == 1) {
            p[4] = 2;
            *(int *)(p + 0x64) = 0;
        }
        break;
    case 2:
        if (sub_BASCUS_97112_DS00_00_EX_ico() == 1) {
            p[4] = 3;
            *(int *)(p + 0x64) = 0;
        }
        break;
    case 3:
        tbl = &D_00273940;
        if (func_001FE6B0((int *)(p + 0x64), *(int *)(p + 0x48), *(int *)(p + 0x4C),
                          tbl, &D_00266B30, 0x3C4) == 1) {
            p[4] = 4;
            *(int *)(p + 0x68) = 0;
            *(int *)(p + 0x64) = 0;
        }
        break;
    case 4:
        if (*(int *)(p + 0x68) == p[0xA]) {
            func_002284F0(&D_00810700);
            src = &D_00810700;
        } else {
            func_001AF220();
            dst = &D_008219D0;
            len = 0x640;
            block_copy(dst, (unsigned char *)0x70000000, len);
            src = &D_008219D0;
        }
        tbl = &D_00267060[*(int *)(p + 0x68) * 0x40];
        r = func_001FE6B0((int *)(p + 0x64), *(int *)(p + 0x48), *(int *)(p + 0x4C),
                          tbl, src, 0x640);
        switch (r) {
        case -1:
            break;
        case 0:
            break;
        case 1:
            if (*(int *)(p + 0x68) == (n = p[0xA])) {
                p[n + 0x6C] = 1;
                p[p[0xA] * 12 + 0x74] = D_00810700;
                p[p[0xA] * 12 + 0x75] = D_00810701;
                p[p[0xA] * 12 + 0x76] = D_00810702;
                p[p[0xA] * 12 + 0x77] = D_00810703;
                p[p[0xA] * 12 + 0x78] = D_00810704;
                p[p[0xA] * 12 + 0x79] = D_00810705;
                *(int *)(p[0xA] * 12 + p + 0x7C) = D_00810750;
            }
            *(int *)(p + 0x64) = 0;
            *(int *)(p + 0x68) = *(int *)(p + 0x68) + 1;
            if (*(int *)(p + 0x68) > 4) {
                p[3] = 3;
                p[4] = 0;
                p[5] = 0;
                *(int *)(p + 0x64) = 0;
            }
            break;
        }
        break;
    }
}
