// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-entity table lookup of a 16-bit value. The entity state byte at arg0+5
// selects which table: states 0x1D and 0x1E use table D_00248B70, everything
// else uses D_00248C50. The table is indexed by the entity sub-type byte at
// arg0+0x275 to get a `short*` row, then arg1 indexes the short within that row.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 96.6%. The `st==0x1D || st==0x1E` test reproduces the target's beq/bne pair
// (fall-through into the shared 0x1D arm), and the two `short*[]` tables emit the
// sll-by-2 (pointer) + sll-by-1 (short index) addressing exactly.
extern short *D_00248B70[];
extern short *D_00248C50[];

short func_0017A0B0(unsigned char *arg0, int arg1) {
    unsigned char st = arg0[5];
    if (st == 0x1D || st == 0x1E) {
        return D_00248B70[arg0[0x275]][arg1];
    }
    return D_00248C50[arg0[0x275]][arg1];
}
