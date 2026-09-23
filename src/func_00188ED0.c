// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x700038CC
//
// Byte-matched (objdiff 100%) from the former NEARMISS body. The scratchpad point at 0x700038CC is a relocated extern (idiom-32); its expected object is symbolized per file via // SPAD:.

//
// Copies the 64-byte matrix at D_008103D0[0]+0x90 into each of the first
// arg0[0xC] entries of the D_00275B40 table (+0x90) with copy_qw4(dst, src).
// Then, gated on the D_008106C7 enable flag: if the (D_008102B4, D_008102B5)
// state pair is in the accepted set (mode 1 with 0x1D / 0x1E..0x20, or mode 2
// with 0x17/0x18), it writes the point (3.6f, 0.5f, 0, 1.0f) to scratchpad
// 0x700038C0, transforms it by entry 0's matrix into arg0+0xB0
// (func_001026A0), computes a mode flag from D_008104A0 (0 for 0x31/0x34, else
// 1), and calls func_00187780(arg0, func_001B0070()&0x80 ? 0 : 1, mode).
// Otherwise it clears the D_008106C7 enable flag.
//
// m2-matching lane (93.47% -> 97.95%): the mode flag is an `||` if/else, which
// reproduces the target's dead `li s0,1` block after `b join`; the range test
// is `> 2U` (idiom-28, gives `sltiu $at`); and the table base is read into a
// local between the third and fourth scratchpad stores. Remaining: the target
// keeps the fourth store before the &D_700038C0 materialization and colors the
// base in $v1, where mwcc gives $a1 and sinks the store (7 rows).

extern float D_700038CC[16];                        /* PS2 scratchpad @ 0x700038CC */

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
    int *b;

    for (i = 0; i < (int)arg0[0xC]; i++) {
        copy_qw4(D_00275B40[i] + 0x90, D_008103D0[0] + 0x90);
    }
    if (D_008106C7[0] != 0) {
        if (D_008102B4[0] == 1) {
            if (D_008102B5[0] != 0x1D && (unsigned int)(D_008102B5[0] - 0x1E) > 2U) {
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
            b = D_00275B40;
            D_700038CC[0] = 1.0f;
            func_001026A0(arg0 + 0xB0, *b + 0x90, &D_700038C0[0]);
            if (D_008104A0[0] == 0x31 || D_008104A0[0] == 0x34) {
                mode = 0;
            } else {
                mode = 1;
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
