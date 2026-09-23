// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching fix round, NEARMISS 60.44% -> 100%). The old header
// called the residual a register-allocation wall that C cannot reach. It was
// the same source shape as the sibling func_001FD580:
//  - arg0 is unsigned. The target keeps arg0 in $s2 and recomputes
//    arg0 + 1 + n after the call (0x001FD75C `addiu v0,s2,1`, 0x001FD760
//    `addu v0,v0,s0`). With a signed arg0, mwcc 2.3.3 keeps a running sum in
//    a caller-saved register instead (3 saved registers, frame 0x30 against
//    the target's 4 and 0x40). The only caller, func_001FDB80, passes the
//    +0x34 field with bit 31 cleared (0x001FDCB8 dsll32 / 0x001FDCC4 dsrl32
//    by 1), which fits an unsigned index.
//  - The scan is a for-loop whose condition indexes the table. The target
//    branches straight to the test (0x001FD708) and computes the entry
//    address only there (0x001FD730..0x001FD738).
// Declaring `int n` before `base` also matches 100%.
//
// Semantics (target 0x001FD6A0): base = D_00264DD4[D_00810700] is the current
// state's array of 8-byte entries (+2 short id, -1 = empty; +5 flags byte).
// If entry arg0 has flags == 1, set *arg1 = -1 and return 0. Otherwise scan
// entries arg0 + 1 + n for n = 0, 1, ... while the id is -1. If such a skipped
// entry has nonzero flags, set *arg1 = -1 and return 0. At the first entry
// with a real id: D_008106F5 = 2, call func_001FA5A0(id), set
// *arg1 = arg0 + 1 + n and return 1. func_001FD580 scans the same table
// starting at entry arg0 itself.

extern unsigned char D_00810700;
extern char D_008106F5;
extern char *D_00264DD4[];
extern int func_001FA5A0(int);

typedef struct {
    short frame;            /* +0 */
    short id;               /* +2: -1 = empty slot */
    unsigned char f4;       /* +4 */
    unsigned char flags;    /* +5 */
    unsigned char pad6[2];  /* +6 */
} Entry8;

int func_001FD6A0(unsigned int arg0, int *arg1) {
    Entry8 *base;
    int n;

    base = (Entry8 *)D_00264DD4[D_00810700];
    if (base[arg0].flags == 1) {
        *arg1 = -1;
        return 0;
    }
    for (n = 0; base[arg0 + 1 + n].id == -1; n++) {
        if (base[arg0 + 1 + n].flags != 0) {
            *arg1 = -1;
            return 0;
        }
    }
    D_008106F5 = 2;
    func_001FA5A0(base[arg0 + 1 + n].id);
    *arg1 = arg0 + 1 + n;
    return 1;
}
