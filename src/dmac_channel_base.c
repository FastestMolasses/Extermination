// COMPILER: eegcc
// CFLAGS: -O2
//
// Returns the DMA channel base address for channel ch (table D_00241050), or 0
// for ch >= 10.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern int D_00241050[];

int dmac_channel_base(unsigned int ch) {
    if (ch < 10) {
        return D_00241050[ch];
    }
    return 0;
}
