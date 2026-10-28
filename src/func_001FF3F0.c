// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003B6C

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.94% NEARMISS).
// Fixes read from the original instructions:
//  - wrong body: the state-1 entry table is the latched descriptor D_00275C70
//    (desc + idx*8), not D_0028A488 (which is only the first call argument);
//  - wrong body: state 0 with a zero count returns 1, not 0;
//  - the scratchpad slot pointer 0x70003B6C is a relocated volatile extern at
//    every access (idiom-32, // SPAD:);
//  - state 0 reads the count through the chained `desc = D_00275C70 = ...`
//    value, so the loaded address register is reused;
//  - the state-3 "reset to 1" store goes through a fresh scratchpad read, not
//    the reused `slot` local.

// State machine for the active task slot (pointer in scratchpad 0x70003B6C),
// dispatched on the slot's state byte at +0xB (values 0..3). State 0: latches
// the sequence descriptor D_00289BC0 into gp-rel D_00275C70, copies its 0xE
// count into slot+0x14, and if nonzero zeroes the slot+0x16 index and advances
// the state (falling into state 1). State 1: indexes the latched descriptor D_00275C70 by
// slot+0x16<<3 (8-byte entries), calls func_00200780(D_0028A488, D_00275C74, ent[0x20]+desc[4], ent[0x24]) and
// advances the state. State 2: func_00200730() -> 0 stay, 1 advance + bump
// slot+0x16, else step the state back. State 3: func_00200830(D_00275C74),
// decrement slot+0x14; at 0 reset state to 0 and return 1, otherwise reset to 1.
// Returns 1 on the state-3 completion edge and when state 0 finds a zero
// count, else 0.
//
extern char *volatile D_70003B6C[16];      /* PS2 scratchpad @ 0x70003B6C */
extern char *D_00275C70;
extern char *D_00275C74;
extern char D_00289BC0[];
extern char D_0028A488[];

extern int func_00200780(void *file, void *buf, int offset, int size);
extern int func_00200730(void);
extern void func_00200830(char *a);

int func_001FF3F0(void) {
    char *slot;
    char *desc;
    char *ent;
    int r;
    unsigned int n;

    slot = D_70003B6C[0];
    switch (*(unsigned char *)(slot + 0xB)) {
    case 0:
        desc = D_00275C70 = D_00289BC0;
        *(unsigned short *)(slot + 0x14) = *(unsigned short *)(desc + 0xE);
        slot = D_70003B6C[0];
        if (*(unsigned short *)(slot + 0x14) == 0) {
            return 1;
        }
        *(short *)(slot + 0x16) = 0;
        slot = D_70003B6C[0];
        *(unsigned char *)(slot + 0xB) = *(unsigned char *)(slot + 0xB) + 1;
        /* fall through */
    case 1:
        slot = D_70003B6C[0];
        desc = D_00275C70;
        ent = (char *)((*(unsigned short *)(slot + 0x16) << 3) + (int)desc);
        func_00200780(D_0028A488,
                      D_00275C74,
                      *(int *)(ent + 0x20) + *(int *)(desc + 4),
                      *(int *)(ent + 0x24));
        slot = D_70003B6C[0];
        *(unsigned char *)(slot + 0xB) = *(unsigned char *)(slot + 0xB) + 1;
        break;
    case 2:
        r = func_00200730();
        if (r == 0) break;
        if (r == 1) {
            slot = D_70003B6C[0];
            *(unsigned char *)(slot + 0xB) = *(unsigned char *)(slot + 0xB) + 1;
            slot = D_70003B6C[0];
            *(unsigned short *)(slot + 0x16) = *(unsigned short *)(slot + 0x16) + 1;
            break;
        }
        slot = D_70003B6C[0];
        *(unsigned char *)(slot + 0xB) = *(unsigned char *)(slot + 0xB) - 1;
        break;
    case 3:
        func_00200830(D_00275C74);
        slot = D_70003B6C[0];
        n = *(unsigned short *)(slot + 0x14) - 1;
        *(short *)(slot + 0x14) = n;
        if ((n & 0xFFFF) == 0) {
            slot = D_70003B6C[0];
            *(unsigned char *)(slot + 0xB) = 0;
            return 1;
        }
        *(unsigned char *)(D_70003B6C[0] + 0xB) = 1;
        break;
    }
    return 0;
}
