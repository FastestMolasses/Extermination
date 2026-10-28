// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.48% NEARMISS).
// Fixes read from the original instructions:
// - The inner result switch's cases end in `return` (the outer cases keep `break`):
//   that restores the branch-likely state-0 entry with the staging %hi in its slot
//   and the shared constant 3 in the first dispatch delay slot.
// - The record is the SaveTask struct of the byte-matched twin
//   sub_BASCUS_97112_DS00_00_EX_DATA_00 (12-byte SaveRec array at +0x74): the
//   per-slot word store only gets the original's index-first address add as a
//   struct member access (raw byte-offset arithmetic gives the base-first add).
//
// SEMANTICS: two-state per-frame step for an overlay record p, driven by the
// state byte p->step (+0x04). Any state other than 0 or 1 does nothing.
//   +0x03 state  = completion/notify byte
//   +0x04 step   = 0 = load in progress, 1 = finished -> reset
//   +0x05 flag5  = secondary flag, cleared on reset
//   +0x0A cur    = slot index, selects both a 64-byte descriptor in D_00267060
//                  and the destination 12-byte record in this task
//   +0x48 port, +0x4C slot = the two parameters handed to func_001FE6B0
//   +0x64 sub    = async transfer sub-state (also cleared on reset)
//   +0x6C + idx  = per-slot "present" byte
//   +0x74 + idx*12 = per-slot 12-byte record: header bytes 0..5, byte 6, word 8
// State 0: refresh the staging record at D_00810700 (func_002284F0), then run
//   func_001FE6B0(&p->sub, port, slot, &D_00267060[idx*64], &D_00810700, 0x640).
//   Its result is a 3-way status: 1 = complete, 0 and -1 = pending / error (both
//   just return). On 1 the destination record is zeroed (func_00121A28 = memset),
//   marked present, and filled from the staging record (bytes 0..5, then the word
//   at D_00810750 -> word 8, then the byte at D_0081070A -> byte 6, in exactly that
//   order), then the step advances to 1.
// State 1: teardown -- state = 3, step back to 0, flag5 = 0, sub-state cleared.
// The staging fields are referenced through separate D_008107xx externs (not
// one struct) because that is how the target's per-field %hi/%lo relocations
// are laid out.

extern void func_002284F0(unsigned char *);
extern int func_001FE6B0(int *, int, int, void *, void *, int);
extern void func_00121A28(void *, int, int);

extern unsigned char D_00267060[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810703;
extern unsigned char D_00810704;
extern unsigned char D_00810705;
extern unsigned char D_0081070A;
extern int D_00810750;

typedef struct {
    unsigned char hdr[8];
    int w;
} SaveRec;

typedef struct {
    unsigned char pad0[3];
    unsigned char state;      /* 0x03 */
    unsigned char step;       /* 0x04 */
    unsigned char flag5;      /* 0x05 */
    unsigned char pad6[4];
    unsigned char cur;        /* 0x0A */
    unsigned char padB[0x3D];
    int port;                 /* 0x48 */
    int slot;                 /* 0x4C */
    unsigned char pad50[0x14];
    int sub;                  /* 0x64 */
    int iter;                 /* 0x68 */
    unsigned char present[8]; /* 0x6C */
    SaveRec rec[5];           /* 0x74 */
} SaveTask;

void sub_BASCUS_97112_DS00_00_EX_DATA_00_2(SaveTask *p)
{
    int r;

    switch (p->step) {
    case 0:
        func_002284F0(&D_00810700);
        r = func_001FE6B0(&p->sub, p->port, p->slot,
                          &D_00267060[p->cur * 0x40], &D_00810700, 0x640);
        switch (r) {
        case -1:
            return;
        case 0:
            return;
        case 1:
            func_00121A28(&p->rec[p->cur], 0, 12);
            p->present[p->cur] = 1;
            p->rec[p->cur].hdr[0] = D_00810700;
            p->rec[p->cur].hdr[1] = D_00810701;
            p->rec[p->cur].hdr[2] = D_00810702;
            p->rec[p->cur].hdr[3] = D_00810703;
            p->rec[p->cur].hdr[4] = D_00810704;
            p->rec[p->cur].hdr[5] = D_00810705;
            p->rec[p->cur].w = D_00810750;
            p->rec[p->cur].hdr[6] = D_0081070A;
            p->step = 1;
            return;
        }
        break;
    case 1:
        p->state = 3;
        p->step = 0;
        p->flag5 = 0;
        p->sub = 0;
        break;
    }
}
