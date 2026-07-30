// NEARMISS func_001FB3E0  (vram 0x001FB3E0, 0x524 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.64% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single jump table, and the jtbl_0026EBF0 dispatch IS byte-identical (reloc clean) — the residual is all body scheduling/regalloc, ~45 instructions of ~350, in five clusters. (a) 20-byte struct copy of D_00264890 into the stack local: CW ends it with `lw v0,0x10(v0); sw v0,0x10(a0)`, mwcc with `lw...
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
//   NOTE: the array over-declarations (D_00282150[8], D_00282190[2], ...) are
//   idiom #20 MATCHING DEVICES: with -sdatathreshold 4 they push these globals
//   past the small-data threshold so they keep the original's absolute
//   lui/%lo addressing while D_00275B18/D_00275B1C stay %gp_rel. Only element
//   [0] is ever touched; the declared sizes carry no semantics. The `volatile`
//   reads are likewise matching devices (they reproduce CW's re-loads).

struct S20 { int w[5]; };

extern int func_001195A8(int a);
extern int func_0010F8F8(int a);
extern void func_0010F968(int a);
extern int func_001FB910(char *a, int b, int c);
extern int func_00119450(int a);
extern int func_001194B8(int a, char *b, int c);
extern void func_001199F0(int a, int b);

extern struct S20 D_00264890;
extern int D_00281D30[64];
extern int D_00281D50[512];
extern char D_00282150[8];
extern char D_00282151[8];
extern char D_00282159[8];
extern char D_0028215C[8];
extern int D_00282190[2];
extern int D_00282194[2];
extern int D_00282198[2];
extern char *D_0028219C[2];
extern char *D_002821A4[2];
extern int D_00275B18;
extern int D_00275B1C;

int func_001FB3E0(char *arg0) {
    struct S20 lut;
    int i;
    char *q;
    int j;
    char *p;
    char st;
    int r;
    int *slot;

    lut = D_00264890;
    q = D_002821A4[0];
    p = arg0 + 0x20;
    st = D_00282151[0];
    switch (st) {
    case 0:
        D_00282151[0] = D_00282151[0] + 1;
        D_00282159[0] = 0;
        D_00282190[0] = 0x63;
        D_002821A4[0] = p + *(int *)(arg0 + 8) * 16;
        D_0028219C[0] = arg0 + *(int *)(arg0 + 0x18);
        break;
    case 1:
        if (*(int *)(q + 8) != D_00282190[0]) {
            D_00282190[0] = *(int *)(q + 8);
            i = 0;
            j = 0;
            while (i < D_00281D30[D_00282190[0]]) {
                func_001195A8(*(int *)(j + ((char *)D_00281D50 + D_00282190[0] * 0x50)));
                j += 4;
                i++;
            }
            D_00281D30[D_00282190[0]] = 0;
            D_00282198[0] = lut.w[D_00282190[0]];
        }
        D_00282194[0] = func_0010F8F8(*(int *)q + 0x10);
        D_00282151[0] = D_00282151[0] + 1;
        D_0028215C[0] = 0;
        break;
    case 2:
        if (func_001FB910(D_0028219C[0], D_00282194[0] & ~0xF, *(int *)q) == 0) {
            D_00282151[0] = D_00282151[0] + 1;
        }
        break;
    case 3:
        if (D_00275B1C != 0) {
            D_00282151[0] = st + 1;
        } else if (func_00119450(0) != 0) {
            D_00282151[0] = D_00282151[0] + 1;
        }
        break;
    case 4:
        if (D_00282198[0] & 0x3F) {
            D_00282198[0] = D_00282198[0] + 0x40;
            D_00282198[0] = D_00282198[0] & ~0x3F;
        }
        r = func_001194B8(D_00282194[0] & ~0xF, arg0 + *(int *)(q + 4), D_00282198[0]);
        slot = (int *)(D_00281D30[D_00282190[0]] * 4 + ((char *)D_00281D50 + D_00282190[0] * 0x50));
        *slot = r;
        if (*slot == -1) {
            break;
        }
        D_00275B18 = 0x50;
        D_00282151[0] = D_00282151[0] + 1;
    case 5:
        if (D_00275B1C != 0) {
            if (D_00275B18 != 0) {
                D_00275B18 = D_00275B18 - 1;
            } else {
                D_00282151[0] = D_00282151[0] + 1;
                if (func_00119450(0) == 1) {
                    D_00275B1C = 0;
                }
            }
        } else {
            r = func_00119450(0);
            if (r == 1) {
                D_00282151[0] = D_00282151[0] + 1;
            } else if (r < 0) {
                D_00282151[0] = 4;
                func_001195A8(*(int *)(D_00281D30[D_00282190[0]] * 4 + ((char *)D_00281D50 + D_00282190[0] * 0x50)));
            } else if (D_00275B18 != 0) {
                D_00275B18 = D_00275B18 - 1;
            } else {
                D_00275B1C = 1;
                D_00282151[0] = D_00282151[0] + 1;
            }
        }
        break;
    case 6:
        D_00282151[0] = 1;
        func_001199F0(*(int *)(D_00281D30[D_00282190[0]] * 4 + ((char *)D_00281D50 + D_00282190[0] * 0x50)), 0x64);
        func_0010F968(D_00282194[0]);
        D_00282198[0] = D_00282198[0] + *(volatile int *)q;
        D_0028219C[0] = D_0028219C[0] + *(volatile int *)q;
        D_00281D30[D_00282190[0]] = D_00281D30[D_00282190[0]] + 1;
        D_002821A4[0] = q + 0x10;
        D_00282159[0] = D_00282159[0] + 1;
        break;
    }
    if ((unsigned int)D_00282159[0] >= *(unsigned int *)(arg0 + 0xC)) {
        D_00282150[0] = 0;
        D_00282151[0] = 0;
        return (int)(arg0 + (*(int *)(arg0 + 0x10) + 0x40)) & ~0x3F;
    }
    return 0;
}
