// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching fix round, NEARMISS 91.21% -> 100%). Three source
// changes, each checked against the target:
//  1. The entry id is the short at +2 of each 8-byte entry (target 0x001FD654
//     `lh v1,0x2(a0)` in the loop test and the call argument `lh a0,0x2(a0)`
//     at 0x001FD670). The old C read +0. The old header called the stream 1:1
//     with the target; it was not (73 instructions against the target's 70).
//  2. The phase-2 scan is a rotated loop. The target branches straight to the
//     test at 0x001FD620 and computes the entry address only there
//     (0x001FD648..0x001FD650), so it is a for-loop whose condition indexes
//     the table. The old "e = ...; while" form added a copy before the loop.
//  3. arg0 is unsigned. The target recomputes arg0 + n after the call
//     (0x001FD674 `addu v0,s2,s0`) instead of reusing the loop's sum. With a
//     signed arg0, mwcc 2.3.3 CSEs the sum into a saved register. Declaring
//     `i` second (a declaration-order sweep of the six locals) gives the
//     target's register colouring (key t0, base a1, i a0, pa a2, pb a3).
//
// Semantics (target 0x001FD580): a two-phase lookup keyed by the state byte
// D_00810700. base = D_00264DD4[key] is that state's array of 8-byte entries.
// Phase 1 walks the 16-byte table D_0026EC60 until word +0 is -1. If an entry
// has word +0 == key and word +8 == arg0, the pair is already claimed: clear
// D_008106F4, set *arg1 = -1 and return 2. Phase 2 scans base[arg0 + n] for
// n = 0, 1, ... while the entry's id is -1. If such a skipped entry has a
// nonzero flags byte (+5), set *arg1 = -1 and return 0. At the first entry
// with a real id: D_008106F5 = 2, call func_001FA5A0(id), set *arg1 = arg0 + n
// and return 1. The +0 short is the entry's frame and +5 its flags byte, as
// read by the caller func_001FD790.

extern unsigned char D_00810700;
extern int D_00264DD4[];
extern int D_0026EC60[];
extern unsigned char D_008106F4;
extern unsigned char D_008106F5;
extern void func_001FA5A0(int a);

typedef struct {
    short frame;            /* +0 */
    short id;               /* +2: -1 = empty slot */
    unsigned char f4;       /* +4 */
    unsigned char flags;    /* +5: nonzero stops the scan */
    unsigned char pad6[2];  /* +6 */
} Entry8;

int func_001FD580(unsigned int arg0, int *arg1) {
    unsigned char key;
    int i;
    int *base;
    int *pa;
    int *pb;
    int n;

    key = D_00810700;
    base = (int *)D_00264DD4[key];
    pa = D_0026EC60;
    pb = D_0026EC60;
    i = 0;
    while (*pb != -1) {
        if (key == *pa) {
            if (arg0 == pa[2]) {
                D_008106F4 = 0;
                *arg1 = -1;
                return 2;
            }
        }
        pa += 4;
        pb += 4;
        i++;
    }

    for (n = 0; ((Entry8 *)base)[arg0 + n].id == -1; n++) {
        if (((Entry8 *)base)[arg0 + n].flags != 0) {
            *arg1 = -1;
            return 0;
        }
    }
    D_008106F5 = 2;
    func_001FA5A0(((Entry8 *)base)[arg0 + n].id);
    *arg1 = arg0 + n;
    return 1;
}
