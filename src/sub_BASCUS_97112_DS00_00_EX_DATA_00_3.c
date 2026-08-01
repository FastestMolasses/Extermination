// NEARMISS sub_BASCUS_97112_DS00_00_EX_DATA_00_3  (vram 0xASCUS_97112_DS00_00_EX_DATA_00_3, 0x2B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.94% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// ONE instruction: commutative addu operand order. Target `addu v1,v0,s1` (offset+base) vs mwcc `addu v1,s1,v0` (base+offset) on the p+n*12 record-pointer CSE. Everything else (176 instructions) is byte-identical: schedule, regalloc, branch-likely, dead-const rematerialization all match. mwcc canon...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Memory-card save-slot ENUMERATION state machine (one tick per call).
// arg0 is the browser context: +0x05 = state, +0x06 = slot cursor (0..4),
// +0x48/+0x4C = card port/slot, +0x58 = close/finalise work area,
// +0x64 = per-operation step counter (reset between slots),
// +0x68 = count of slots found, +0x6C.. = 5-byte "slot present" flags,
// +0x74 + 12*n = the 12-byte per-slot record (6 header bytes, a flag byte at
// +0x7A and the play-time tick count at +0x7C) consumed by the slot-list
// renderer sub__02d_02d_02d.
// D_00822050.. is the 0x640-byte staging buffer the card read fills: bytes
// +0x00..+0x05 are the save-name/id header, +0x0A a flag, +0x0F the record
// type (1 = a real Extermination save) and +0x50 the play-time word.
//
// state 0: clear the present-flags and the 0x480-byte directory buffer, reset
//          the counters, advance to state 1.
// state 1: open the card directory (func_001FEC20); on success validate it
//          (func_00228C90) -- a validation failure returns 3 (fatal), success
//          advances to state 2. A negative open result just retries next tick.
// state 2: read slot p[6]'s header (func_001FE7C0). 1 = complete: if the
//          record type is 1, verify it (func_00228C50, failure -> 3), mark the
//          slot present, copy the header + play time into the slot record and
//          bump the found count; either way reset the step counter and advance
//          the cursor. 0 = still busy. -1 = read error -> func_001FE8D0(), 3.
//          Once the cursor passes slot 4, advance to state 3.
// state 3: close the card (func_001FECB0). 0 = closed cleanly -> notify
//          (func_00225CF0) and return 1. -1 = still busy -> 0. Anything else is
//          an error -> notify, latch D_00275C58 and return 2.
// Returns 0 while still working.
extern void func_00121A28(void *p, int v, int n);
extern int func_001FEC20(int a, int b, void *c, int d, int e, void *f);
extern int func_001FE7C0(void *a, int b, int c, void *d, void *e, int f);
extern void func_001FE8D0(void);
extern int func_001FECB0(int a, int b, void *c, int d, int e);
extern void func_00225CF0(void *p, int a, int b);
extern int func_00228C50(void *p);
extern int func_00228C90(int h);

extern char D_00267060[];
extern char D_00273980[];
extern int D_00275C58;
extern unsigned char D_00821500[];
extern unsigned char D_00822050[8];
extern unsigned char D_00822051[8];
extern unsigned char D_00822052[8];
extern unsigned char D_00822053[8];
extern unsigned char D_00822054[8];
extern unsigned char D_00822055[8];
extern unsigned char D_0082205A[8];
extern unsigned char D_0082205F[8];
extern int D_008220A0[2];

int sub_BASCUS_97112_DS00_00_EX_DATA_00_3(unsigned char *p) {
    int r;
    int n;
    unsigned char v;
    unsigned char *q;

    switch (p[5]) {
    case 0:
        func_00121A28(p + 0x6C, 0, 8);
        func_00121A28(D_00821500, 0, 0x480);
        p[5] = 1;
        p[6] = 0;
        *(int *)(p + 0x68) = 0;
        *(int *)(p + 0x64) = 0;
        break;
    case 1:
        r = func_001FEC20(*(int *)(p + 0x48), *(int *)(p + 0x4C), D_00273980, 0, 0x12, D_00821500);
        if (r >= 0) {
            if (func_00228C90(r) != 0) {
                p[5] = 2;
            } else {
                return 3;
            }
        }
        break;
    case 2:
        n = p[6];
        if (n < 5) {
            r = func_001FE7C0(p + 0x64, *(int *)(p + 0x48), *(int *)(p + 0x4C),
                              D_00267060 + (n << 6), D_00822050, 0x640);
            switch (r) {
            case -1:
                func_001FE8D0();
                return 3;
            case 0:
                break;
            case 1:
                if (D_0082205F[0] == 1) {
                    if (func_00228C50(D_00822050) != 0) {
                        return 3;
                    }
                    p[0x6C + n] = 1;
                    v = D_00822050[0];
                    /* idiom-19: the first record store is written with the
                     * address inlined and `q` re-derived afterwards, so mwcc
                     * schedules the header load ahead of the address chain the
                     * way CodeWarrior did. Both spellings address p + n*12. */
                    *(unsigned char *)(p + n * 12 + 0x74) = v;
                    q = (unsigned char *)((int)p + n * 12);
                    q[0x75] = D_00822051[0];
                    q[0x76] = D_00822052[0];
                    q[0x77] = D_00822053[0];
                    q[0x78] = D_00822054[0];
                    q[0x79] = D_00822055[0];
                    q[0x7A] = D_0082205A[0];
                    *(int *)(q + 0x7C) = D_008220A0[0];
                    *(int *)(p + 0x68) = *(int *)(p + 0x68) + 1;
                }
                *(int *)(p + 0x64) = 0;
                p[6] = p[6] + 1;
                break;
            }
            return 0;
        } else {
            p[5] = 3;
        }
        break;
    case 3:
        r = func_001FECB0(*(int *)(p + 0x48), *(int *)(p + 0x4C), p + 0x58, 0, 0);
        switch (r) {
        case -1:
            return 0;
        case 0:
            func_00225CF0(p, 5, 7);
            return 1;
        default:
            func_00225CF0(p, 5, 7);
            D_00275C58 = 1;
            return 2;
        }
    }
    return 0;
}
