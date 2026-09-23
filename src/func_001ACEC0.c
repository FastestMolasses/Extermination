// COMPILER: mwcc233
// CFLAGS: -O4,p
//
// The GAME TASK installed in frame-task slot 0. On entry stores 2 to scratchpad
// byte 0x70003B90, then dispatches on the task slot's state byte +8 (slot ptr
// in spad 0x70003B6C):
//   0: if func_001AD1A0() done: +8 = (gp-rel D_00275BE0 == 0) ? 1 : 2,
//      clear +9/+0xA; then func_001D1EF0() unconditionally.
//   1: if func_001AD230(): +8 = 3, clear +9/+0xA/+0xB.
//   2: +8 = 3 (via the slot+8 pointer computed at entry), +9 = 5, clear +0xA/+0xB.
//   3: func_001AD250() (the sub-state machine).
//
// The slot pointer is the relocated scratchpad extern D_70003B6C (see
// tools/match/spad_symbolize.py): the original re-forms its %hi per access, so
// a literal address let mwcc speculate a CSE'd scratchpad upper-half load into the case-0
// `bnez` delay slot (the former 97.50% NEARMISS residual). It is over-declared
// as an array (idiom #20) so it stays absolute while the byte D_00275BE0 stays
// gp-relative under the default -O4,p small-data threshold.

extern int func_001AD1A0(void);
extern int func_001AD230(void);
extern void func_001AD250(void);
extern void func_001D1EF0(void);
extern unsigned char D_00275BE0;
extern char * volatile D_70003B6C[4];
void func_001ACEC0(void) {
    char *slot;
    unsigned char *statep;
    slot = D_70003B6C[0];
    statep = (unsigned char *)(slot + 8);
    *(volatile unsigned char *)0x70003B90 = 2;
    switch (*(unsigned char *)(slot + 8)) {
    case 0:
        if (func_001AD1A0()) {
            if (D_00275BE0 == 0)
                *(unsigned char *)(D_70003B6C[0] + 0x8) = 1;
            else
                *(unsigned char *)(D_70003B6C[0] + 0x8) = 2;
            *(unsigned char *)(D_70003B6C[0] + 0x9) = 0;
            *(unsigned char *)(D_70003B6C[0] + 0xA) = 0;
        }
        func_001D1EF0();
        break;
    case 1:
        if (func_001AD230()) {
            *(unsigned char *)(D_70003B6C[0] + 0x8) = 3;
            *(unsigned char *)(D_70003B6C[0] + 0x9) = 0;
            *(unsigned char *)(D_70003B6C[0] + 0xA) = 0;
            *(unsigned char *)(D_70003B6C[0] + 0xB) = 0;
        }
        break;
    case 2:
        *statep = 3;
        *(unsigned char *)(D_70003B6C[0] + 0x9) = 5;
        *(unsigned char *)(D_70003B6C[0] + 0xA) = 0;
        *(unsigned char *)(D_70003B6C[0] + 0xB) = 0;
        break;
    case 3:
        func_001AD250();
        break;
    }
}
