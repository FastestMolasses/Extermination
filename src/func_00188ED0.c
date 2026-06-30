// NEARMISS func_00188ED0  (vram 0x00188ED0, 0x1BC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.47% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two genuine compiler artifacts downstream of correct logic: (1) one-slot scheduling of the gp-rel D_00275B40 load relative to the 4 float-constant stores; (2) sltiu $at vs $v1 register coloring on the (D_008102B5-0x1E)<3 range test. Both cascade branch offsets. Scheduling + regalloc-permutation c...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Copies a 0x90-byte qword block from each of arg0[0xC] entries of the
// D_00275B40 table into the global block at D_008103D0 (loop). Then, gated on
// the D_008106C7 enable flag: if the (D_008102B4, D_008102B5) state pair is in
// the accepted set (mode 1 with 0x1D / 0x1E..0x20, or mode 2 with 0x17/0x18),
// it writes a fixed transform (3.6f, 0.5f, 0, 1.0f) to the 0x700038C0 GS/VU
// region, runs func_001026A0(arg0+0xB0, *D_00275B40+0x90, &D_700038C0),
// computes a mode flag from D_008104A0 (0 for 0x31/0x34, else 1), and dispatches
// func_00187780(arg0, func_001B0070()&0x80 ? 0 : 1, mode). Otherwise it clears
// the D_008106C7 enable flag.
//
// Residual (sole deltas vs target): (1) a one-slot scheduling difference -- the
// target hoists `lw $v1,0($gp)` (the D_00275B40 base load) between the 3rd and
// 4th 0x700038C? stores, mwcc emits it after all four stores; (2) the range
// test `(unsigned)(D_008102B5-0x1E)<3` colors to `sltiu $at` in the target but
// `sltiu $v1` under mwcc. Both cascade the branch-target offsets. Scheduling +
// register-coloring permutation class -- permuter territory.
extern void copy_qw4(int, int);
extern void func_001026A0(unsigned char *, int, void *);
extern void func_00187780(unsigned char *, int, int);
extern int func_001B0070(void);
extern int *D_00275B40;
extern unsigned char D_008102B4[256];
extern unsigned char D_008102B5[256];
extern int D_008103D0[64];
extern unsigned char D_008104A0[256];
extern unsigned char D_008106C7[256];
extern int D_700038C0[64];

void func_00188ED0(unsigned char *arg0) {
    int i;
    int mode;

    for (i = 0; i < (int)arg0[0xC]; i++) {
        copy_qw4(D_00275B40[i] + 0x90, D_008103D0[0] + 0x90);
    }
    if (D_008106C7[0] != 0) {
        if (D_008102B4[0] == 1) {
            if (D_008102B5[0] != 0x1D && (unsigned int)(D_008102B5[0] - 0x1E) >= 3U) {
                goto check2;
            }
            goto setup;
        }
check2:
        if (D_008102B4[0] == 2 && (D_008102B5[0] == 0x17 || D_008102B5[0] == 0x18)) {
setup:
            *(float *)0x700038C0 = 3.6f;
            *(float *)0x700038C4 = 0.5f;
            *(int *)0x700038C8 = 0;
            *(float *)0x700038CC = 1.0f;
            func_001026A0(arg0 + 0xB0, *D_00275B40 + 0x90, &D_700038C0[0]);
            mode = 0;
            if (D_008104A0[0] != 0x31) {
                mode = 1;
                if (D_008104A0[0] == 0x34) {
                    mode = 0;
                }
            }
            if (func_001B0070() & 0x80) {
                func_00187780(arg0, 0, mode);
                return;
            }
            func_00187780(arg0, 1, mode);
        } else if (D_008106C7[0] != 0) {
            D_008106C7[0] = 0;
        }
    }
}
