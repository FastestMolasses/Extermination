// NEARMISS func_0010E6F8  (vram 0x0010E6F8, 0x11C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 56.27% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely + return-value coloring wall. Two non-source-crackable artifacts: (1) the post-func_0010E270 null check — expected `beqz s0,end` (plain) with `addiu v0,-1` in the delay slot, but with early-return C our compiler emits a forward `bnezl s0` (branch-likely-skip), which the s84 notes confirm ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void *func_0010E270(void *);
extern int EndOfHeap(void *);
extern int func_0010DE38(unsigned int, void *, int, int, int, int);
extern int SignalSema(int);
extern int RFU063(int);

extern unsigned char D_00278BC0[];

int func_0010E6F8(int a0, int a1, int a2) {
    void *blk;
    int sema[6];
    int r;
    int rv;

    *(int *)(a0 + 0x10) = 0;
    *(int *)(a0 + 0x24) = 0;
    blk = func_0010E270(D_00278BC0);
    rv = -1;
    if (blk != 0) {
        *(int *)(a0 + 0x0) = (int)blk;
        *(int *)(a0 + 0x4) = *(int *)((char *)blk + 0x18);
        *(int *)((char *)blk + 0x20) = a1;
        *(int *)((char *)blk + 0x14) = (int)blk;
        *(int *)((char *)blk + 0x1C) = a0;
        if (a2 & 1) {
            *(int *)(a0 + 0x8) = -1;
            r = func_0010DE38(0x80000009, blk, 0x40, 0, 0, 0);
            rv = r ? 0 : -2;
        } else {
            sema[2] = 0;  /* 0x8($sp) */
            sema[1] = 1;  /* 0x4($sp) */
            *(int *)(a0 + 0x8) = EndOfHeap((void *)sema);
            if (*(int *)(a0 + 0x8) < 0) {
                rv = -3;
            } else if (func_0010DE38(0x80000009, blk, 0x40, 0, 0, 0) == 0) {
                rv = -2;
            } else {
                SignalSema(*(int *)(a0 + 0x8));
                RFU063(*(int *)(a0 + 0x8));
                rv = 0;
            }
        }
    }
    return rv;
}
