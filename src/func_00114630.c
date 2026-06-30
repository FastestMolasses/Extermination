// NEARMISS func_00114630  (vram 0x00114630, 0xD8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.93% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall (confirmed s84): body byte-identical (216/216, 95.93%); two deterministic adjacent-op swaps remain -- addiu s0,%lo(D_0027B5C0) vs sw a0 ordering at 0x48, and the two func_0010DFD8 arg-moves (move a0,a3 / move a1,a2) at 0x50. Permuter ran twice full-window, never beat base score 80.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D68;
extern int D_0027B0C0[];
extern int D_0027B140[];
extern int D_0027B5C0[];
extern int D_0027C680[];
extern void func_001145A0(void);
extern void func_0010DFD8(int a0, int a1);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, void *a7, void *a8);

int func_00114630(int a0, int a1, int a2) {
    int r;

    if (D_00241D68 != 0) {
        return D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }
    D_0027B140[0] = a0;
    D_0027B140[6] = a1;
    D_0027B140[3] = a2;
    D_0027B140[7] = (int)D_0027B5C0;
    func_0010DFD8(a1, a2);
    func_0010DFD8((int)D_0027B5C0, 0xC0);
    r = func_0010E8A8(D_0027B0C0, 5, 1, D_0027B140, 0x30, D_0027C680, 4,
                      func_001145A0, D_0027B5C0);
    if (r == 0) {
        D_00241D68 = 5;
    }
    return r;
}
