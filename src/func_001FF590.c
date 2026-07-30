// NEARMISS func_001FF590  (vram 0x001FF590, 0x29C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Decode is fully verified against the .s (every case, every offset/width, the case-0 -> case-1 fallthrough, the beql default). The jtbl_00273240 dispatch itself MATCHES (build/jtblrodata is present and objdiff accepts the local jtbl reloc) - there is no dispatch residual. 94.204 with mwcc233 -O4,p...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
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

extern int func_00200730();
extern int func_00200780(int *, int, int, int);
extern int func_00200830(int);
extern int func_001FB370(int);

extern unsigned char *D_00275C70;
extern int D_0028A488[2];
extern int D_0028A490[];

/* the state block pointer parked in scratchpad */
#define ST (*(unsigned char **)0x70003B6C)

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
                          *(int *)(D_00275C70 + 0x24));
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
        q = (*(unsigned short *)(ST + 0x16) * 8) + D_00275C70;
        func_00200780(D_0028A488, D_0028A490[arg0],
                      *(int *)(q + 0x20) + *(int *)(D_00275C70 + 4),
                      *(int *)(q + 0x24));
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
