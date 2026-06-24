// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame slice: if the global actor record D_008102B0 is in state 1
// (D_008102B4 == 1), not flagged at +0x275, has action byte +0x1F0 == 0x33,
// its anim id +0x20C matches one of two clip ids (D_00248B98 / D_00248C78),
// and its clip time +0x3C is in [12.0, 51.0], copy a quadword block from the
// +0x15C target pointer (+0x90) into the global publish slot (*D_00275B40)+0x90
// via copy_qw4; otherwise copy from the +0x120 pointer instead.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (91.23%):
// the far globals are forced out of small-data by oversizing their array decls
// so only the gp-rel pointer D_00275B40 lands in sdata at threshold 4.
// objdiff 100.0% vs expected.
extern void copy_qw4(char *, char *);
extern short D_00248B98[16];
extern short D_00248C78[16];
extern char **D_00275B40;
extern char D_008102B0[1024];
extern unsigned char D_008102B4[16];

void func_00188B80(void) {
    char *p;

    p = D_008102B0;
    if (D_008102B4[0] == 1 &&
        *(unsigned char *)(p + 0x275) == 0 &&
        *(unsigned char *)(p + 0x1F0) == 0x33 &&
        (*(short *)(p + 0x20C) == D_00248B98[0] || *(short *)(p + 0x20C) == D_00248C78[0]) &&
        *(float *)(p + 0x3C) >= 12.0f &&
        *(float *)(p + 0x3C) <= 51.0f) {
        copy_qw4(*D_00275B40 + 0x90, *(char **)(p + 0x15C) + 0x90);
        return;
    }
    copy_qw4(*D_00275B40 + 0x90, *(char **)(p + 0x120) + 0x90);
}
