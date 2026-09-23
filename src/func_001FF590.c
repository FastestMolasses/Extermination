// func_001FF590 -- byte-matched from C (objdiff 100%).
// Jump-table dispatcher; the local .rodata table is pinned at its original
// address (tools/decomp/rodata_pin.py). Promoted from NEARMISS in round 6
// (2026-09-23): 0x70003B6C is the relocated scratchpad extern (idiom-32); case 0
// reads the transfer size as unsigned int; case 1 stages the base offset in r
// before adding the chunk offset (permuter shape) and forms the chunk pointer
// through an int sum, which orders the addu operands like the target.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: per-frame step of the streaming-audio / cutscene-track state machine.
// The whole machine lives in the scratchpad block whose pointer sits at 0x70003B6C:
// byte +0x0B is the state, halfword +0x14 a remaining-chunk counter and halfword
// +0x16 the current chunk index. D_00275C70 points at the descriptor for the track
// being played: +0x0C = chunk count, +0x0E = total chunks, +0x04 = a base offset and
// the 8-byte pairs from +0x20 give each chunk's (offset, length). D_0028A490[] is the
// per-voice stream handle table, D_0028A488 the shared transfer descriptor.
//   state 0: first call. With arg1 == 0 (single-shot) it kicks one transfer covering
//            the whole track and jumps to state 4; otherwise it latches the chunk
//            counter/index and falls through into state 1. Returns 1 (= done/idle)
//            when the descriptor is empty.
//   state 1: kick the transfer for the current chunk, then advance to state 2.
//   state 2: poll func_00200730 - 1 = chunk finished (bump chunk index, go to state 3),
//            other non-zero = error (step back a state), 0 = still busy.
//   state 3: retire the chunk via func_00200830; when the counter hits zero reset to
//            state 0 and report 1 (finished), otherwise go back to state 1.
//   state 4: poll func_00200730 for the single-shot transfer - 1 advances to state 5,
//            any other non-zero resets to state 0.
//   state 5: hand the handle to func_001FB370; on success store the new handle,
//            reset to state 0 and report 1 (finished).
// Any other state value, and every "still working" path, returns 0.

extern unsigned char *D_70003B6C[16];               /* PS2 scratchpad @ 0x70003B6C */

extern int func_00200730();
extern int func_00200780(int *, int, int, int);    /* really (void *file, void *buf, int offset, int size) */
extern int func_00200830(int);
extern int func_001FB370(int);

extern unsigned char *D_00275C70;
extern int D_0028A488[2];
extern int D_0028A490[];                            /* really char *[]: per-voice stream handles */

/* the state block pointer parked in scratchpad */
#define ST (D_70003B6C[0])

int func_001FF590(int arg0, int arg1)
{
    unsigned char *q;
    int r;

    switch (ST[0xB]) {
    case 0:
        if (arg1 == 0) {
            if (*(unsigned short *)(D_00275C70 + 0xC) == 0) {
                return 1;
            }
            func_00200780(D_0028A488, D_0028A490[arg0],
                          *(int *)(D_00275C70 + 0x20) + *(int *)(D_00275C70 + 4),
                          *(unsigned int *)(D_00275C70 + 0x24));
            ST[0xB] = 4;
            break;
        }
        *(unsigned short *)(ST + 0x14) = *(unsigned short *)(D_00275C70 + 0xE);
        if (*(unsigned short *)(ST + 0x14) == 0) {
            return 1;
        }
        *(unsigned short *)(ST + 0x16) = *(unsigned short *)(D_00275C70 + 0xC);
        ST[0xB]++;
        /* fallthrough */
    case 1:
        /* matching device: the (int) round trip only orders the addu operands;
           q is really D_00275C70 + idx * 8 (unsigned char *) */
        q = (unsigned char *)((*(unsigned short *)(ST + 0x16) << 3) + (int)D_00275C70);
        r = *(int *)(D_00275C70 + 4);
        r = *(int *)(q + 0x20) + r;
        func_00200780(D_0028A488, D_0028A490[arg0], r, *(int *)(q + 0x24));
        ST[0xB]++;
        break;
    case 2:
        r = func_00200730();
        if (r != 0) {
            if (r == 1) {
                (*(unsigned short *)(ST + 0x16))++;
                ST[0xB]++;
            } else {
                ST[0xB]--;
            }
        }
        break;
    case 3:
        func_00200830(D_0028A490[arg0]);
        if (--(*(unsigned short *)(ST + 0x14)) == 0) {
            ST[0xB] = 0;
            return 1;
        }
        ST[0xB] = 1;
        break;
    case 4:
        r = func_00200730();
        if (r != 0) {
            if (r == 1) {
                ST[0xB]++;
            } else {
                ST[0xB] = 0;
            }
        }
        break;
    case 5:
        r = func_001FB370(D_0028A490[arg0]);
        if (r != 0) {
            D_0028A490[arg0] = r;
            ST[0xB] = 0;
            return 1;
        }
        break;
    }
    return 0;
}
