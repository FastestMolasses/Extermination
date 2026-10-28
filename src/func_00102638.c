// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK DMA channel helper: clears the STR (start/busy) bit of a channel's CHCR
// image at *chcr and returns that bit's previous value (1 if the channel was
// marked running). func_00102638 and func_00102658 are identical copies.
typedef struct {
    unsigned int DIR : 1;
    unsigned int pad0 : 1;
    unsigned int MOD : 2;
    unsigned int ASP : 2;
    unsigned int TTE : 1;
    unsigned int TIE : 1;
    unsigned int STR : 1;
    unsigned int pad1 : 7;
    unsigned int TAG : 16;
} tD_CHCR;

int func_00102638(tD_CHCR *chcr)
{
    int str = chcr->STR;

    chcr->STR = 0;
    return str;
}
