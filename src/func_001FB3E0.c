// NEARMISS func_001FB3E0  (vram 0x001FB3E0, 0x524 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.54% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) (was 86.64%; matching wave 2026-10-27).
// Fixes this wave: the loader state bytes/words at D_00282150..D_002821A7 are ONE struct
// (field accesses keep CW's store/load order); the handle table is a 2-D array
// D_00281D50[bucket][slot]; case 0 uses `state++`; the epilogue compare is written
// `total <= done`; dest is unsigned; no volatile reads are needed.
// Remaining differences: (a) the 20-byte copy of D_00264890 ends with its +0x10 word
// moved through a GPR in the original, through an FPR here (every struct/array/initialiser
// spelling tried gives the FPR copy); (b) in state 6 `src += entry size` loads its two
// operands into swapped registers (3 instructions).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: streaming/DMA loader state machine, one step per call, dispatched
//   through a 7-entry jump table on the state byte D_00282151.
//   arg0 = the stream descriptor: +0x08 = entry count, +0x0C = the total number
//   of records to process, +0x10 = the payload size, +0x18 = the payload offset,
//   +0x20 = the start of the entry array (16 bytes per entry).
//   D_002821A4 = the cursor into that entry array; each entry is
//   { +0x00 size, +0x04 dest offset, +0x08 bucket id }.
//   D_0028219C = the source pointer, D_00282190 = the current bucket,
//   D_00282194 = the scratch buffer handle from func_0010F8F8,
//   D_00282198 = the running destination address, D_00282159 = the records
//   completed so far, D_00282150 / D_00282151 = the outer/inner state bytes,
//   D_0028215C = a retry flag. D_00281D30[bucket] = how many handles that
//   bucket holds; D_00281D50 is the 0x50-byte-per-bucket handle table.
//   D_00275B18 / D_00275B1C are the shared retry countdown and busy flag.
//   D_00264890 is a 5-entry table of per-bucket base addresses, copied to a
//   local on entry and indexed by the bucket.
//   state 0: initialise -- clear the completed count, set the bucket to an
//            impossible 0x63, point the cursor past the entry array and the
//            source at arg0 + its payload offset.
//   state 1: if the entry's bucket changed, release every handle the old bucket
//            still holds (func_001195A8), reset its count and re-seat the
//            destination from the base table; then allocate the scratch buffer
//            (func_0010F8F8 on the entry size + 0x10) and clear the retry flag.
//   state 2: kick the read (func_001FB910); advance only when it accepts.
//   state 3: if the device is busy just advance, otherwise wait for
//            func_00119450 to report ready.
//   state 4: round the destination up to a 0x40 boundary, register the decoded
//            block (func_001194B8) into the bucket's handle slot and, unless it
//            failed (-1), arm the 0x50-step timeout and advance -- falling
//            through into state 5.
//   state 5: poll func_00119450 -- 1 = done (advance), negative = failure (roll
//            back to state 4 and release the handle), otherwise tick the
//            timeout down and flip the busy flag when it expires.
//   state 6: finish the entry -- publish it (func_001199F0 ... 100), free the
//            scratch buffer (func_0010F968), advance the destination and source
//            by the entry size, bump the bucket's handle count, step the cursor
//            to the next entry and count one record done; state returns to 1.
//   Every path ends in the shared epilogue: while fewer records are done than
//   arg0+0x0C, return 0; once they are all done, clear both state bytes and
//   return the 0x40-aligned end of the payload.
//
//   NOTE: the loader block is one struct at D_00282150 (outer/inner state bytes,
//   completed count, retry flag, bucket, scratch handle, destination, source,
//   cursor); being larger than -sdatathreshold 4 it keeps absolute addressing
//   while D_00275B18/D_00275B1C stay gp-relative.

struct S20 { int w[5]; };

extern int func_001195A8(unsigned int a);
extern int func_0010F8F8(int a);
extern int func_0010F968(int a);
extern int func_001FB910(char *a, int b, int c);
extern int func_00119450(int a);
extern int func_001194B8(int a, char *b, int c);
extern int func_001199F0(unsigned int a, int b);

extern struct S20 D_00264890;
extern int D_00281D30[64];
extern int D_00281D50[][20];
struct Loader {
    char outer;          /* 0x00 */
    char state;          /* 0x01 */
    char pad02[7];
    char done;           /* 0x09 */
    char pad0A[2];
    char retry;          /* 0x0C */
    char pad0D[0x33];
    int bucket;          /* 0x40 */
    int buf;             /* 0x44 */
    unsigned int dest;   /* 0x48 */
    char *src;           /* 0x4C */
    int pad50;
    char *cursor;        /* 0x54 */
};
extern struct Loader D_00282150;
extern int D_00275B18;
extern int D_00275B1C;

int func_001FB3E0(char *arg0) {
    struct S20 lut;
    int i;
    char *q;
    char *p;
    char st;
    int r;

    lut = D_00264890;
    q = D_00282150.cursor;
    p = arg0 + 0x20;
    st = D_00282150.state;
    switch (st) {
    case 0:
        D_00282150.state++;
        D_00282150.done = 0;
        D_00282150.bucket = 0x63;
        D_00282150.cursor = p + *(int *)(arg0 + 8) * 16;
        D_00282150.src = arg0 + *(int *)(arg0 + 0x18);
        break;
    case 1:
        if (*(int *)(q + 8) != D_00282150.bucket) {
            D_00282150.bucket = *(int *)(q + 8);
            i = 0;
            while (i < D_00281D30[D_00282150.bucket]) {
                func_001195A8(D_00281D50[D_00282150.bucket][i]);
                i++;
            }
            D_00281D30[D_00282150.bucket] = 0;
            D_00282150.dest = lut.w[D_00282150.bucket];
        }
        D_00282150.buf = func_0010F8F8(*(int *)q + 0x10);
        D_00282150.state++;
        D_00282150.retry = 0;
        break;
    case 2:
        if (func_001FB910(D_00282150.src, D_00282150.buf & ~0xF, *(int *)q) == 0) {
            D_00282150.state++;
        }
        break;
    case 3:
        if (D_00275B1C != 0) {
            D_00282150.state = st + 1;
        } else if (func_00119450(0) != 0) {
            D_00282150.state++;
        }
        break;
    case 4:
        if (D_00282150.dest & 0x3F) {
            D_00282150.dest = D_00282150.dest + 0x40;
            D_00282150.dest = D_00282150.dest & ~0x3F;
        }
        r = func_001194B8(D_00282150.buf & ~0xF, arg0 + *(int *)(q + 4), D_00282150.dest);
        D_00281D50[D_00282150.bucket][D_00281D30[D_00282150.bucket]] = r;
        if (D_00281D50[D_00282150.bucket][D_00281D30[D_00282150.bucket]] == -1) {
            break;
        }
        D_00282150.state++;
        D_00275B18 = 0x50;
    case 5:
        if (D_00275B1C != 0) {
            if (D_00275B18 != 0) {
                D_00275B18 = D_00275B18 - 1;
            } else {
                D_00282150.state++;
                if (func_00119450(0) == 1) {
                    D_00275B1C = 0;
                }
            }
        } else {
            r = func_00119450(0);
            if (r == 1) {
                D_00282150.state++;
            } else if (r < 0) {
                D_00282150.state = 4;
                func_001195A8(D_00281D50[D_00282150.bucket][D_00281D30[D_00282150.bucket]]);
            } else if (D_00275B18 != 0) {
                D_00275B18 = D_00275B18 - 1;
            } else {
                D_00275B1C = 1;
                D_00282150.state++;
            }
        }
        break;
    case 6:
        D_00282150.state = 1;
        func_001199F0(D_00281D50[D_00282150.bucket][D_00281D30[D_00282150.bucket]], 0x64);
        func_0010F968(D_00282150.buf);
        D_00282150.dest = D_00282150.dest + *(int *)q;
        D_00282150.src = D_00282150.src + *(int *)q;
        D_00281D30[D_00282150.bucket] = D_00281D30[D_00282150.bucket] + 1;
        D_00282150.cursor = q + 0x10;
        D_00282150.done = D_00282150.done + 1;
        break;
    }
    if (*(unsigned int *)(arg0 + 0xC) <= (unsigned int)D_00282150.done) {
        D_00282150.outer = 0;
        D_00282150.state = 0;
        return (int)(arg0 + (*(int *)(arg0 + 0x10) + 0x40)) & ~0x3F;
    }
    return 0;
}
