// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// SDK (libcdvd, sceCdIntToPos shape): converts a logical sector number to a
// BCD minute/second/sector position: adds the 150-sector (2 s) lead-in, then
// stores sector (i % 75), second ((i / 75) % 60) and minute (i / 75 / 60), each
// as BCD, in that order, and returns p. The quotient is updated in place.

typedef struct {
    unsigned char minute;
    unsigned char second;
    unsigned char sector;
    unsigned char track;
} sceCdlLOCCD;

#define itob(i) ((i) / 10 * 16 + (i) % 10)

sceCdlLOCCD *func_001119B8(int i, sceCdlLOCCD *p)
{
    i += 150;
    p->sector = itob(i % 75);
    i /= 75;
    p->second = itob(i % 60);
    i /= 60;
    p->minute = itob(i);
    return p;
}
